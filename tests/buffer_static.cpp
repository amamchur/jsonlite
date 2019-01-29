#include "callback_recorder.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <jsonlite.h>
#include <stdint.h>
#include <string>
#include <vector>

TEST(static_buffer, should_not_be_created_if_not_enough_memory) {
    uint8_t mem[jsonlite_buffer_static_size() + 64];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(nullptr, sizeof(mem));
    EXPECT_EQ(buffer, nullptr);

    buffer = jsonlite_buffer_static_init(mem, 0);
    EXPECT_EQ(buffer, nullptr);
}

TEST(static_buffer, should_set_memory) {
    uint8_t mem[jsonlite_buffer_static_size() + 64];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(mem, sizeof(mem));
    EXPECT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 64);

    uint8_t data[] = {1, 2, 3, 4};
    int code = jsonlite_buffer_set_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, 0);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 4);

    const void *b = jsonlite_buffer_data(buffer);
    int result = memcmp(b, data, sizeof(data));
    EXPECT_EQ(result, 0);
}

TEST(static_buffer, should_not_set_memory_if_out_of__memory) {
    uint8_t mem[jsonlite_buffer_static_size() + 8];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(mem, sizeof(mem));
    EXPECT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 8);

    uint32_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int code = jsonlite_buffer_set_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, -1);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);
}

TEST(static_buffer, should_append_memory) {
    uint8_t mem[jsonlite_buffer_static_size() + 64];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(mem, sizeof(mem));
    EXPECT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);

    uint8_t data[] = {1, 2, 3, 4};
    int code = jsonlite_buffer_append_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, 0);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 4);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 64);

    const void *b = jsonlite_buffer_data(buffer);
    int result = memcmp(b, data, sizeof(data));
    EXPECT_EQ(result, 0);

    code = jsonlite_buffer_append_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, 0);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 8);

    uint8_t data2[] = {1, 2, 3, 4, 1, 2, 3, 4};
    result = memcmp(b, data2, sizeof(data2));
    EXPECT_EQ(result, 0);
}

TEST(static_buffer, should_not_append_memory_if_out_of__memory) {
    uint8_t mem[jsonlite_buffer_static_size() + 6];
    jsonlite_buffer buffer = jsonlite_buffer_static_init(mem, sizeof(mem));
    EXPECT_NE(buffer, nullptr);

    EXPECT_EQ(jsonlite_buffer_size(buffer), 0);

    uint8_t data[] = {1, 2, 3, 4};
    int code = jsonlite_buffer_append_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, 0);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 4);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 6);

    const void *b = jsonlite_buffer_data(buffer);
    int result = memcmp(b, data, sizeof(data));
    EXPECT_EQ(result, 0);

    code = jsonlite_buffer_append_mem(buffer, data, sizeof(data));
    EXPECT_EQ(code, -1);
    EXPECT_EQ(jsonlite_buffer_size(buffer), 4);
    EXPECT_EQ(jsonlite_buffer_capacity(buffer), 6);
}
