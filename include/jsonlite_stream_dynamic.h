#ifndef JSONLITE_STREAM_DYNAMIC_H
#define JSONLITE_STREAM_DYNAMIC_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream.h"
#endif

#include <stdint.h>

typedef struct jsonlite_stream_dynamic_block {
    struct jsonlite_stream_dynamic_block *next;
    uint8_t *data;
} jsonlite_stream_dynamic_block;

typedef struct jsonlite_stream_dynamic {
    size_t block_size;
    uint8_t *cursor;
    uint8_t *limit;
    struct jsonlite_stream_dynamic_block *current;
    struct jsonlite_stream_dynamic_block *first;
} jsonlite_stream_dynamic;

jsonlite_stream jsonlite_stream_dynamic_alloc(size_t block_size);
void jsonlite_stream_dynamic_free(jsonlite_stream stream);
size_t jsonlite_stream_dynamic_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes);

#endif
