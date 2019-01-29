#include <gtest/gtest.h>
#include <jsonlite.h>
#include <stdint.h>
#include <string>

TEST(builder, should_be_initialized_from_memory) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_NE(builder, nullptr);
}

TEST(builder, should_not_be_initialized_if_not_enough_memory) {
    uint8_t builder_memory[1];
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), jsonlite_stream_null());
    EXPECT_EQ(builder, nullptr);
}

TEST(builder, should_build_empty_object) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{}"), data);
}

TEST(builder, should_build_nested_object) {
    char key[] = "k";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"k\":{\"k\":{\"k\":{}}}}"), data);
}

TEST(builder, should_build_nested_object_indentation) {
    char key[] = "k";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_set_indentation(builder, 2);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, key, sizeof(key) - 1);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\n  \"k\": {\n    \"k\": {\n      \"k\": {\n      }\n    }\n  }\n}"), data);
}

TEST(builder, should_build_empty_array) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[]"), data);
}

TEST(builder, should_build_nested_arrays) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[[[[]]]]"), data);
}

TEST(builder, should_build_nested_arrays_indentation) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_set_indentation(builder, 2);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[\n  [\n    [\n      [\n      ]\n    ]\n  ]\n]"), data);
}

TEST(builder, should_build_object_with_number) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "key1", sizeof("key1") - 1);
    jsonlite_builder_int(builder, 123214);
    jsonlite_builder_key(builder, "key2", sizeof("key2") - 1);
    jsonlite_builder_double(builder, 1232.14);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"key1\":123214,\"key2\":1232.14}"), data);
}

TEST(builder, should_build_object_with_raw_string_value) {
    char key1[] = "key1";
    char value1[] = "value1";
    char key2[] = "key2";
    char value2[] = "value2";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_raw_key(builder, key1, sizeof(key1) - 1);
    jsonlite_builder_raw_string(builder, value1, sizeof(value1) - 1);
    jsonlite_builder_raw_key(builder, key2, sizeof(key2) - 1);
    jsonlite_builder_raw_string(builder, value2, sizeof(value2) - 1);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"key1\":\"value1\",\"key2\":\"value2\"}"), data);
}

TEST(builder, should_build_object_with_raw_key_value_with_identation) {
    char key1[] = "key1";
    char value1[] = "value1";
    char key2[] = "key2";
    char value2[] = "value2";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_set_indentation(builder, 2);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_raw_key(builder, key1, sizeof(key1) - 1);
    jsonlite_builder_raw_string(builder, value1, sizeof(value1) - 1);
    jsonlite_builder_raw_key(builder, key2, sizeof(key2) - 1);
    jsonlite_builder_raw_string(builder, value2, sizeof(value2) - 1);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\n  \"key1\": \"value1\",\n  \"key2\": \"value2\"\n}"), data);
}


TEST(builder, should_build_object_with_string) {
    char str[] = "value\"\\\b\f\n\r\t";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "key", sizeof("key") - 1);
    jsonlite_builder_string(builder, str, sizeof(str) - 1);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"key\":\"value\\\"\\\\\\b\\f\\n\\r\\t\"}"), data);
}

TEST(builder, should_build_object_with_true_false_null) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "t", sizeof("t") - 1);
    jsonlite_builder_true(builder);
    jsonlite_builder_key(builder, "f", sizeof("f") - 1);
    jsonlite_builder_false(builder);
    jsonlite_builder_key(builder, "n", sizeof("n") - 1);
    jsonlite_builder_null(builder);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"t\":true,\"f\":false,\"n\":null}"), data);
}

TEST(builder, should_build_object_with_base64) {
    uint8_t v1[] = {0x00};
    uint8_t v2[] = {0x00, 0x01};

    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "t", sizeof("t") - 1);
    jsonlite_builder_base64_value(builder, v1, sizeof(v1) - 1);
    jsonlite_builder_key(builder, "f", sizeof("f") - 1);
    jsonlite_builder_base64_value(builder, v2, sizeof(v2) - 1);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"t\":\"\",\"f\":\"AA==\"}"), data);
}

TEST(builder, should_build_object_with_raw_value) {
    char str[] = "raw_value";
    char number[] = "12345";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_raw_key(builder, str, sizeof(str) - 1);
    jsonlite_builder_raw_value(builder, number, sizeof(number) - 1);
    jsonlite_builder_object_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("{\"raw_value\":12345}"), data);
}

TEST(builder, should_build_array_with_number) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_int(builder, 123214);
    jsonlite_builder_double(builder, 13.2);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[123214,13.2]"), data);
}

TEST(builder, should_build_array_with_string) {
    char str[] = "value\"\\\b\f\n\r\t";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_string(builder, str, sizeof(str) - 1);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[\"value\\\"\\\\\\b\\f\\n\\r\\t\"]"), data);
}

TEST(builder, should_build_array_with_raw_value) {
    char str[] = "raw_value";
    char number[] = "12345";
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_raw_string(builder, str, sizeof(str) - 1);
    jsonlite_builder_raw_value(builder, number, sizeof(number) - 1);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[\"raw_value\",12345]"), data);
}

TEST(builder, should_build_array_with_true_false_null) {
    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_true(builder);
    jsonlite_builder_false(builder);
    jsonlite_builder_null(builder);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[true,false,null]"), data);
}

TEST(builder, should_build_array_with_base64) {
    uint8_t v1[] = {0x00};
    uint8_t v2[] = {0x00, 0x01};
    uint8_t v3[] = {0x00, 0x01, 0x02};
    uint8_t v4[] = {0x00, 0x01, 0x02, 0x03};

    uint8_t builder_memory[jsonlite_builder_estimate_size(16)];
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    jsonlite_builder builder = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_builder_array_begin(builder);
    jsonlite_builder_base64_value(builder, v1, sizeof(v1) - 1);
    jsonlite_builder_base64_value(builder, v2, sizeof(v2) - 1);
    jsonlite_builder_base64_value(builder, v3, sizeof(v3) - 1);
    jsonlite_builder_base64_value(builder, v4, sizeof(v4) - 1);
    jsonlite_builder_array_end(builder);
    char *data = (char *) jsonlite_stream_static_data(stream);
    EXPECT_EQ(std::string("[\"\",\"AA==\",\"AAE=\",\"AAEC\"]"), data);
}
