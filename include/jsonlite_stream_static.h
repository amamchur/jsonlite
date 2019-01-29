#ifndef JSONLITE_STREAM_STATIC_H
#define JSONLITE_STREAM_STATIC_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream.h"
#endif

#include <stdint.h>

typedef struct jsonlite_static_mem_stream {
    uint8_t *buffer;
    size_t size;
    size_t written;
    uint8_t *limit;
    int enabled;
} jsonlite_static_mem_stream;

#define jsonlite_static_mem_stream_size() (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_static_mem_stream))

jsonlite_stream jsonlite_stream_static_init(void *buffer, size_t size);
size_t jsonlite_stream_static_written_bytes(jsonlite_stream stream);
const void *jsonlite_stream_static_data(jsonlite_stream stream);

#endif
