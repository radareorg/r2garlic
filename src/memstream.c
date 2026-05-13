#include "memstream.h"

#include <stdlib.h>
#include <string.h>

static char *empty_string(void) {
    char *out = malloc(1);
    if (out) {
        out[0] = '\0';
    }
    return out;
}

bool mem_stream_open(R2GarlicMemStream *ms) {
    if (!ms) {
        return false;
    }
    memset(ms, 0, sizeof(*ms));
#ifdef _WIN32
    ms->stream = tmpfile();
    ms->tmpfile_backed = true;
#else
    ms->stream = open_memstream(&ms->buf, &ms->len);
#endif
    return ms->stream != NULL;
}

void mem_stream_discard(R2GarlicMemStream *ms) {
    if (!ms || !ms->stream) {
        return;
    }
    fclose(ms->stream);
    ms->stream = NULL;
    free(ms->buf);
    ms->buf = NULL;
    ms->len = 0;
}

char *mem_stream_close(R2GarlicMemStream *ms) {
    if (!ms || !ms->stream) {
        return NULL;
    }
    fflush(ms->stream);
    if (ms->tmpfile_backed) {
        long len;
        char *out;
        if (fseek(ms->stream, 0, SEEK_END) != 0) {
            mem_stream_discard(ms);
            return NULL;
        }
        len = ftell(ms->stream);
        if (len < 0 || fseek(ms->stream, 0, SEEK_SET) != 0) {
            mem_stream_discard(ms);
            return NULL;
        }
        out = malloc((size_t)len + 1);
        if (!out) {
            mem_stream_discard(ms);
            return NULL;
        }
        if (len > 0 && fread(out, 1, (size_t)len, ms->stream) != (size_t)len) {
            free(out);
            mem_stream_discard(ms);
            return NULL;
        }
        out[len] = '\0';
        fclose(ms->stream);
        ms->stream = NULL;
        return out;
    }
    if (fclose(ms->stream) != 0) {
        ms->stream = NULL;
        free(ms->buf);
        ms->buf = NULL;
        ms->len = 0;
        return NULL;
    }
    ms->stream = NULL;
    if (!ms->buf) {
        return empty_string();
    }
    return ms->buf;
}
