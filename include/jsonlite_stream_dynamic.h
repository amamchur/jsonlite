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

#ifndef JSONLITE_STREAM_DYNAMIC_H
#define JSONLITE_STREAM_DYNAMIC_H

#include <jsonlite_stream.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jsonlite_stream_dynamic_block {
    struct jsonlite_stream_dynamic_block *next;
    uint8_t *data;
} jsonlite_stream_dynamic_block;

typedef struct jsonlite_stream_dynamic {
    size_t block_size;
    uint8_t *cursor;
    uint8_t *limit;
    struct jsonlite_stream_dynamic_block *current;
    struct jsonlite_stream_dynamic_block *first;
} jsonlite_stream_dynamic;

jsonlite_stream jsonlite_stream_dynamic_alloc(size_t block_size);
void jsonlite_stream_dynamic_free(jsonlite_stream stream);
size_t jsonlite_stream_dynamic_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes);

#ifdef __cplusplus
}
#endif

#endif
