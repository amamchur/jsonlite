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

#ifndef JSONLITE_STREAM_STATIC__H
#define JSONLITE_STREAM_STATIC__H

#include <jsonlite_stream.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
const void *jsonlite_static_mem_stream_data(jsonlite_stream stream);

#ifdef __cplusplus
}
#endif

#endif
