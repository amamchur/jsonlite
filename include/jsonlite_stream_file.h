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

#ifndef JSONLITE_STREAM_FILE_H
#define JSONLITE_STREAM_FILE_H

#include <jsonlite_stream.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

jsonlite_stream jsonlite_file_stream_alloc(FILE *file);
void jsonlite_file_stream_free(jsonlite_stream stream);

#ifdef __cplusplus
}
#endif

#endif
