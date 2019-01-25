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

#ifndef JSONLITE_BUFFER_STATIC_H
#define JSONLITE_BUFFER_STATIC_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    #define JSONLITE_MAX(x, y) (((x) > (y)) ? (x) : (y))
    #define jsonlite_buffer_static_size() (sizeof(jsonlite_buffer_struct))
    #define jsonlite_buffer_static_size_ext(max_token_size, chunk_size) \
    (sizeof(jsonlite_buffer_struct) + JSONLITE_MAX(2 * (max_token_size), (max_token_size) + (chunk_size)))
    
    jsonlite_buffer jsonlite_buffer_static_init(void *mem, size_t size);

#ifdef __cplusplus
}
#endif

#endif
