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

#define CAST_TO_MEM_STREAM(S)   (jsonlite_mem_stream *)((uint8_t *)(S) + sizeof(jsonlite_stream_struct))
#define SIZE_OF_MEM_STREAM()    (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_mem_stream))

struct jsonlite_stream_struct {
    jsonlite_stream_write_fn write;
    jsonlite_stream_release_fn release;
} jsonlite_stream_struct;

typedef struct jsonlite_mem_stream_block {
    struct jsonlite_mem_stream_block *next;
    uint8_t *data;
} jsonlite_mem_stream_block;

typedef struct jsonlite_mem_stream {
    size_t block_size;
    uint8_t *cursor;
    uint8_t *limit;
    struct jsonlite_mem_stream_block *first;
    struct jsonlite_mem_stream_block *current;
} jsonlite_mem_stream;

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    if (stream == NULL) {
        return -1;
    }
    
    if (data == NULL) {
        return -1;
    }
    
    return stream->write(stream, data, length);
}

void jsonlite_stream_release(jsonlite_stream stream) {
    if (stream == NULL) {
        return;
    }
    
    if (stream->release != NULL) {
        stream->release(stream);
    }
}

static int jsonlite_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    size_t write_limit = mem_stream->limit - mem_stream->cursor;
    if (write_limit >= length) {
        memcpy(mem_stream->cursor, data, length); // LCOV_EXCL_LINE
        mem_stream->cursor += length;
    } else {
        memcpy(mem_stream->cursor, data, write_limit); // LCOV_EXCL_LINE
        mem_stream->cursor += write_limit;
        
        size_t size = sizeof(jsonlite_mem_stream_block) + mem_stream->block_size;
        jsonlite_mem_stream_block *block = malloc(size);
        mem_stream->current->next = block;
        mem_stream->current = block;
        block->data = (uint8_t *)block + sizeof(jsonlite_mem_stream_block);
        block->next = NULL;
        mem_stream->cursor = block->data;
        mem_stream->limit = block->data + mem_stream->block_size;
        
        jsonlite_mem_stream_write(stream, (char *)data + write_limit, length - write_limit);
    }
    
    return (int)length;
}

static void jsonlite_mem_stream_release(jsonlite_stream stream) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_stream_block *block = mem_stream->first;
    void *prev;
    for (; block != NULL;) {
        prev = block;
        block = block->next;
        free(prev);
    }
    
    free((void *)stream);
}

jsonlite_stream jsonlite_mem_stream_init(size_t block_size) {
    size_t size = SIZE_OF_MEM_STREAM();
    
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_mem_stream_write;
    stream->release = jsonlite_mem_stream_release;

    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_stream_block *first = malloc(sizeof(jsonlite_mem_stream_block) + block_size);
    first->data = (uint8_t *)first + sizeof(jsonlite_mem_stream_block);
    first->next = NULL;

    mem_stream->block_size = block_size;
    mem_stream->first = first;
    mem_stream->current = first;
    mem_stream->cursor = first->data;
    mem_stream->limit = first->data + block_size;
    return stream;
}

size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_stream_block *block = NULL;
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
                memcpy(buff, block->data, mem_stream->block_size); // LCOV_EXCL_LINE
                buff += mem_stream->block_size;
            } else {
                memcpy(buff, block->data, mem_stream->cursor - block->data); // LCOV_EXCL_LINE
            }
        }
    }

    return size;
}

static int jsonlite_null_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return (int)length;
}

static int jsonlite_stdout_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return (int)fwrite(data, 1, length, stdout);
}

static struct jsonlite_stream_struct jsonlit_stdout_stream_struct = {jsonlite_stdout_stream_write, NULL};
static struct jsonlite_stream_struct jsonlit_null_stream_struct = {jsonlite_null_stream_write, NULL};

jsonlite_stream jsonlite_null_stream = &jsonlit_null_stream_struct;
jsonlite_stream jsonlite_stdout_stream = &jsonlit_stdout_stream_struct;
