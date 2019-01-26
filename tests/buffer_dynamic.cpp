#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

TEST(dynamic_buffer, should_be_created_and_destroyed) {
    uint8_t mem[jsonlite_heap_buffer_size()];

    jsonlite_buffer buffer = jsonlite_heap_buffer_init(mem);
    EXPECT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 0);

    jsonlite_heap_buffer_cleanup(nullptr);
    jsonlite_heap_buffer_cleanup(buffer);
}

TEST(dynamic_buffer, should_automatically_extends) {
    uint8_t mem[jsonlite_heap_buffer_size()];

    jsonlite_buffer buffer = jsonlite_heap_buffer_init(mem);
    ASSERT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 0);

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    ASSERT_NE(p, nullptr);

    callback_recorder cr;
    jsonlite_parser_set_callback(p, &cr.cbs);

    char chunk1[] = "{\"k";
    char chunk2[] = "-long-key-rest\":null}";
    jsonlite_result result = jsonlite_parser_tokenize(p, chunk1, sizeof(chunk1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 2);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 2);

    char data[] = {'"', 'k'};
    const void *b = jsonlite_buffer_data(buffer);
    int code = memcmp(b, data, sizeof(data));
    EXPECT_EQ(code, 0);

    result = jsonlite_parser_tokenize(p, chunk2, sizeof(chunk2) - 1);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 23);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 23);
    EXPECT_EQ(result, jsonlite_result_ok);

    jsonlite_heap_buffer_cleanup(nullptr);
    jsonlite_heap_buffer_cleanup(buffer);
}
