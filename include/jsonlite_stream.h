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

#ifndef JSONLITE_STREAM_H
#define JSONLITE_STREAM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct jsonlite_stream_struct;
typedef struct jsonlite_stream_struct const *jsonlite_stream;
typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);

typedef struct jsonlite_stream_struct {
    jsonlite_stream_write_fn write;
} jsonlite_stream_struct;

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);

jsonlite_stream jsonlite_stream_null();

#ifdef __cplusplus
}
#endif

#endif
