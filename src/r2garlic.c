#include "r2garlic.h"
#include "memstream.h"

#include <r_core.h>
#include <r_config.h>
#include <r_cons.h>
#include <r_io.h>
#include <r_bin.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/dex/metadata.h"
#include "parser/dex/dex.h"
#include "dalvik/dex_decompile.h"
#include "dalvik/dex_structure.h"
#include "dalvik/dex_class.h"
#include "dalvik/dex_dump.h"
#include "dalvik/dex_smali.h"
#include "decompiler/expression_writter.h"
#include "decompiler/structure.h"
#include "libs/memory/mem_pool.h"
#include "common/types.h"
#include "common/str_tools.h"

extern jd_dex *dex_init(jd_meta_dex *meta, int thread_num);

static void set_node_output_stream(jd_node *node, FILE *stream);

static void set_source_file_output_stream(jsource_file *jf, FILE *stream) {
	if (!jf) {
		return;
	}
	jf->source = stream;
	if (!jf->blocks) {
		return;
	}
	for (size_t i = 0; i < jf->blocks->size; i++) {
		set_node_output_stream (lget_obj (jf->blocks, i), stream);
	}
}

static void set_node_output_stream(jd_node *node, FILE *stream) {
	if (!node) {
		return;
	}
	if ((node->type == JD_NODE_CLASS_ROOT ||
		node->type == JD_NODE_PACKAGE_IMPORT ||
		node->type == JD_NODE_CLASS ||
		node->type == JD_NODE_FIELD) &&
		node->data) {
		jsource_file *jf = (jsource_file *)node->data;
		if (jf->source != stream) {
			set_source_file_output_stream (jf, stream);
		}
	}
	if (!node->children) {
		return;
	}
	for (size_t i = 0; i < node->children->size; i++) {
		set_node_output_stream (lget_obj (node->children, i), stream);
	}
}

static const char *help_msg[] = {
	"Usage: pd:G[cmd] [args]",
	"",
	"Garlic DEX/Dalvik decompiler plugin for radare2",
	"",
	"  pd:G            Decompile the class at current seek",
	"  pd:Ga           Decompile all classes in the DEX",
	"  pd:Gi           Show DEX file info (dexdump)",
	"  pd:Gs           Output smali for current class",
	"  pd:G?           Show this help",
	"",
	"Only works on DEX files (format=dex, arch=dalvik).",
	NULL
};

static void print_help(RCore *core) {
	int i;
	for (i = 0; help_msg[i]; i++) {
		r_cons_println (core->cons, help_msg[i]);
	}
}

static bool is_dex_file(RCore *core) {
	RBinInfo *bi = r_bin_get_info (core->bin);
	if (bi) {
		if (bi->lang && !strcmp (bi->lang, "dalvik")) {
			return true;
		}
		if (bi->rclass && !strcmp (bi->rclass, "DEX CLASS")) {
			return true;
		}
	}
	const char *format = r_config_get (core->config, "bin.format");
	if (format && !strcmp (format, "dex")) {
		return true;
	}
	const char *arch = r_config_get (core->config, "bin.arch");
	if (arch && !strcmp (arch, "dalvik")) {
		return true;
	}
	return false;
}

static char *get_file_path(RCore *core) {
	RBinInfo *bi = r_bin_get_info (core->bin);
	if (bi && bi->file) {
		return strdup (bi->file);
	}
	const char *fp = r_config_get (core->config, "file.path");
	if (fp && *fp) {
		return strdup (fp);
	}
	return NULL;
}

static ut8 *get_file_bytes(RCore *core, size_t *out_size) {
	ut64 sz = r_io_size (core->io);
	if (sz == 0 || sz == UT64_MAX) {
		return NULL;
	}
	ut8 *buf = malloc ((size_t)sz);
	if (!buf) {
		return NULL;
	}
	bool ok = r_io_read_at (core->io, 0, buf, (int)sz);
	if (!ok) {
		free (buf);
		return NULL;
	}
	*out_size = (size_t)sz;
	return buf;
}

static char *decompile_class_to_string(jd_dex *dex, dex_class_def *cf) {
	R2GarlicMemStream ms;
	if (!mem_stream_open (&ms)) {
		return NULL;
	}
	jsource_file *jf = dex_class_inside (dex, cf, NULL);
	if (!jf) {
		mem_stream_discard (&ms);
		return NULL;
	}
	if (jf->parent != NULL) {
		mem_stream_discard (&ms);
		return NULL;
	}
	set_source_file_output_stream (jf, ms.stream);
	writter_for_class (jf, NULL);
	set_source_file_output_stream (jf, NULL);
	return mem_stream_close (&ms);
}

static void find_class_name_from_type(jd_meta_dex *meta, u4 class_idx, char *out, size_t out_size) {
	string name = dex_str_of_type_id (meta, class_idx);
	if (name) {
		const char *p = name;
		if (*p == 'L') {
			p++;
		}
		size_t i = 0;
		while (*p && *p != ';' && i < out_size - 1) {
			out[i++] = (*p == '/')? '.': *p;
			p++;
		}
		out[i] = '\0';
	} else {
		snprintf (out, out_size, "class_%u", class_idx);
	}
}

static bool get_r2_class_name(RCore *core, char *out, size_t out_size) {
	char *ic_result = r_core_cmd_str (core, "ic.");
	if (!ic_result || !*ic_result) {
		free (ic_result);
		return false;
	}
	char *first_line = ic_result;
	char *nl = strchr (first_line, '\n');
	if (nl) {
		*nl = '\0';
	}
	char *colon = strchr (first_line, ':');
	if (!colon) {
		free (ic_result);
		return false;
	}
	const char *classname = colon + 1;
	while (*classname == ':') {
		classname++;
	}
	size_t i = 0;
	while (*classname && *classname != '.' && *classname != ';' && i < out_size - 2) {
		out[i++] = *classname++;
	}
	out[i++] = ';';
	out[i] = '\0';
	free (ic_result);
	return (i > 1);
}

static int find_class_by_name(jd_meta_dex *meta, const char *dex_type_name) {
	dex_header *hdr = meta->header;
	for (int i = 0; i < (int)hdr->class_defs_size; i++) {
		dex_class_def *cf = &meta->class_defs[i];
		string name = dex_str_of_type_id (meta, cf->class_idx);
		if (name && strcmp (name, dex_type_name) == 0) {
			return i;
		}
	}
	return -1;
}

static int find_class_at_seek(jd_meta_dex *meta, ut64 seek_addr) {
	dex_header *hdr = meta->header;
	int found = -1;
	for (int i = 0; i < (int)hdr->class_defs_size; i++) {
		dex_class_def *cf = &meta->class_defs[i];
		if (cf->is_inner || cf->is_anonymous) {
			continue;
		}
		if (cf->class_data_off != 0 && cf->class_data_off <= seek_addr) {
			if (found == -1 || cf->class_data_off > meta->class_defs[found].class_data_off) {
				found = i;
			}
		}
	}
	if (found == -1) {
		for (int i = 0; i < (int)hdr->class_defs_size; i++) {
			dex_class_def *cf = &meta->class_defs[i];
			if (cf->is_inner || cf->is_anonymous) {
				continue;
			}
			found = i;
			break;
		}
	}
	return found;
}

static char *smali_class_to_string(jd_meta_dex *meta, dex_class_def *cf) {
	R2GarlicMemStream ms;
	if (!mem_stream_open (&ms)) {
		return NULL;
	}
	dex_class_def_to_smali (meta, cf, ms.stream);
	return mem_stream_close (&ms);
}

static char *dexdump_to_string(jd_meta_dex *meta) {
	R2GarlicMemStream ms;
	if (!mem_stream_open (&ms)) {
		return NULL;
	}
	dexdump_to_stream (meta, ms.stream);
	return mem_stream_close (&ms);
}

static void cmd_decompile_current(RCore *core, ut8 *file_buf, size_t file_size) {
	if (!file_buf || file_size == 0) {
		r_cons_println (core->cons, "[r2garlic] No file loaded.");
		return;
	}
	mem_init_pool ();
	jd_meta_dex *meta = parse_dex_from_buffer ((char *)file_buf, file_size);
	if (!meta) {
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to parse DEX file.");
		return;
	}
	meta->source_dir = NULL;
	jd_dex *dex = dex_init_without_thread (meta);
	if (!dex) {
		mem_pool_free (meta->pool);
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to init DEX context.");
		return;
	}
	int found = -1;
	char r2_class[256] = { 0 };
	if (get_r2_class_name (core, r2_class, sizeof (r2_class))) {
		found = find_class_by_name (meta, r2_class);
	}
	if (found == -1) {
		found = find_class_at_seek (meta, core->addr);
	}
	if (found == -1) {
		mem_pool_free (meta->pool);
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] No decompilable class found at current seek.");
		return;
	}
	dex_class_def *cf = &meta->class_defs[found];
	char class_name[256] = { 0 };
	find_class_name_from_type (meta, cf->class_idx, class_name, sizeof (class_name));
	r_cons_printf (core->cons, "/* Decompiled by r2garlic (Garlic) - Class: %s */\n", class_name);
	char *result = decompile_class_to_string (dex, cf);
	if (result) {
		r_cons_print (core->cons, result);
		r_cons_newline (core->cons);
		free (result);
	} else {
		r_cons_println (core->cons, "[r2garlic] Decompilation failed for this class.");
	}
	mem_pool_free (meta->pool);
	mem_free_pool ();
}

static void cmd_decompile_all(RCore *core, ut8 *file_buf, size_t file_size) {
	if (!file_buf || file_size == 0) {
		r_cons_println (core->cons, "[r2garlic] No file loaded.");
		return;
	}
	mem_init_pool ();
	jd_meta_dex *meta = parse_dex_from_buffer ((char *)file_buf, file_size);
	if (!meta) {
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to parse DEX file.");
		return;
	}
	meta->source_dir = NULL;
	jd_dex *dex = dex_init_without_thread (meta);
	if (!dex) {
		mem_pool_free (meta->pool);
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to init DEX context.");
		return;
	}
	r_cons_println (core->cons, "[r2garlic] Decompiling all classes...");
	dex_header *hdr = meta->header;
	for (int i = 0; i < (int)hdr->class_defs_size; i++) {
		dex_class_def *cf = &meta->class_defs[i];
		if (cf->is_inner || cf->is_anonymous) {
			continue;
		}
		char class_name[256] = { 0 };
		find_class_name_from_type (meta, cf->class_idx, class_name, sizeof (class_name));
		r_cons_printf (core->cons, "\n/* ===== %s ===== */\n", class_name);
		char *result = decompile_class_to_string (dex, cf);
		if (result) {
			r_cons_print (core->cons, result);
			r_cons_newline (core->cons);
			free (result);
		}
	}
	mem_pool_free (meta->pool);
	mem_free_pool ();
}

static void cmd_dexdump(RCore *core, ut8 *file_buf, size_t file_size) {
	if (!file_buf || file_size == 0) {
		r_cons_println (core->cons, "[r2garlic] No file loaded.");
		return;
	}
	mem_init_pool ();
	jd_meta_dex *meta = parse_dex_from_buffer ((char *)file_buf, file_size);
	if (!meta) {
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to parse DEX file.");
		return;
	}
	meta->source_dir = NULL;
	char *result = dexdump_to_string (meta);
	if (result) {
		r_cons_print (core->cons, result);
		free (result);
	}
	mem_pool_free (meta->pool);
	mem_free_pool ();
}

static void cmd_smali_class(RCore *core, ut8 *file_buf, size_t file_size) {
	if (!file_buf || file_size == 0) {
		r_cons_println (core->cons, "[r2garlic] No file loaded.");
		return;
	}
	mem_init_pool ();
	jd_meta_dex *meta = parse_dex_from_buffer ((char *)file_buf, file_size);
	if (!meta) {
		mem_free_pool ();
		r_cons_println (core->cons, "[r2garlic] Failed to parse DEX file.");
		return;
	}
	meta->source_dir = NULL;
	int found = -1;
	char r2_class[256] = { 0 };
	if (get_r2_class_name (core, r2_class, sizeof (r2_class))) {
		found = find_class_by_name (meta, r2_class);
	}
	if (found == -1) {
		found = find_class_at_seek (meta, core->addr);
	}
	if (found >= 0) {
		dex_class_def *cf = &meta->class_defs[found];
		char *result = smali_class_to_string (meta, cf);
		if (result) {
			r_cons_print (core->cons, result);
			free (result);
		}
	} else {
		r_cons_println (core->cons, "[r2garlic] No class found at current address.");
	}
	mem_pool_free (meta->pool);
	mem_free_pool ();
}

static bool r2garlic_call(RCorePluginSession *cps, const char *input) {
	RCore *core = cps? cps->core: NULL;
	if (!core || !input) {
		return false;
	}
	if (!r_str_startswith (input, "pd:G")) {
		return false;
	}
	if (!is_dex_file (core)) {
		r_cons_println (core->cons, "[r2garlic] Error: This plugin only works on DEX files (format=dex, arch=dalvik).");
		return true;
	}
	GarlicContext *ctx = (GarlicContext *)cps->data;
	if (!ctx) {
		return false;
	}
	char *fp = get_file_path (core);
	if (!ctx->file_path || !fp || strcmp (ctx->file_path, fp? fp: "") != 0) {
		free (ctx->file_path);
		ctx->file_path = fp;
		free (ctx->file_buf);
		ctx->file_buf = NULL;
		ctx->file_size = 0;
		ctx->file_buf = get_file_bytes (core, &ctx->file_size);
	} else {
		free (fp);
	}
	const char *arg = input + 4;
	switch (*arg) {
	case '\0':
	case ' ':
		cmd_decompile_current (core, ctx->file_buf, ctx->file_size);
		break;
	case 'a':
		cmd_decompile_all (core, ctx->file_buf, ctx->file_size);
		break;
	case 'i':
		cmd_dexdump (core, ctx->file_buf, ctx->file_size);
		break;
	case 's':
		cmd_smali_class (core, ctx->file_buf, ctx->file_size);
		break;
	case '?':
	default:
		print_help (core);
		break;
	}
	return true;
}

static bool r2garlic_init(RCorePluginSession *cps) {
	RCore *core = cps? cps->core: NULL;
	if (!core) {
		return false;
	}
	GarlicContext *ctx = calloc (1, sizeof (GarlicContext));
	if (!ctx) {
		return false;
	}
	ctx->core = core;
	cps->data = ctx;
	RConfig *cfg = core->config;
	r_config_lock (cfg, false);
	r_config_set (cfg, "r2garlic.threads", "1");
	r_config_lock (cfg, true);
	return true;
}

static bool r2garlic_fini(RCorePluginSession *cps) {
	if (!cps) {
		return false;
	}
	GarlicContext *ctx = (GarlicContext *)cps->data;
	if (ctx) {
		free (ctx->file_path);
		free (ctx->file_buf);
		free (ctx);
	}
	cps->data = NULL;
	return true;
}

static RCorePlugin r_core_plugin_r2garlic = {
	.meta = {
		.name = "r2garlic",
		.desc = "Garlic DEX/Dalvik decompiler plugin",
		.license = "MIT",
		.author = "AbhiTheModder",
		.version = R2GARLIC_VERSION,
	},
	.call = r2garlic_call,
	.init = r2garlic_init,
	.fini = r2garlic_fini,
};

R_API RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_CORE,
	.data = &r_core_plugin_r2garlic,
	.version = R2_VERSION,
	.abiversion = R2_ABIVERSION,
	.pkgname = "r2garlic"
};
