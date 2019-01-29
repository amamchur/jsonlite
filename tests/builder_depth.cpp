#include <gtest/gtest.h>
#include <jsonlite.h>
#include <stdint.h>
#include <string>

TEST(builder_depth, should_depth_for_object) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(4)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_key(builder, "k", 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_key(builder, "k", 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_key(builder, "k", 1);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_depth_limit);

    result = jsonlite_builder_key(builder, "k", 1);
    EXPECT_EQ(result, jsonlite_result_not_allowed);

    result = jsonlite_builder_object_begin(builder);
    EXPECT_EQ(result, jsonlite_result_depth_limit);
}

TEST(builder_depth, should_depth_for_array) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(4)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);

    jsonlite_result result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_ok);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_depth_limit);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_depth_limit);

    result = jsonlite_builder_array_begin(builder);
    EXPECT_EQ(result, jsonlite_result_depth_limit);
}