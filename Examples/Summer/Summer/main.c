//
//  Copyright 2012-2013, Andrii Mamchur
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


#include <stdlib.h>
#include <assert.h>
#import "JsonLiteObjC/jsonlite.h"

long long sum = 0;

static void number_callback(jsonlite_callback_context *ctx, jsonlite_token *token) {
    sum += jsonlite_token_to_long_long(token);
}

int main(int argc, const char * argv[]) {
    const int JSON_DEPTH = 4;                                           // Limitation of JSON depth
    char json[] = "[-13453453, 0, 1, 123, 45345, -94534555]";           // Numbers to sum
    uint8_t parser_memory[jsonlite_parser_estimate_size(JSON_DEPTH)];   // Parser memory
    jsonlite_parser_callbacks cbs = jsonlite_default_callbacks;         // Init callbacks with default values
    cbs.number_found = &number_callback;                                // Assign new callback function
    jsonlite_parser p = jsonlite_parser_init(parser_memory,
                                             sizeof(parser_memory),
                                             jsonlite_null_buffer);     // Init parser
    jsonlite_parser_set_callback(p, &cbs);                              // Set callback function(s)
    jsonlite_parser_tokenize(p, json, sizeof(json));                    // Tokenize/find numbers
    printf("Total sum: %lld\n", sum);
    return 0;
}