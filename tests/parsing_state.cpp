#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <jsonlite_buffer_static.h>
#include <stdint.h>
#include <string>

TEST(parsing_state, skip_whitespaces) {
    char simple_object[] = "\r\n\t {} \t\n\r";
    char simple_object_2[] = "\r\n\t {\r\n\t} \t\n\r";

    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    jsonlite_result result = jsonlite_parser_tokenize(p, simple_object, sizeof(simple_object));
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ((void *)p->cursor, (void *)&simple_object[5]);

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    result = jsonlite_parser_tokenize(p, simple_object_2, sizeof(simple_object_2));
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ((void *)p->cursor, (void *)&simple_object_2[8]);

    char array_object[] = "\r\n\t [] \t\n\r";
    char array_object_2[] = "\r\n\t [\r\n\t] \t\n\r";

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    result = jsonlite_parser_tokenize(p, array_object, sizeof(array_object));
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ((void *)p->cursor, (void *)&array_object[5]);

    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    result = jsonlite_parser_tokenize(p, array_object_2, sizeof(array_object_2));
    EXPECT_EQ(result, jsonlite_result_ok);
    EXPECT_EQ((void *)p->cursor, (void *)&array_object_2[8]);
}

TEST(parsing_state, skip_whitespaces_eos) {
    char white_spaces[] = "\r\n\t ";
    uint8_t parser_memory[1024];
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer());
    jsonlite_result result = jsonlite_parser_tokenize(p, white_spaces, sizeof(white_spaces) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, parse_negative_number_eos) {
    char json[] = "[-";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, parse_negative_number_not_a_digit) {
    char json1[] = "[-z";
    char json2[] = "[- ";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json1, sizeof(json1) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_number);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json2, sizeof(json2) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parsing_state, parse_exponent_or_fraction_eos) {
    char json[] = "[0";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, parse_exponent_or_fraction_zero_end) {
    char json_1[] = "[0 ";
    char json_2[] = "[0\r";
    char json_3[] = "[0\n";
    char json_4[] = "[0\t";
    char json_5[] = "[0}";
    char json_6[] = "[0]";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, fraction_start_eos) {
    char json[] = "[0.";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, fraction_eos) {
    char json[] = "[0.1";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, fraction_not_a_digit) {
    char json1[] = "[0.z";
    char json2[] = "[0. ";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json1, sizeof(json1) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_number);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json2, sizeof(json2) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_number);
}

TEST(parsing_state, fraction_end) {
    char json_1[] = "[0.1 ";
    char json_2[] = "[0.1\r";
    char json_3[] = "[0.1\n";
    char json_4[] = "[0.1\t";
    char json_5[] = "[0.1}";
    char json_6[] = "[0.1]";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, exponent_eos) {
    char json[] = "[10e";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, exponent_number_eos) {
    char json[] = "[10e2";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, exponent_sign_eos) {
    char json[] = "[10e-";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, exponent_end) {
    char json_1[] = "[1e2 ";
    char json_2[] = "[1e2\r";
    char json_3[] = "[1e2\n";
    char json_4[] = "[1e2\t";
    char json_5[] = "[1e2}";
    char json_6[] = "[1e2]";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, digit_eos) {
    char json[] = "[10";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);
}

TEST(parsing_state, digit_end) {
    char json_1[] = "[12 ";
    char json_2[] = "[12\r";
    char json_3[] = "[12\n";
    char json_4[] = "[12\t";
    char json_5[] = "[12}";
    char json_6[] = "[12]";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_expected_comma_or_end);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, utf8_hex) {
    char json_1[] = R"json(["\u0000"])json";
    char json_2[] = R"json(["\ua000"])json";
    char json_3[] = R"json(["\uA000"])json";
    char json_4[] = R"json(["\u0a000"])json";
    char json_5[] = R"json(["\u0A00"])json";
    char json_6[] = R"json(["\u00a0"])json";
    char json_7[] = R"json(["\u00A0"])json";
    char json_8[] = R"json(["\u000a"])json";
    char json_9[] = R"json(["\u000A"])json";
    char json_10[] = R"json(["\uFFFF"])json";
    char json_11[] = R"json(["\uFDD0"])json";

    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_7, sizeof(json_7) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_8, sizeof(json_8) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_9, sizeof(json_9) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_10, sizeof(json_10) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_11, sizeof(json_11) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, utf8_hex_validation) {
    char json_1[] = R"json(["\uzzzz"])json";
    char json_2[] = R"json(["\u    "])json";
    char json_3[] = R"json(["\u0zzz"])json";
    char json_4[] = R"json(["\u0   "])json";
    char json_5[] = R"json(["\u00zz"])json";
    char json_6[] = R"json(["\u00  "])json";
    char json_7[] = R"json(["\u000z"])json";
    char json_8[] = R"json(["\u000 "])json";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_7, sizeof(json_7) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_8, sizeof(json_8) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parsing_state, utf8) {
    char json_1[] = R"json(["הקליטה"])json";
    char json_2[] = R"json(["いろはにほへとちりぬるを"])json";
    char json_3[] = R"json(["𝄞 ♔♩ ♪ ♫ ♬ ♭ ♮ ♯"])json";
    char json_4[] = R"json(["Γαζέες καὶ μυρτιὲς"])json";

    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, utf16_noncharacter) {
    char json_1[] = R"json(["\uDBFF\uDFFF"])json";
    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}

TEST(parsing_state, utf16_surrogate_validation) {
    //    char json_1[] = R"json(["\uD835\uDEE2"])json";
    char json_1[] = R"json(["\uD835\uDEE)json";
    char json_2[] = R"json(["\uD835?uDEE2"])json";
    char json_3[] = R"json(["\uD835\?DEE2"])json";
    char json_4[] = R"json(["\uD835\uZEE2"])json";
    char json_5[] = R"json(["\uD835\uDZE2"])json";
    char json_6[] = R"json(["\uD835\uDEZ2"])json";
    char json_7[] = R"json(["\uD835\uFfFf"])json";
    char json_8[] = R"json(["\uD835\u0000"])json";
    char json_9[] = R"json(["\uD835\u EE2"])json";
    char json_10[] = R"json(["\uD835\uD E2"])json";
    char json_11[] = R"json(["\uD835\uDE 2"])json";
    char json_12[] = R"json(["\uD835\uDEE "])json";
    char json_13[] = R"json(["\uD835\uzEE2"])json";
    char json_14[] = R"json(["\uD835\uDzE2"])json";
    char json_15[] = R"json(["\uD835\uDEz2"])json";
    char json_16[] = R"json(["\uD835\uDEEz"])json";

    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_end_of_stream);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_4, sizeof(json_4) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_5, sizeof(json_5) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_6, sizeof(json_6) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_7, sizeof(json_7) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_8, sizeof(json_8) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_9, sizeof(json_9) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_10, sizeof(json_10) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_11, sizeof(json_11) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_12, sizeof(json_12) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_13, sizeof(json_13) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_14, sizeof(json_14) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_15, sizeof(json_15) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_16, sizeof(json_16) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_escape);
}

TEST(parsing_state, incorrect_utf8_sequence) {
    char json_1[] = "[\"\xC0\x40\x40\"]";
    char json_2[] = "[\"\xFF\x40\x40\"]";
    char json_3[] = "[\"\x81\x40\x40\"]";

    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_utf8);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_utf8);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_3, sizeof(json_3) - 1);
    EXPECT_EQ(result, jsonlite_result_invalid_utf8);
}

TEST(parsing_state, utf8_noncharacter) {
    char json_1[] = "[\"\xF7\xBF\xBF\xBF\"]";
    char json_2[] = "[\"\xEF\xB7\x91\"]";

    uint8_t parser_memory[1024];
    uint8_t buffer_memory[1024];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(p, json_1, sizeof(json_1) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    buffer = jsonlite_buffer_static_init(buffer_memory, sizeof(buffer_memory));
    p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(p, json_2, sizeof(json_2) - 1);
    EXPECT_EQ(result, jsonlite_result_ok);
}
