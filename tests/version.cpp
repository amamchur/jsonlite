#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(jsonlite, should_return_version) {
    EXPECT_EQ(std::string(jsonlite_version()), "2.0.0.0");
}