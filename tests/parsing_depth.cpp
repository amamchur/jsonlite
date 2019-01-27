#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

TEST(parsing_depth, should_handle_depth_limit_for_array) {
    char json[] =  R"json([[[[[[[[]]]]]]]])json";
    uint8_t parser_memory[jsonlite_parser_estimate_size(4)];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_depth_limit);
}

TEST(parsing_depth, should_handle_depth_limit_for_object) {
    char json[] =  R"json({"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{)json";
    uint8_t parser_memory[jsonlite_parser_estimate_size(4)];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_depth_limit);
}
