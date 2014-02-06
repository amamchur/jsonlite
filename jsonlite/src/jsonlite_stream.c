//
//  Copyright 2012-2014, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#ifndef JSONLITE_AMALGAMATED
#include "../include/jsonlite_stream.h"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct jsonlite_stream_struct {
    jsonlite_stream_write_fn write;
    jsonlite_stream_release_fn release;
} jsonlite_stream_struct;

int jsonlite_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return stream->write(stream, data, length);
}

void jsonlite_stream_release(jsonlite_stream stream) {
    if (stream == NULL) {
        return;
    }
    
    if (stream->release != NULL) {
        stream->release(stream);
    }
}

static void jsonlite_stream_free_mem(jsonlite_stream stream) {
    free((void *)stream);
}

#ifdef JSONLITE_HEAP_ENABLED

#define CAST_TO_MEM_STREAM(S)   (jsonlite_mem_stream *)((uint8_t *)(S) + sizeof(jsonlite_stream_struct))
#define SIZE_OF_MEM_STREAM()    (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_mem_stream))

typedef struct jsonlite_mem_stream_block {
    struct jsonlite_mem_stream_block *next;
    uint8_t *data;
} jsonlite_mem_stream_block;

typedef struct jsonlite_mem_stream {
    size_t block_size;
    uint8_t *cursor;
    uint8_t *limit;
    struct jsonlite_mem_stream_block *current;
    struct jsonlite_mem_stream_block *first;
} jsonlite_mem_stream;

static int jsonlite_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    size_t write_limit = mem_stream->limit - mem_stream->cursor;
    if (write_limit >= length) {
        memcpy(mem_stream->cursor, data, length); // LCOV_EXCL_LINE
        mem_stream->cursor += length;
    } else {
        memcpy(mem_stream->cursor, data, write_limit); // LCOV_EXCL_LINE
        mem_stream->cursor += write_limit;
        
        size_t size = sizeof(jsonlite_mem_stream_block) + mem_stream->block_size;
        jsonlite_mem_stream_block *block = malloc(size);
        block->data = (uint8_t *)block + sizeof(jsonlite_mem_stream_block);
        block->next = NULL;
        
        mem_stream->current->next = block;
        mem_stream->current = block;
        mem_stream->cursor = block->data;
        mem_stream->limit = block->data + mem_stream->block_size;
        
        jsonlite_mem_stream_write(stream, (char *)data + write_limit, length - write_limit);
    }
    
    return (int)length;
}

static void jsonlite_mem_stream_release(jsonlite_stream stream) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_stream_block *block = mem_stream->first;
    void *prev;
    for (; block != NULL;) {
        prev = block;
        block = block->next;
        free(prev);
    }
    
    free((void *)stream);
}

jsonlite_stream jsonlite_mem_stream_init(size_t block_size) {
    size_t size = SIZE_OF_MEM_STREAM();    
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_mem_stream_write;
    stream->release = jsonlite_mem_stream_release;

    jsonlite_mem_stream_block *first = malloc(sizeof(jsonlite_mem_stream_block) + block_size);
    first->data = (uint8_t *)first + sizeof(jsonlite_mem_stream_block);
    first->next = NULL;
    
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    mem_stream->block_size = block_size;
    mem_stream->cursor = first->data;
    mem_stream->limit = first->data + block_size;
    mem_stream->current = first;
    mem_stream->first = first;
    return stream;
}

size_t jsonlite_mem_stream_data(jsonlite_stream stream, uint8_t **data, size_t extra_bytes) {
    jsonlite_mem_stream *mem_stream = CAST_TO_MEM_STREAM(stream);
    jsonlite_mem_stream_block *block = NULL;
    uint8_t *buff = NULL;
    size_t size = 0;
    
    for (block = mem_stream->first; block != NULL; block = block->next) {
        if (block->next != NULL) {
            size += mem_stream->block_size;
        } else {
            size += mem_stream->cursor - block->data;
        }
    }
    
    if (size == 0) {
        *data = NULL;
    } else {
        *data = (uint8_t *)malloc(size + extra_bytes);
        buff = *data;
        for (block = mem_stream->first; block != NULL; block = block->next) {
            if (block->next != NULL) {
                memcpy(buff, block->data, mem_stream->block_size); // LCOV_EXCL_LINE
                buff += mem_stream->block_size;
            } else {
                memcpy(buff, block->data, mem_stream->cursor - block->data); // LCOV_EXCL_LINE
            }
        }
    }

    return size;
}

#endif

typedef struct jsonlite_static_mem_stream {
    uint8_t *buffer;
    size_t size;
    size_t written;
    uint8_t *limit;
    int enabled;
} jsonlite_static_mem_stream;

static int jsonlite_static_mem_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    size_t write_limit = mem_stream->size - mem_stream->written;
    if (mem_stream->enabled && write_limit >= length) {
        memcpy(mem_stream->buffer + mem_stream->written, data, length); // LCOV_EXCL_LINE
        mem_stream->written += length;
    } else {
        mem_stream->enabled = 0;
        return 0;
    }
    
    return (int)length;
}

jsonlite_stream jsonlite_static_mem_stream_init(void *buffer, size_t size) {
    int extra_size = (int)size - sizeof(jsonlite_stream_struct) - sizeof(jsonlite_static_mem_stream);
    if (extra_size <= 0) {
        return NULL;
    }
    
    struct jsonlite_stream_struct *stream = (struct jsonlite_stream_struct *)buffer;
    stream->write = jsonlite_static_mem_stream_write;
    stream->release = jsonlite_stream_free_mem;
    
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    mem_stream->buffer = (uint8_t *)mem_stream + sizeof(jsonlite_static_mem_stream);
    mem_stream->size = (size_t)extra_size;
    mem_stream->written = 0;
    mem_stream->enabled = 1;
    return stream;
}

size_t jsonlite_static_mem_stream_written_bytes(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->written;
}

const void * jsonlite_static_mem_stream_data(jsonlite_stream stream) {
    jsonlite_static_mem_stream *mem_stream = (jsonlite_static_mem_stream *)((uint8_t *)stream + sizeof(jsonlite_stream_struct));
    return mem_stream->buffer;
}

#if JSONLITE_FILE_SYSTEM_ENABLED

#define CAST_TO_FILE_STREAM(S)   (jsonlite_file_stream *)((uint8_t *)(S) + sizeof(jsonlite_stream_struct))
#define SIZE_OF_FILE_STREAM()    (sizeof(jsonlite_stream_struct) + sizeof(jsonlite_file_stream))

typedef struct jsonlite_file_stream {
    FILE *file;
} jsonlite_file_stream;

static int jsonlite_file_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    return (int)fwrite(data, 1, length, file_stream->file);
}

jsonlite_stream jsonlite_file_stream_init(FILE *file) {
    size_t size = SIZE_OF_FILE_STREAM();
    struct jsonlite_stream_struct *stream = malloc(size);
    stream->write = jsonlite_file_stream_write;
    stream->release = jsonlite_stream_free_mem;
    
    jsonlite_file_stream *file_stream = CAST_TO_FILE_STREAM(stream);
    file_stream->file = file;
    return stream;
}

#endif

static int jsonlite_null_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return (int)length;
}

static int jsonlite_stdout_stream_write(jsonlite_stream stream, const void *data, size_t length) {
    return (int)fwrite(data, 1, length, stdout);
}

static struct jsonlite_stream_struct jsonlite_stdout_stream_struct = {jsonlite_stdout_stream_write, NULL};
static struct jsonlite_stream_struct jsonlite_null_stream_struct = {jsonlite_null_stream_write, NULL};

jsonlite_stream jsonlite_stdout_stream = &jsonlite_stdout_stream_struct;
jsonlite_stream jsonlite_null_stream = &jsonlite_null_stream_struct;
