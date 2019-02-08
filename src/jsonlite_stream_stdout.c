#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream_stdout.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdio.h>

static int jsonlite__stream_stdout_write(jsonlite_stream stream, const void *data, size_t length) {
    (void)(stream);
    jsonlite_stack_check();
    return (int)fwrite(data, 1, length, stdout);
}

jsonlite_stream jsonlite_stream_stdout() {
    jsonlite_stack_check();
    static struct jsonlite_stream_struct jsonlite_stdout_stream_struct = {jsonlite__stream_stdout_write};
    jsonlite_stack_check();
    return &jsonlite_stdout_stream_struct;
}
