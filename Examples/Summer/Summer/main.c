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
#include "jsonlite.h"
#include "jsonlite_token.h"

long long sum = 0;

static void number_callback(jsonlite_callback_context *ctx, jsonlite_token *token) {
    char *end = NULL;
    sum += strtoll((const char *)token->start, &end, 10);
    assert(end == (char *)token->end);
}

int main(int argc, const char * argv[]) {
    const int JSON_DEPTH = 4;                                   // Limitation of JSON depth
    char json[] = "[-13453453, 0, 1, 123, 45345, -94534555]";   // Numbers to sum
    jsonlite_parser_callbacks cbs = jsonlite_default_callbacks; // Init callbacks with default values
    cbs.number_found = &number_callback;                        // Assign new callback function
    jsonlite_parser p = jsonlite_parser_init(JSON_DEPTH);       // Init parser with specified depth
    jsonlite_parser_set_callback(p, &cbs);                      // Set callback function(s)
    jsonlite_parser_tokenize(p, json, sizeof(json));            // Tokenize/find numbers
    jsonlite_parser_release(p);                                 // Free resources
    printf("Total sum: %lld", sum);
    return 0;
}