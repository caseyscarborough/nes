#include <gtest/gtest.h>
#include "../../src/registers/loopy.h"

TEST(LoopRegisterTests, test_set) {
    LoopyRegister reg;
    reg.set_value(0b1111111111);
    EXPECT_EQ(reg.get_value(), 0b1111111111);
    reg.set(LoopyRegister::CoarseY, 0b10101);
    EXPECT_EQ(reg.get_value(), 0b1010111111);
    reg.set(LoopyRegister::Unused, 1);
    EXPECT_EQ(reg.get_value(), 0b1000001010111111);
    reg.set(LoopyRegister::Unused, 0);
    EXPECT_EQ(reg.get_value(), 0b0000001010111111);
    reg.set(LoopyRegister::FineY, 0b011);
    EXPECT_EQ(reg.get_value(), 0b0011001010111111);
    reg.set(LoopyRegister::CoarseX, 0b1000000);
    EXPECT_EQ(reg.get_value(), 0b0011001010100000);
}