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
#include "jsonlite_buffer.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_buffer_dynamic_set(jsonlite_buffer buffer, const void *data, size_t length) {
    if (length > buffer->capacity) {
        free(buffer->mem);
        buffer->mem = malloc(length);
        buffer->capacity = length;
    }

    buffer->size = length;
    memcpy(buffer->mem, data, length);
    return 0;
}

static int jsonlite_buffer_dynamic_append(jsonlite_buffer buffer, const void *data, size_t length) {
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        uint8_t *b = (uint8_t *)malloc(total_size);
        memcpy(b, buffer->mem, buffer->size);

        free(buffer->mem);
        buffer->mem = b;
        buffer->capacity = total_size;
    }

    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    return 0;
}

void jsonlite_buffer_dynamic_cleanup(jsonlite_buffer buffer) {
    if (buffer != NULL) {
        free(buffer->mem);
        buffer->mem = NULL;
        buffer->size = 0;
        buffer->capacity = 0;
    }
}

jsonlite_buffer jsonlite_buffer_dynamic_init(void *mem) {
    struct jsonlite_buffer_struct *buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_buffer_dynamic_set;
    buffer->append_mem = &jsonlite_buffer_dynamic_append;
    buffer->mem = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    return buffer;
}
