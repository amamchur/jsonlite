#ifndef JSONLITE_BUFFER_STATIC_H
#define JSONLITE_BUFFER_STATIC_H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#define JSONLITE_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define jsonlite_buffer_static_size() (sizeof(jsonlite_buffer_struct))
#define jsonlite_buffer_static_size_ext(max_token_size, chunk_size)                                                                                            \
    (sizeof(jsonlite_buffer_struct) + JSONLITE_MAX(2 * (max_token_size), (max_token_size) + (chunk_size)))

jsonlite_buffer jsonlite_buffer_static_init(void *mem, size_t size);

#endif
