#include <gtest/gtest.h>
#include <jsonlite.h>
#include <stdint.h>
#include <string>

TEST(builder_error_handling, should_not_be_initialized_if_oom) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(1)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_EQ(builder, nullptr);
}

TEST(builder_error_handling, should_check_object_begin) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_object_end) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_object_end((builder));
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_array_begin) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_array_end) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_array_end((builder));
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_key) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_key(builder, "key", 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_raw_key) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_raw_key(builder, "key", 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_raw_value) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_raw_value(builder, "key", 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_string) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_string(builder, "key", 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_raw_string) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_raw_string(builder, "key", 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_double) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_double(builder, 3.0);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_int) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_int(builder, 3);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_true) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_true(builder);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_false) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_false(builder);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_null) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_null(builder);
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}

TEST(builder_error_handling, should_check_base64) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    uint8_t value[] = {0x00, 0x01, 0x02};
    jsonlite_result result = jsonlite_builder_base64_value(builder, value, sizeof(value));
    EXPECT_EQ(result, jsonlite_result_not_allowed);
}
