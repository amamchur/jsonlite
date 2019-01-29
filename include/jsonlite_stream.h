#ifndef JSONLITE_STREAM_H
#define JSONLITE_STREAM_H

#include <stddef.h>

struct jsonlite_stream_struct;
typedef struct jsonlite_stream_struct const *jsonlite_stream;
typedef int (*jsonlite_stream_write_fn)(jsonlite_stream stream, const void *data, size_t length);

typedef struct jsonlite_stream_struct {
    jsonlite_stream_write_fn write;
} jsonlite_stream_struct;

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length);

jsonlite_stream jsonlite_stream_null();

#endif
