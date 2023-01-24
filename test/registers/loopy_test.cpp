#include <gtest/gtest.h>
#include "../../src/registers/loopy.h"

TEST(LoopyRegisterTest, test_set) {
    LoopyRegister reg;
    EXPECT_EQ(reg.get_value(), 0b0000000000000000);
    reg.set(LoopyRegister::CoarseX, 0b11111);
    reg.set(LoopyRegister::CoarseY, 0b11111);
    EXPECT_EQ(reg.get_value(), 0b0000001111111111);
    reg.set(LoopyRegister::CoarseY, 0b10101);
    EXPECT_EQ(reg.get_value(), 0b0000001010111111);
    reg.set(LoopyRegister::Unused, 0b1);
    EXPECT_EQ(reg.get_value(), 0b1000001010111111);
    reg.set(LoopyRegister::Unused, 0b0);
    EXPECT_EQ(reg.get_value(), 0b0000001010111111);
    reg.set(LoopyRegister::FineY, 0b011);
    EXPECT_EQ(reg.get_value(), 0b0011001010111111);
    reg.set(LoopyRegister::CoarseX, 0b1000000);
    EXPECT_EQ(reg.get_value(), 0b0011001010100000);
}

TEST(LoopyRegisterTest, test_increment) {
    LoopyRegister reg;
    reg.increment(2);
    EXPECT_EQ(reg.get_value(), 2);
    reg.increment(10);
    EXPECT_EQ(reg.get_value(), 12);
}