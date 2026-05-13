#ifndef MEMSTREAM_H
#define MEMSTREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
    FILE *stream;
    char *buf;
    size_t len;
    bool tmpfile_backed;
} R2GarlicMemStream;

bool mem_stream_open(R2GarlicMemStream *ms);
void mem_stream_discard(R2GarlicMemStream *ms);
char *mem_stream_close(R2GarlicMemStream *ms);

#endif
