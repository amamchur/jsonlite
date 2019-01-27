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
#include <jsonlite_stream_dynamic.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CAST_TO_MEM_STREAM(S)   (jsonlite_stream_dynamic *)((uint8_t *)(S) + sizeof(jsonlite_stream_struct))
#define SIZE_OF_MEM_STREAM()    (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_stream_dynamic))

static int jsonlite_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_stream_dynamic *mem_stream = CAST_TO_MEM_STREAM(stream);
    size_t write_limit = mem_stream->limit - mem_stream->cursor;
    if (write_limit >= length) {
        memcpy(mem_stream->cursor, data, length);
        mem_stream->cursor += length;
    } else {
        memcpy(mem_stream->cursor, data, write_limit);
        mem_stream->cursor += write_limit;
        
        size_t size = sizeof(jsonlite_stream_dynamic_block) + mem_stream->block_size;
        jsonlite_stream_dynamic_block *block = malloc(size);
        block->data = (uint8_t *)block + sizeof(jsonlite_stream_dynamic_block);
        block->next = NULL;
        
        mem_stream->current->next = block;
        mem_stream->current = block;
        mem_stream->cursor = block->data;
        mem_stream->limit = block->data + mem_stream->block_size;
        
        jsonlite_mem_stream_write(stream, (char *)data + write_limit, length - write_limit);
    }
    
    return (int)length;
}

void jsonlite_stream_dynamic_free(jsonlite_stream stream) {
    jsonlite_stream_dynamic *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_stream_dynamic_block *block = mem_stream->first;
    void *prev;
    for (; block != NULL;) {
        prev = block;
        block = block->next;
        free(prev);
    }
    
    free((void *)stream);
}

jsonlite_stream jsonlite_stream_dynamic_alloc(size_t block_size) {
    size_t size = SIZE_OF_MEM_STREAM();    
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_mem_stream_write;

    jsonlite_stream_dynamic_block *first = malloc(sizeof(jsonlite_stream_dynamic_block) + block_size);
    first->data = (uint8_t *)first + sizeof(jsonlite_stream_dynamic_block);
    first->next = NULL;
    
    jsonlite_stream_dynamic *mem_stream = CAST_TO_MEM_STREAM(stream);
    mem_stream->block_size = block_size;
    mem_stream->cursor = first->data;
    mem_stream->limit = first->data + block_size;
    mem_stream->current = first;
    mem_stream->first = first;
    return stream;
}

size_t jsonlite_stream_dynamic_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes) {
    jsonlite_stream_dynamic *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_stream_dynamic_block *block = NULL;
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
        *data = (uint8_t *)malloc(size + extra_bytes);
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
