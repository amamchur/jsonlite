#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(stream_dynamic, should_be_initialized_from_memory) {
    jsonlite_stream stream = jsonlite_stream_dynamic_alloc(128);
    EXPECT_NE(stream, nullptr);
    jsonlite_stream_dynamic_free(stream);
}

TEST(stream_dynamic, should_write_data) {
    auto *buffer = (uint8_t *)0xDEADBEEF;
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    jsonlite_stream stream = jsonlite_stream_dynamic_alloc(8);
    EXPECT_NE(stream, nullptr);

    size_t size = jsonlite_stream_dynamic_data(stream, &buffer, 0);
    EXPECT_EQ(size, 0);
    EXPECT_EQ(buffer, nullptr);

    int result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 6);

    result = jsonlite_stream_write(stream, data, sizeof(data));
    EXPECT_EQ(result, 6);

    size = jsonlite_stream_dynamic_data(stream, &buffer, 0);
    EXPECT_EQ(size, 12);

    uint8_t data2[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    int r = memcmp(buffer, data2, sizeof(data2));
    EXPECT_EQ(r, 0);

    free(buffer);

    jsonlite_stream_dynamic_free(stream);
}
