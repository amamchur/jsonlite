#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_buffer_dynamic_set(jsonlite_buffer buffer, const void *data, size_t length) {
    if (length > buffer->capacity) {
        free(buffer->mem);
        buffer->mem = malloc(length);
        buffer->capacity = length;
    }

    buffer->size = length;
    memcpy(buffer->mem, data, length);
    return 0;
}

static int jsonlite_buffer_dynamic_append(jsonlite_buffer buffer, const void *data, size_t length) {
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        uint8_t *b = (uint8_t *)malloc(total_size);
        memcpy(b, buffer->mem, buffer->size);

        free(buffer->mem);
        buffer->mem = b;
        buffer->capacity = total_size;
    }

    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    return 0;
}

void jsonlite_buffer_dynamic_cleanup(jsonlite_buffer buffer) {
    if (buffer != NULL) {
        free(buffer->mem);
        buffer->mem = NULL;
        buffer->size = 0;
        buffer->capacity = 0;
    }
}

jsonlite_buffer jsonlite_buffer_dynamic_init(void *mem) {
    struct jsonlite_buffer_struct *buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_buffer_dynamic_set;
    buffer->append_mem = &jsonlite_buffer_dynamic_append;
    buffer->mem = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    return buffer;
}
