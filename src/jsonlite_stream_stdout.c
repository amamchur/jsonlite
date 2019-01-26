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
#include <jsonlite_stream.h>
#include <jsonlite_stream_stdout.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int jsonlite_stdout_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return (int)fwrite(data, 1, length, stdout);
}

static struct jsonlite_stream_struct jsonlite_stdout_stream_struct = {jsonlite_stdout_stream_write};

jsonlite_stream jsonlite_stdout_stream() {
    return &jsonlite_stdout_stream_struct;
}
