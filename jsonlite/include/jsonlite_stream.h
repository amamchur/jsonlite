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

#ifndef JSONLITE_STREAM_H
#define JSONLITE_STREAM_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct jsonlite_stream_struct;
    typedef struct jsonlite_stream_struct const * jsonlite_stream;

    typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);
    typedef void (*jsonlite_stream_release_fn)(jsonlite_stream stream);

    int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);
    void jsonlite_stream_release(jsonlite_stream stream);
    
    jsonlite_stream jsonlite_mem_stream_init(size_t block_size);
    size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data);
   
    extern jsonlite_stream jsonlite_null_stream;
    extern jsonlite_stream jsonlite_stdout_stream;

#ifdef __cplusplus
}
#endif

#endif
