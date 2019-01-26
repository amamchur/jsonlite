//
//  Copyright 2012-2019, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_AMALGAMATED
#include <jsonlite_stream.h>
#include <jsonlite_stream_static.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

size_t jsonlite_static_mem_stream_written_bytes(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->written;
}

const void * jsonlite_static_mem_stream_data(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->buffer;
}

static int jsonlite_static_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    size_t write_limit = mem_stream->size - mem_stream->written;
    if (mem_stream->enabled && write_limit >= length) {
        memcpy(mem_stream->buffer + mem_stream->written, data, length); // LCOV_EXCL_LINE
        mem_stream->written += length;
    } else {
        mem_stream->enabled = 0;
        return 0;
    }

    return (int)length;
}

jsonlite_stream jsonlite_static_mem_stream_init(void *buffer, size_t size) {
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
