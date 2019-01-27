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
#include <stdio.h>
#include <stdlib.h>

#define MAX_JSON_DEPTH 4

const char json[] = "[-13453453, 0, 1, 123, 45345, -94534555]";

// Reserving memory for jsonlite parser
// Use jsonlite_parser_estimate_size marco to determinate
// minimum requires mem size
uint8_t parser_memory[jsonlite_parser_estimate_size(MAX_JSON_DEPTH)];

static void number_callback(jsonlite_callback_context *ctx, jsonlite_token *token) {
    jsonlite_token_type type = token->type & jsonlite_token_type_mask;
    if (type == jsonlite_token_number) {
        long long *sum = (long long *)ctx->client_state;
        *sum += jsonlite_token_to_long_long(token);
    }
}

int main(int argc, const char *argv[]) {
    // We are not going to use chunk processing in this example
    // so we don't need extra buffer for incomplete tokens
    // use jsonlite_null_buffer is such case
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);

    // We are not going to process all tokens except numbers
    // initializing other callbacks with default (dummy) callbacks
    jsonlite_parser_callbacks cbs;
    jsonlite_parser_callbacks_init(&cbs);
    cbs.token_found = &number_callback;

    long long total_sum = 0;
    cbs.context.client_state = &total_sum;

    jsonlite_parser_set_callback(p, &cbs);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json));
    assert(result == jsonlite_result_ok);

    printf("Total sum: %lld\n", total_sum);
    return 0;
}
