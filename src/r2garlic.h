#ifndef R2GARLIC_H
#define R2GARLIC_H

#include <r_core.h>

#define R2GARLIC_VERSION "0.1.0"

typedef struct {
    RCore *core;
    char *file_path;
    ut8 *file_buf;
    size_t file_size;
} GarlicContext;

bool r2garlic_init(RCorePluginSession *cps);
bool r2garlic_fini(RCorePluginSession *cps);
bool r2garlic_call(RCorePluginSession *cps, const char *input);

#endif