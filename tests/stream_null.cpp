#include <gtest/gtest.h>
#include <jsonlite.h>
#include <jsonlite_stream.h>
#include <string>

TEST(stream_null, should_write_any_data) {
    jsonlite_stream stream = jsonlite_stream_null();
    EXPECT_NE(stream, nullptr);

    int result = jsonlite_stream_write(stream, nullptr, 123123);
    EXPECT_EQ(result, 123123);

    result = jsonlite_stream_write(stream, nullptr, 0);
    EXPECT_EQ(result, 0);
}
