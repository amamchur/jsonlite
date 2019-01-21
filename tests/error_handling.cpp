#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

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

TEST(parser, should_handle_bad_utf8_token) {
    auto data = read_file_content("/bad_token/bad_utf8_token.json");

    EXPECT_FALSE(data.empty());
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_utf8);
    EXPECT_EQ(result, jsonlite_parser_get_result(p));
}

TEST(parser, should_handle_bad_false_token) {
    auto data = read_file_content("/bad_token/bad_false_token.json");

    EXPECT_FALSE(data.empty());
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
    EXPECT_EQ(result, jsonlite_parser_get_result(p));
}

TEST(parser, should_handle_bad_false_token_2) {
    auto data = read_file_content("/bad_token/bad_false_token_2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_false_token_3) {
    auto data = read_file_content("/bad_token/bad_false_token_3.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_false_token_4) {
    auto data = read_file_content("/bad_token/bad_false_token_4.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_exponent_token) {
    auto data = read_file_content("/bad_token/bad_exponent_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_fraction_token) {
    auto data = read_file_content("/bad_token/bad_fraction_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_fraction_token_2) {
    auto data = read_file_content("/bad_token/bad_fraction_token_2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_int_token) {
    auto data = read_file_content("/bad_token/bad_int_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_int_token2) {
    auto data = read_file_content("/bad_token/bad_int_token2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_int_token3) {
    auto data = read_file_content("/bad_token/bad_int_token3.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_int_token4) {
    auto data = read_file_content("/bad_token/bad_int_token4.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_null_token) {
    auto data = read_file_content("/bad_token/bad_null_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_null_token2) {
    auto data = read_file_content("/bad_token/bad_null_token_2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}


TEST(parser, should_handle_bad_null_token3) {
    auto data = read_file_content("/bad_token/bad_null_token_3.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_oct_token) {
    auto data = read_file_content("/bad_token/bad_oct_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_bad_true_token) {
    auto data = read_file_content("/bad_token/bad_true_token.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_true_token_2) {
    auto data = read_file_content("/bad_token/bad_true_token_2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_bad_true_token_3) {
    auto data = read_file_content("/bad_token/bad_true_token_3.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_fail1) {
    auto data = read_file_content("/fail/fail1.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_object_or_array);
}

TEST(parser, should_handle_fail2) {
    auto data = read_file_content("/fail/fail2.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parser, should_handle_fail3) {
    auto data = read_file_content("/fail/fail3.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_key_or_end);
}

TEST(parser, should_handle_fail4) {
    auto data = read_file_content("/fail/fail4.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail5) {
    auto data = read_file_content("/fail/fail5.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail6) {
    auto data = read_file_content("/fail/fail6.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail9) {
    auto data = read_file_content("/fail/fail9.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_key);
}

TEST(parser, should_handle_fail11) {
    auto data = read_file_content("/fail/fail11.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);
}

TEST(parser, should_handle_fail12) {
    auto data = read_file_content("/fail/fail12.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail13) {
    auto data = read_file_content("/fail/fail13.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_fail14) {
    auto data = read_file_content("/fail/fail14.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_fail15) {
    auto data = read_file_content("/fail/fail15.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parser, should_handle_fail16) {
    auto data = read_file_content("/fail/fail16.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail17) {
    auto data = read_file_content("/fail/fail17.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parser, should_handle_fail19) {
    auto data = read_file_content("/fail/fail19.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_colon);
}

TEST(parser, should_handle_fail20) {
    auto data = read_file_content("/fail/fail20.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail21) {
    auto data = read_file_content("/fail/fail21.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_colon);
}

TEST(parser, should_handle_fail22) {
    auto data = read_file_content("/fail/fail22.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);
}

TEST(parser, should_handle_fail23) {
    auto data = read_file_content("/fail/fail23.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_fail24) {
    auto data = read_file_content("/fail/fail24.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_value);
}

TEST(parser, should_handle_fail25) {
    auto data = read_file_content("/fail/fail25.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_fail26) {
    auto data = read_file_content("/fail/fail26.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parser, should_handle_fail27) {
    auto data = read_file_content("/fail/fail27.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_token);
}

TEST(parser, should_handle_fail28) {
    auto data = read_file_content("/fail/fail28.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parser, should_handle_fail29) {
    auto data = read_file_content("/fail/fail29.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_fail30) {
    auto data = read_file_content("/fail/fail30.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_fail31) {
    auto data = read_file_content("/fail/fail31.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parser, should_handle_fail32) {
    auto data = read_file_content("/fail/fail32.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}


TEST(parser, should_handle_fail33) {
    auto data = read_file_content("/fail/fail33.json");
    EXPECT_FALSE(data.empty());

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    ASSERT_TRUE(p != nullptr);

    jsonlite_result result = jsonlite_parser_tokenize(p, data.data(), data.size());
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);
}
