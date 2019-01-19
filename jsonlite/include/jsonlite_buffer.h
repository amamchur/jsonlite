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

#ifndef JSONLITE_BUFFER_H
#define JSONLITE_BUFFER_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct jsonlite_buffer_struct *jsonlite_buffer;
    typedef int (*jsonlite_buffer_mem_fn)(jsonlite_buffer buffer, const void *data, size_t length);
    typedef const void * (*jsonlite_buffer_data_fn)(jsonlite_buffer buffer);
    
    typedef struct jsonlite_buffer_struct {
        uint8_t *mem;
        size_t size;
        size_t capacity;
        
        jsonlite_buffer_mem_fn set_mem;
        jsonlite_buffer_mem_fn append_mem;
    } jsonlite_buffer_struct;
    
    int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length);
    int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length);
    const void *jsonlite_buffer_data(jsonlite_buffer buffer);
    size_t jsonlite_buffer_size(jsonlite_buffer buffer);

    #define jsonlite_static_buffer_size() (sizeof(jsonlite_buffer_struct))
    #define jsonlite_static_buffer_size_ext(max_token_size, chunk_size) \
    (sizeof(jsonlite_buffer_struct) + MAX(2 * (max_token_size), (max_token_size) + (chunk_size)))
    
    jsonlite_buffer jsonlite_static_buffer_init(void *mem, size_t size);
    
    #define jsonlite_heap_buffer_size() (sizeof(jsonlite_buffer_struct))
    jsonlite_buffer jsonlite_heap_buffer_init(void *mem);
    void jsonlite_heap_buffer_cleanup(jsonlite_buffer buffer);
    
    extern jsonlite_buffer jsonlite_null_buffer;
    
#ifdef __cplusplus
}
#endif

#endif
