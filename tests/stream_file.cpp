#include <gtest/gtest.h>
#include <jsonlite.h>
#include <jsonlite_stream_file.h>
#include <string>

TEST(stream_file, should_be_initialized) {
    jsonlite_stream stream = jsonlite_stream_file_alloc(nullptr);
    EXPECT_NE(stream, nullptr);
    jsonlite_stream_file_free(stream);
}

TEST(stream_file, should_write_data) {
    char data[] = "[]";
    FILE *fp = fopen("stream_file_should_write_data.json", "wb");
    EXPECT_NE(fp, nullptr);

    jsonlite_stream stream = jsonlite_stream_file_alloc(fp);
    EXPECT_NE(stream, nullptr);

    int result = jsonlite_stream_write(stream, data, sizeof(data) - 1);
    EXPECT_EQ(result, 2);

    jsonlite_stream_file_free(stream);

    fclose(fp);

    fp = fopen("stream_file_should_write_data.json", "rb");
    EXPECT_NE(fp, nullptr);

    char buffer[1024];
    size_t count = fread(buffer, 1, sizeof(buffer), fp);
    EXPECT_EQ(count, sizeof(data) - 1);

    int r = memcmp(buffer, data, sizeof(data) - 1);
    EXPECT_EQ(r, 0);

    fclose(fp);

    remove("stream_file_should_write_data.json");
}
