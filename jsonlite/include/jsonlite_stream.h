//
//  Copyright 2012-2016, Andrii Mamchur
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

#ifndef JSONLITE_STREAM_H
#define JSONLITE_STREAM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct jsonlite_stream_struct;
    typedef struct jsonlite_stream_struct const * jsonlite_stream;
    typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);

    typedef struct jsonlite_stream_struct {
        jsonlite_stream_write_fn write;
    } jsonlite_stream_struct;
    
    int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);
    
    typedef struct jsonlite_mem_stream_block {
        struct jsonlite_mem_stream_block *next;
        uint8_t *data;
    } jsonlite_mem_stream_block;
    
    typedef struct jsonlite_mem_stream {
        size_t block_size;
        uint8_t *cursor;
        uint8_t *limit;
        struct jsonlite_mem_stream_block *current;
        struct jsonlite_mem_stream_block *first;
    } jsonlite_mem_stream;

    jsonlite_stream jsonlite_mem_stream_alloc(size_t block_size);
    void jsonlite_mem_stream_free(jsonlite_stream stream);
    size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes);
    
    typedef struct jsonlite_static_mem_stream {
        uint8_t *buffer;
        size_t size;
        size_t written;
        uint8_t *limit;
        int enabled;
    } jsonlite_static_mem_stream;
    
    #define jsonlite_static_mem_stream_size() (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_static_mem_stream))
    
    jsonlite_stream jsonlite_static_mem_stream_init(void *buffer, size_t size);
    size_t jsonlite_static_mem_stream_written_bytes(jsonlite_stream stream);
    const void * jsonlite_static_mem_stream_data(jsonlite_stream stream);
    
    jsonlite_stream jsonlite_file_stream_alloc(FILE *file);
    void jsonlite_file_stream_free(jsonlite_stream stream);
    
    extern jsonlite_stream jsonlite_null_stream;
    extern jsonlite_stream jsonlite_stdout_stream;

#ifdef __cplusplus
}
#endif

#endif
