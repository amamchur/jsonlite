/*
 * Copyright 2012-2021, Andrii Mamchur
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http:www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
*/

#include <assert.h>
#include <jsonlite.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifndef JSONLIE_STACK_CHECK
#error "Example & library should be compiled with JSONLIE_STACK_CHECK"
#endif

static size_t arithmetic_progression_sum(size_t n) {
    jsonlite_stack_check();

    if (n == 0) {
        return 0;
    }

    jsonlite_stack_check();
    return n + arithmetic_progression_sum(n - 1);
}

#define MAX_JSON_DEPTH 64

const char json_1[] = "[]";
const char json_2[] = "{}";
const char json_3[] = "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";
const char json_4[] = "{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{}}}}}}}}}}}}}";

uint8_t parser_memory[jsonlite_parser_estimate_size(MAX_JSON_DEPTH)];

static void token_callback(jsonlite_callback_context *ctx, jsonlite_token *token) {
    jsonlite_token_type type = token->type & jsonlite_token_type_mask;
    wchar_t unicode[64];
    if (type == jsonlite_token_key) {
        jsonlite_token_to_uft16(token, unicode);
        wprintf(L"Unicode token %p", unicode);
    }
}

int main(int argc, const char *argv[]) {

	return 0;
}
