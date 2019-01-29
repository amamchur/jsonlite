#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_null_buffer_set_append(jsonlite_buffer buffer, const void *data, size_t length) {
    return length == 0 ? 0 : -1;
}

jsonlite_buffer jsonlite_null_buffer() {
    static struct jsonlite_buffer_struct jsonlite_null_buffer_struct = {
            NULL,
            0,
            0,
            &jsonlite_null_buffer_set_append,
            &jsonlite_null_buffer_set_append
    };
	return &jsonlite_null_buffer_struct;
}

const void *jsonlite_buffer_data(jsonlite_buffer_const buffer) {
    return buffer->mem;
}

size_t jsonlite_buffer_size(jsonlite_buffer_const buffer) {
    return buffer->size;
}

size_t jsonlite_buffer_capacity(jsonlite_buffer_const buffer) {
    return buffer->capacity;
}

int jsonlite_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->set_mem(buffer, data, length);
}

int jsonlite_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    return buffer->append_mem(buffer, data, length);
}
