#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(stream_static, should_be_initialized_from_memory) {
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    EXPECT_NE(stream, nullptr);
}

TEST(stream_static, should_not_be_initialized_if_out_of_memory) {
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, 1);
    EXPECT_EQ(stream, nullptr);
}

TEST(stream_static, should_write_data) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 256] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    EXPECT_NE(stream, nullptr);

    int result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 4);

    const void *b = jsonlite_stream_static_data(stream);
    EXPECT_NE(b, nullptr);

    int r = memcmp(b, data, sizeof(data));
    EXPECT_EQ(r, 0);

    result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 4);

    size_t size  = jsonlite_stream_static_written_bytes(stream);
    EXPECT_EQ(size, 8);
}

TEST(stream_static, should_stop_write_data_if_out_of_memory) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t stream_memory[jsonlite_static_mem_stream_size() + 8] = {0};
    jsonlite_stream stream = jsonlite_stream_static_init(stream_memory, sizeof(stream_memory));
    EXPECT_NE(stream, nullptr);

    int result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 4);

    const void *b = jsonlite_stream_static_data(stream);
    EXPECT_NE(b, nullptr);

    int r = memcmp(b, data, sizeof(data));
    EXPECT_EQ(r, 0);

    result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 4);

    size_t size  = jsonlite_stream_static_written_bytes(stream);
    EXPECT_EQ(size, 8);

    result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 0);

    result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 0);

    size  = jsonlite_stream_static_written_bytes(stream);
    EXPECT_EQ(size, 8);
}
