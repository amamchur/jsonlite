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

#include <stdio.h>

static int jsonlite__stream_stdout_write(jsonlite_stream stream, const void *data, size_t length) {
    (void)(stream);
    return (int)fwrite(data, 1, length, stdout);
}

jsonlite_stream jsonlite_stream_stdout() {
    static struct jsonlite_stream_struct jsonlite_stdout_stream_struct = {jsonlite__stream_stdout_write};
    return &jsonlite_stdout_stream_struct;
}
