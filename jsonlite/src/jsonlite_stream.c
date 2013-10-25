//
//  Copyright 2012-2013, Andrii Mamchur
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
#include "../include/jsonlite_stream.h"
#endif

#include <stdlib.h>
#include <string.h>

#define CAST_TO_MEM_STREAM(S)   (jsonlite_mem_out_stream *)((uint8_t *)(S) + sizeof(jsonlite_out_stream_struct))
#define SIZE_OF_MEM_STREAM()    (sizeof(jsonlite_out_stream_struct) + sizeof(jsonlite_out_stream_struct))

typedef struct jsonlite_mem_out_stream_block {
    struct jsonlite_mem_out_stream_block *next;
    uint8_t *data;
} jsonlite_mem_out_stream_block;

typedef struct jsonlite_mem_out_stream {
    size_t block_size;
    uint8_t *cursor;
    uint8_t *limit;
    struct jsonlite_mem_out_stream_block *first;
    struct jsonlite_mem_out_stream_block *current;
} jsonlite_mem_out_stream;

int jsonlite_out_stream_write(jsonlite_out_stream stream, const void *data, size_t length) {
    if (stream == NULL) {
        return -1;
    }
    
    return stream->write(stream, data, length);
}

static int jsonlite_mem_stream_write(jsonlite_out_stream stream, const void *data, size_t length) {
    jsonlite_mem_out_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    size_t write_limit = mem_stream->limit - mem_stream->cursor;
    if (write_limit >= length) {
        memcpy(mem_stream->cursor, data, length); // LCOV_EXCL_LINE
        mem_stream->cursor += length;
        return (int)length;
    }
    
    return -1;
}

jsonlite_out_stream jsonlite_mem_stream_init(size_t block_size) {
    size_t size = SIZE_OF_MEM_STREAM();
    
    jsonlite_out_stream stream = malloc(size);
    stream->write = jsonlite_mem_stream_write;
    
    jsonlite_mem_out_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_out_stream_block *first = malloc(sizeof(jsonlite_mem_out_stream_block) + block_size);
    first->data = (uint8_t *)first + sizeof(jsonlite_mem_out_stream_block);
    first->next = NULL;
    
    mem_stream->block_size = block_size;
    mem_stream->first = first;
    mem_stream->current = NULL;
    mem_stream->cursor = first->data;
    mem_stream->limit = first->data + block_size;
    return stream;
}

size_t jsonlite_mem_stream_data(jsonlite_out_stream stream, uint8_t **data) {
    jsonlite_mem_out_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_out_stream_block *block = NULL;
    uint8_t *buff = NULL;
    size_t size = 0;
    
    for (block = mem_stream->first; block != NULL; block = block->next) {
        if (block->next != NULL) {
            size += mem_stream->block_size;
        } else {
            size += mem_stream->cursor - block->data;
        }
    }
    
    if (size == 0) {
        *data = NULL;
    } else {
        *data = (uint8_t *)malloc(size);
        buff = *data;
        for (block = mem_stream->first; block != NULL; block = block->next) {
            if (block->next != NULL) {
                memcpy(buff, block->data, mem_stream->block_size);
                buff += mem_stream->block_size;
            } else {
                memcpy(buff, block->data, mem_stream->cursor - block->data);
            }
        }
    }

    return size;
}

void jsonlite_mem_stream_release(jsonlite_out_stream stream) {
    jsonlite_mem_out_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_out_stream_block *block = mem_stream->first;
    void *prev;
    for (; block != NULL;) {
        prev = block;
        block = block->next;
        free(prev);
    }
    
    free(stream);
}
