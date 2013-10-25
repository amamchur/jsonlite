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

struct jsonlite_out_stream_struct;
typedef struct jsonlite_out_stream_struct* jsonlite_out_stream;

typedef int (*jsonlite_out_stream_write_fn)(jsonlite_out_stream stream, const void *data, size_t length);

struct jsonlite_out_stream_struct {
    void *internal;
    
    jsonlite_out_stream_write_fn write;
} jsonlite_out_stream_struct;

int jsonlite_out_stream_write(jsonlite_out_stream stream, const void *data, size_t length);

jsonlite_out_stream jsonlite_mem_stream_init(size_t block_size);
size_t jsonlite_mem_stream_data(jsonlite_out_stream stream, uint8_t **data);
void jsonlite_mem_stream_release(jsonlite_out_stream stream);

#endif
