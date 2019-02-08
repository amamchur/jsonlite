#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream_file.h"
#include "jsonlite_stream.h"
#include "jsonlite_stack_check.h"
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
    jsonlite_stack_check();
    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    jsonlite_stack_check();
    return (int)fwrite(data, 1, length, file_stream->file);
}

jsonlite_stream jsonlite_stream_file_alloc(FILE *file) {
    jsonlite_stack_check();
    size_t size = SIZE_OF_FILE_STREAM();
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_file_stream_write;

    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    file_stream->file = file;
    jsonlite_stack_check();
    return stream;
}

void jsonlite_stream_file_free(jsonlite_stream stream) {
    jsonlite_stack_check();
    free((void *)stream);
}
