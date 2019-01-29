#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream_static.h"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

size_t jsonlite_stream_static_written_bytes(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->written;
}

const void * jsonlite_stream_static_data(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->buffer;
}

static int jsonlite_static_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    size_t write_limit = mem_stream->size - mem_stream->written;
    if (mem_stream->enabled && write_limit >= length) {
        memcpy(mem_stream->buffer + mem_stream->written, data, length);
        mem_stream->written += length;
    } else {
        mem_stream->enabled = 0;
        return 0;
    }

    return (int)length;
}

jsonlite_stream jsonlite_stream_static_init(void *buffer, size_t size) {
    int extra_size = (int)size - sizeof(jsonlite_stream_struct) - sizeof(jsonlite_static_mem_stream);
    if (extra_size <= 0) {
        return NULL;
    }

    struct jsonlite_stream_struct *stream = (struct jsonlite_stream_struct *)buffer;
    stream->write = jsonlite_static_mem_stream_write;

    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    mem_stream->buffer = (uint8_t *)mem_stream + sizeof(jsonlite_static_mem_stream);
    mem_stream->size = (size_t)extra_size;
    mem_stream->written = 0;
    mem_stream->enabled = 1;
    return stream;
}
