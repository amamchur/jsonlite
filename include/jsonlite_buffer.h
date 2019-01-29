#ifndef JSONLITE_BUFFER_H
#define JSONLITE_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct jsonlite_buffer_struct *jsonlite_buffer;
typedef const struct jsonlite_buffer_struct *jsonlite_buffer_const;

typedef int (*jsonlite_buffer_mem_fn)(jsonlite_buffer buffer, const void *data, size_t length);

typedef struct jsonlite_buffer_struct {
    uint8_t *mem;
    size_t size;
    size_t capacity;

    jsonlite_buffer_mem_fn set_mem;
    jsonlite_buffer_mem_fn append_mem;
} jsonlite_buffer_struct;

int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length);

int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length);

const void *jsonlite_buffer_data(jsonlite_buffer_const buffer);

size_t jsonlite_buffer_size(jsonlite_buffer_const buffer);

size_t jsonlite_buffer_capacity(jsonlite_buffer_const buffer);

jsonlite_buffer jsonlite_null_buffer();

#endif
