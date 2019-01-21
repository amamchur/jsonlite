#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

TEST(parser, should_handle_empty_chunk) {
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, nullptr, 0);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parser, should_handle_bad_empty_chunk) {
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_static_buffer_init(buffer_memory, sizeof(buffer_memory));

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk1[] = "{\"broken-";
    char chunk2[] = "key\":null}";

    jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    result = jsonlite_parser_tokenize(p, chunk2, sizeof(chunk2) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parser, should_handle_out_of_memory_for_null_buffer) {
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk1[] = "{\"broken-";
    jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
    EXPECT_EQ(result, jsonlite_result_out_of_memory);
}

TEST(parser, should_handle_rest_out_of_memory) {
    uint8_t buffer_memory[jsonlite_static_buffer_size() + 5];
    jsonlite_buffer buffer = jsonlite_static_buffer_init(buffer_memory, sizeof(buffer_memory));
    ASSERT_TRUE(buffer != nullptr);

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk[] = "{\"long-long-long-long-broken-";
    jsonlite_result result = jsonlite_parser_tokenize(p, chunk, sizeof(chunk) - 1);
    EXPECT_EQ(result, jsonlite_result_out_of_memory);
}


TEST(parser, should_handle_chunk_out_of_memory) {
    uint8_t buffer_memory[jsonlite_static_buffer_size() + 5];
    jsonlite_buffer buffer = jsonlite_static_buffer_init(buffer_memory, sizeof(buffer_memory));
    ASSERT_TRUE(buffer != nullptr);

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk1[] = "{\"k";
    char chunk2[] = "-long-key-rest\":null}";
    jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    result = jsonlite_parser_tokenize(p, chunk2, sizeof(chunk2) - 1);
    EXPECT_EQ(result, jsonlite_result_out_of_memory);
}

TEST(parser, should_handle_depth_limit) {
    uint8_t buffer_memory[jsonlite_static_buffer_size() + 1024];
    jsonlite_buffer buffer = jsonlite_static_buffer_init(buffer_memory, sizeof(buffer_memory));
    ASSERT_TRUE(buffer != nullptr);

    uint8_t parser_memory[jsonlite_parser_estimate_size(8)];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    ASSERT_TRUE(p != nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk1[] = "[[[[[[[[[[[[[[[[";
    jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
    EXPECT_EQ(result, jsonlite_result_depth_limit);
}

TEST(parser, should_parser_init_params) {
    uint8_t parser_memory[jsonlite_parser_estimate_size(8)];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, 0, jsonlite_null_buffer());
    EXPECT_EQ(p, nullptr);

    p = jsonlite_parser_init(nullptr, sizeof(parser_memory), jsonlite_null_buffer());
    EXPECT_EQ(p, nullptr);

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), nullptr);
    EXPECT_EQ(p, nullptr);
}
