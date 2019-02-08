#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#include "jsonlite_stack_check.h"
#endif

#include <stdlib.h>
#include <string.h>

static int jsonlite_static_buffer_set_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    jsonlite_stack_check();
    if (length > buffer->capacity) {
        return -1;
    }

    buffer->size = length;
    memcpy(buffer->mem, data, length);
    jsonlite_stack_check();
    return 0;
}

static int jsonlite_static_buffer_append_mem(jsonlite_buffer buffer, const void *data, size_t length) {
    jsonlite_stack_check();
    size_t total_size = buffer->size + length;
    if (total_size > buffer->capacity) {
        return -1;
    }

    memcpy(buffer->mem + buffer->size, data, length);
    buffer->size = total_size;
    jsonlite_stack_check();
    return 0;
}

jsonlite_buffer jsonlite_buffer_static_init(void *mem, size_t size) {
    jsonlite_stack_check();
    if (mem == NULL) {
        return NULL;
    }

    if (size < sizeof(jsonlite_buffer_struct)) {
        return NULL;
    }

    struct jsonlite_buffer_struct *buffer = (struct jsonlite_buffer_struct *)mem;
    buffer->set_mem = &jsonlite_static_buffer_set_mem;
    buffer->append_mem = &jsonlite_static_buffer_append_mem;
    buffer->mem = (uint8_t *)mem + sizeof(jsonlite_buffer_struct);
    buffer->size = 0;
    buffer->capacity = size - sizeof(jsonlite_buffer_struct);
    jsonlite_stack_check();
    return buffer;
}
