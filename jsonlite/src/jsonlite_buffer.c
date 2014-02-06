//
//  Copyright 2012-2014, Andrii Mamchur
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
#include "jsonlite_buffer.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_null_buffer_set_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return length == 0 ? 0 : -1;
}

static void jsonlite_null_buffer_cleanup(jsonlite_buffer buffer) {
}

struct jsonlite_buffer_struct jsonlite_null_buffer_struct = {
    NULL,
    0,
    0,
    &jsonlite_null_buffer_set_append_mem,
    &jsonlite_null_buffer_set_append_mem,
    &jsonlite_null_buffer_cleanup
};

jsonlite_buffer jsonlite_null_buffer = &jsonlite_null_buffer_struct;

const void *jsonlite_buffer_data(jsonlite_buffer buffer) {
    return buffer->mem;
}

size_t jsonlite_buffer_size(jsonlite_buffer buffer) {
    return buffer->size;
}

int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->set_mem(buffer, data, length);
}

int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->append_mem(buffer, data, length);
}

void jsonlite_buffer_cleanup(jsonlite_buffer buffer) {
    if (buffer != NULL) {
        buffer->cleanup(buffer);
    }
}

static int jsonlite_static_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    if (length > buffer->capacity) {
        return -1;
    }
    
    buffer->size = length;
    memcpy(buffer->mem, data, length);
    return 0;
}

static int jsonlite_static_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        return -1;
    }
    
    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    return 0;
}

jsonlite_buffer jsonlite_static_buffer_init_memory(void *mem) {
    struct jsonlite_buffer_struct *buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_static_buffer_set_mem;
    buffer->append_mem = &jsonlite_static_buffer_append_mem;
    buffer->cleanup = &jsonlite_null_buffer_cleanup;
    buffer->mem = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    return buffer;
}

#if JSONLITE_HEAP_ENABLED

static int jsonlite_heap_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    if (length > buffer->capacity) {
        free(buffer->mem);
        buffer->mem = malloc(length);
    }
    
    buffer->size = length;
    memcpy(buffer->mem, data, length);
    return 0;
}

static int jsonlite_heap_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        uint8_t *b = (uint8_t *)malloc(total_size);
        memcpy(b, buffer->mem, buffer->size);
        
        free(buffer->mem);
        buffer->mem = b;
    }
    
    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    return 0;
}

static void jsonlite_heap_buffer_cleanup(jsonlite_buffer buffer) {
    free(buffer->mem);
    buffer->mem = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
}

jsonlite_buffer jsonlite_heap_buffer_init_memory(void *mem) {
    struct jsonlite_buffer_struct *buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_heap_buffer_set_mem;
    buffer->append_mem = &jsonlite_heap_buffer_append_mem;
    buffer->cleanup = &jsonlite_heap_buffer_cleanup;
    buffer->mem = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    return buffer;
}

#endif
