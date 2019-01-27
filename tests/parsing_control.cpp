#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

static void null_callback(jsonlite_callback_context *ctx, jsonlite_event event) {
}

static void suspend_callback(jsonlite_callback_context *ctx, jsonlite_token *t) {
    auto *cr = reinterpret_cast<callback_recorder *>(ctx->client_state);
    cr->add_record(token_string, t);
    jsonlite_parser_suspend(ctx->parser);
}

TEST(parser, should_suspend_resume_parsing) {
    std::string path = TEST_DIR;
    std::string file = path + "/success/object_values.json";

    std::fstream f(file, std::ios::in | std::ios::binary);
    char buffer[4096];
    f.read(buffer, sizeof(buffer));
    auto count = static_cast<size_t>(f.gcount());
    EXPECT_TRUE(count < sizeof(buffer));

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_resume(p);
    EXPECT_EQ(result, jsonlite_result_not_allowed);

    result = jsonlite_parser_suspend(p);
    EXPECT_EQ(result, jsonlite_result_not_allowed);

    callback_recorder cr;
    cr.cbs.token_found = &suspend_callback;
    cr.cbs.event_occurred = &null_callback;
    jsonlite_parser_set_callback(p, &cr.cbs);

    result = jsonlite_parser_tokenize(p, buffer, count);
    while (result == jsonlite_result_suspended) {
        result = jsonlite_parser_resume(p);
    }

    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ(cr.records.size(), 60);
}
