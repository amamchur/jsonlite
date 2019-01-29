#include <gtest/gtest.h>
#include <jsonlite.h>
#include <string>

TEST(jsonlite, should_return_version) {
    EXPECT_EQ(JSONLITE_VERSION, 200000);
}
