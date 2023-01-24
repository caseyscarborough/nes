#include <gtest/gtest.h>
#include "../src/utils.h"

TEST(utils, test_count_trailing_zeroes_when_zero) {
    EXPECT_EQ(0, utils::count_trailing_zeroes(0));
}

TEST(utils, test_count_trailing_zeroes_when_nonzero) {
    EXPECT_EQ(5, utils::count_trailing_zeroes(0b100100000));
    EXPECT_EQ(0, utils::count_trailing_zeroes(0b111));
    EXPECT_EQ(1, utils::count_trailing_zeroes(0b1010));
}