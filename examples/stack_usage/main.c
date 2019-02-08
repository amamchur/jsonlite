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
    jsonlite_stack_check_init();
    printf("Initial stack usage (should be signed machine infinite): %ti bytes\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    size_t n = 1000;
    size_t value = arithmetic_progression_sum(n);
    printf("Arithmetic progression sum of %zu = %zu\n", n, value);
    printf("Used %ti bytes of stack memory\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);

    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, strlen(json_1));
    assert(result == jsonlite_result_ok);

    printf("jsonlite validation\n");
    printf("Used %ti bytes of stack memory (case 1)\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);
    result = jsonlite_parser_tokenize(p, json_2, strlen(json_2));
    assert(result == jsonlite_result_ok);
    printf("jsonlite validation\n");
    printf("Used %ti bytes of stack memory (case 2)\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);
    result = jsonlite_parser_tokenize(p, json_3, strlen(json_3));
    assert(result == jsonlite_result_ok);
    printf("jsonlite validation\n");
    printf("Used %ti bytes of stack memory (case 3)\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);
    result = jsonlite_parser_tokenize(p, json_4, strlen(json_4));
    assert(result == jsonlite_result_ok);
    printf("jsonlite validation\n");
    printf("Used %ti bytes of stack memory (case 4)\n\n", jsonlite_stack_used());

    jsonlite_stack_check_init();
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    assert(p != NULL);

    jsonlite_parser_callbacks cbs;
    jsonlite_parser_callbacks_init(&cbs);
    cbs.token_found = &token_callback;
    jsonlite_parser_set_callback(p, &cbs);
    result = jsonlite_parser_tokenize(p, json_4, strlen(json_4));
    assert(result == jsonlite_result_ok);
    printf("jsonlite validation with jsonlite_token_to_uft16\n");
    printf("Used %ti bytes of stack memory (case 4)\n\n", jsonlite_stack_used());
}
