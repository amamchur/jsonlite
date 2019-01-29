#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(stream_stdout, should_be_initialized_from_memory) {
    jsonlite_stream stream = jsonlite_stream_stdout();
    EXPECT_NE(stream, nullptr);
}

TEST(stream_stdout, should_be_print_text) {
    jsonlite_stream stream = jsonlite_stream_stdout();
    EXPECT_NE(stream, nullptr);

    char text[] = "\nHello World!\n";
    int result = jsonlite_stream_write(stream, text, sizeof(text) - 1);
    EXPECT_EQ(result, 14);
}
