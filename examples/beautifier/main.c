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

#include <jsonlite.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_JSON_DEPTH (16)

typedef struct context {
    jsonlite_parser parser;
    jsonlite_builder builder;
} context;

static void key_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_raw_key(c->builder, t->start, t->end - t->start);
}

static void value_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_raw_value(c->builder, t->start, t->end - t->start);
}

static void string_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_raw_string(c->builder, t->start, t->end - t->start);
}

static void object_begin_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_object_begin(c->builder);
}

static void object_end_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_object_end(c->builder);
}

static void array_begin_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_array_begin(c->builder);
}

static void array_end_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_array_end(c->builder);
}

static void true_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_true(c->builder);
}

static void false_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_false(c->builder);
}

static void null_callback(jsonlite_callback_context *ctx) {
    context *c = (context *)ctx->client_state;
    jsonlite_builder_null(c->builder);
}

int main(int argc, const char *argv[]) {
    context ctx;
    char json[] = "{\"a\":null,\"b\":[1,2,3],\"c\":true,\"d\":{\"a\":1,\"b\":[]},\"e\":false,\"f\":[\"a\",\"a\",\"a\"]}";
    uint8_t parser_memory[jsonlite_parser_estimate_size(MAX_JSON_DEPTH)];
    uint8_t builder_memory[jsonlite_builder_estimate_size(MAX_JSON_DEPTH)];

    ctx.builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stdout_stream);
    jsonlite_builder_set_indentation(ctx.builder, 4);

    ctx.parser = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer);
    jsonlite_parser_callbacks cbs = {.parse_finished = jsonlite_default_callbacks.parse_finished,
                                     .object_start = object_begin_callback,
                                     .object_end = object_end_callback,
                                     .array_start = array_begin_callback,
                                     .array_end = array_end_callback,
                                     .key_found = key_callback,
                                     .string_found = string_callback,
                                     .number_found = value_callback,
                                     .true_found = true_callback,
                                     .false_found = false_callback,
                                     .null_found = null_callback,
                                     .context = {.client_state = &ctx}};
    jsonlite_parser_set_callback(ctx.parser, &cbs);
    jsonlite_parser_tokenize(ctx.parser, json, sizeof(json));
    printf("\n");
    return 0;
}
