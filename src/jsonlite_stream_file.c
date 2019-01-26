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
#include "jsonlite_stream_file.h"
#include "jsonlite_stream.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CAST_TO_FILE_STREAM(S) (jsonlite_file_stream *)((uint8_t *)(S) + sizeof(jsonlite_stream_struct))
#define SIZE_OF_FILE_STREAM() (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_file_stream))

typedef struct jsonlite_file_stream {
    FILE *file;
} jsonlite_file_stream;

static int jsonlite_file_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    return (int)fwrite(data, 1, length, file_stream->file);
}

jsonlite_stream jsonlite_stream_file_alloc(FILE *file) {
    size_t size = SIZE_OF_FILE_STREAM();
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_file_stream_write;

    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    file_stream->file = file;
    return stream;
}

void jsonlite_stream_file_free(jsonlite_stream stream) {
    free((void *)stream);
}
