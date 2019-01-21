#include "callback_recorder.hpp"

#include <gtest/gtest.h>

static void key_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_key, t);
}

static void number_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_number, t);
}

static void string_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_string, t);
}

static void object_begin_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(begin_object);
}

static void object_end_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(end_object);
}

static void array_begin_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(begin_array);
}

static void array_end_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(end_array);
}

static void true_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_true);
}

static void false_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_false);
}

static void null_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_null);
}

static void finished_callback(jsonlite_callback_context *ctx) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(parse_finished);
}

callback_recorder::callback_recorder() {
    cbs.parse_finished = finished_callback;
    cbs.object_start = object_begin_callback;
    cbs.object_end = object_end_callback;
    cbs.array_start = array_begin_callback;
    cbs.array_end = array_end_callback;
    cbs.key_found = key_callback;
    cbs.string_found = string_callback;
    cbs.number_found = number_callback;
    cbs.true_found = true_callback;
    cbs.false_found = false_callback;
    cbs.null_found = null_callback;
    cbs.context.client_state = this;
}

void callback_recorder::add_record(record_type type, jsonlite_token *t) {
    if (type == token_string) {
        if (t->type.string == jsonlite_string_ascii) {
            records.emplace_back(type, std::string(t->start, t->end));
            return;
        }

        auto buffer = new char[jsonlite_token_size_of_uft8(t)];
        auto size = jsonlite_token_to_uft8(t, buffer);
        records.emplace_back(type, std::string(buffer, buffer + size));
        delete[] buffer;
        return;
    }

    records.emplace_back(type, std::string(t->start, t->end));
}

void callback_recorder::add_record(record_type type) {
    records.emplace_back(type);
}

void callback_recorder::expect_finished() const {
    ASSERT_FALSE(records.empty());
    ASSERT_EQ(records.back().type, parse_finished);
}

record::record(record_type type, std::string token)
    : type(type)
    , token(std::move(token)) {}

record::record(record_type type)
    : type(type) {}

void record::expect_eq(const record &r) const {
    EXPECT_EQ(type, r.type);
    EXPECT_EQ(token, r.token);
}

void record::expect_eq(record_type t) const {
    EXPECT_EQ(type, t);
}

void record::expect_eq(record_type t, const std::string &tk) const {
    EXPECT_EQ(type, t);
    EXPECT_EQ(token, tk);
}
