#ifndef JSONLITE_STREAM_FILE_H
#define JSONLITE_STREAM_FILE_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_stream.h"
#endif

#include <stdio.h>

jsonlite_stream jsonlite_stream_file_alloc(FILE *file);
void jsonlite_stream_file_free(jsonlite_stream stream);

#endif
