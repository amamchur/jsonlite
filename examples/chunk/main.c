//
//  Copyright 2012-2019, Andrii Mamchur
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

#include <assert.h>
#include <jsonlite.h>
#include <string.h>

#define MAX_CHUNK_SIZE 64
#define MAX_JSON_DEPTH 6
#define MAX_JSON_TOKEN_SIZE 20 // See "long-long-long-key", including double quotes

// Reserving space for jsonlite is global memory
// You can use stack for this purpose, but be careful is case of kernel driver
// or MCU development, just keep platform limitation in mind
uint8_t parser_memory[jsonlite_parser_estimate_size(MAX_JSON_DEPTH)];

// Buffer size have to be greater then MAX_CHUNK_SIZE + MAX_JSON_TOKEN_SIZE
// and 2 * MAX_JSON_TOKEN_SIZE
uint8_t buffer_memory[jsonlite_buffer_static_size_ext(MAX_JSON_TOKEN_SIZE, MAX_CHUNK_SIZE)];

const char json_chunk_1[] = "{\"a\":null,\"b\":[1,2,3],\"long-long-";
const char json_chunk_2[] = "long-key\":true,\"d\":{\"a\":[1234312";
const char json_chunk_3[] = "123321],\"b\":[]},\"e\":false,\"f\":[\"a\",\"a\",\"a\"]}";

char *longest_token = NULL;
intptr_t longest_token_size = 0;

static void measure_token_size_callback(jsonlite_callback_context *ctx, jsonlite_token *token) {
    intptr_t size = token->end - token->start;
    if (size <= longest_token_size) {
        return;
    }

    free(longest_token);
    longest_token = (char *)malloc((size_t)(size + 1));
    strncpy(longest_token, (const char *)token->start, (size_t)size);
    longest_token[size] = 0;
    longest_token_size = size;
}

int main(int argc, const char *argv[]) {
    // The longest JSON token should fill data chunk
    assert(MAX_CHUNK_SIZE > MAX_JSON_TOKEN_SIZE);

    // Just checking if all chunks matches memory limits
    assert(MAX_CHUNK_SIZE > sizeof(json_chunk_1));
    assert(MAX_CHUNK_SIZE > sizeof(json_chunk_2));
    assert(MAX_CHUNK_SIZE > sizeof(json_chunk_3));

    printf("Total memory used for parsing: %d bytes\n", (int)(sizeof(parser_memory) + sizeof(buffer_memory)));

    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    assert(buffer != NULL);

    jsonlite_parser parser = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    assert(parser != NULL);

    longest_token = malloc(10);
    strcpy(longest_token, "false");
    longest_token_size = strlen(longest_token);

    jsonlite_parser_callbacks cbs;
    jsonlite_parser_callbacks_init(&cbs);
    cbs.number_found = &measure_token_size_callback;
    cbs.string_found = &measure_token_size_callback;
    cbs.key_found = &measure_token_size_callback;
    jsonlite_parser_set_callback(parser, &cbs);

    // C-string literal always contains '\0' (null char) in the end
    // That's why we are using sizeof(xxx) - 1 instead of sizeof(xxx)
    // otherwise we will get invalid_token error
    jsonlite_result result = jsonlite_parser_tokenize(parser, json_chunk_1, sizeof(json_chunk_1) - 1);
    assert(result == jsonlite_result_end_of_stream);

    result = jsonlite_parser_tokenize(parser, json_chunk_2, sizeof(json_chunk_2) - 1);
    assert(result == jsonlite_result_end_of_stream);

    result = jsonlite_parser_tokenize(parser, json_chunk_3, sizeof(json_chunk_3) - 1);
    assert(result == jsonlite_result_ok);

    printf("The longest token is \"%s\"; size: %d\n", longest_token, (int)longest_token_size);
    free(longest_token);

    return 0;
}
