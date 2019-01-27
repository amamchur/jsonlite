#include "callback_recorder.hpp"

#include <gtest/gtest.h>
#include <fstream>

static void token_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    auto type = t->type & jsonlite_token_type_mask;
    switch (type) {
        case jsonlite_token_null:
            cr->add_record(token_null, t);
            break;
        case jsonlite_token_true:
            cr->add_record(token_true, t);
            break;
        case jsonlite_token_false:
            cr->add_record(token_false, t);
            break;
        case jsonlite_token_key:
            cr->add_record(token_key, t);
            break;
        case jsonlite_token_number:
            cr->add_record(token_number, t);
            break;
        case jsonlite_token_string:
            cr->add_record(token_string, t);
            break;
        default:
            break;
    }
}

static void event_occurred(jsonlite_callback_context *ctx, jsonlite_event event) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    switch (event) {
        case jsonlite_event_none:
            break;
        case jsonlite_event_finished:
            cr->add_record(parse_finished);
            break;
        case jsonlite_event_object_start:
            cr->add_record(begin_object);
            break;
        case jsonlite_event_object_end:
            cr->add_record(end_object);
            break;
        case jsonlite_event_array_start:
            cr->add_record(begin_array);
            break;
        case jsonlite_event_array_end:
            cr->add_record(end_array);
            break;
    }
}

callback_recorder::callback_recorder() {
    cbs.event_occurred = event_occurred;
    cbs.token_found = token_callback;
    cbs.context.client_state = this;
}

void callback_recorder::add_record(record_type type, jsonlite_token *t) {
    if (type == token_string) {
        if (t->type == jsonlite_string_ascii) {
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
        : type(type), token(std::move(token)) {}

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

std::vector<char> read_file_content(const std::string &rel_path) {
    std::string path = TEST_DIR;
    std::string file = path + rel_path;
    std::ifstream stream(file, std::ios::in | std::ios::binary);
    std::vector<char> result;
    if (!stream) {
        return result;
    }

    std::ifstream::char_type buffer[4096];
    do {
        const auto &count = stream.read(buffer, sizeof(buffer)).gcount();
        if (count > 0) {
            result.insert(result.end(), buffer, buffer + count);
        } else {
            break;
        }
    } while (true);
    return result;
}
