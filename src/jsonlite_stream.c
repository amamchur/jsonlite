#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return stream->write(stream, data, length);
}

static int jsonlite_null_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    (void)(stream);
    (void)(data);
    return (int)length;
}

jsonlite_stream jsonlite_stream_null() {
    static struct jsonlite_stream_struct jsonlite_null_stream_struct = {jsonlite_null_stream_write};
    return &jsonlite_null_stream_struct;
}
