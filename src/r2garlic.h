#ifndef R2GARLIC_H
#define R2GARLIC_H

#include <r_core.h>

#define R2GARLIC_VERSION "0.1.2"

typedef struct {
	RCore *core;
	char *file_path;
	ut8 *file_buf;
	size_t file_size;
} GarlicContext;

#endif
