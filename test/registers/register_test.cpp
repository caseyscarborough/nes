#include <gtest/gtest.h>
#include "../../src/registers/status.h"

class TestRegister : public Register<uint16_t> {
public:

};

class RegisterTest : public ::testing::Test {
public:
    void SetUp() override {
        reg.set_value(0);
    }

    void TearDown() override {
    }

    TestRegister reg;
};

TEST_F(RegisterTest, test_get_value) {
    EXPECT_EQ(reg.get_value(), 0);
}

TEST_F(RegisterTest, test_set_value) {
    reg.set_value(1);
    EXPECT_EQ(reg.get_value(), 1);
    reg.set_value(0b1100);
    EXPECT_EQ(reg.get_value(), 0b1100);
}

TEST_F(RegisterTest, test_set) {
    reg.set(0b0100);
    EXPECT_EQ(reg.get_value(), 0b0100);
    reg.set(0b0010);
    EXPECT_EQ(reg.get_value(), 0b0110);
    reg.set(0b110000);
    EXPECT_EQ(reg.get_value(), 0b110110);
}

TEST_F(RegisterTest, test_clear) {
    reg.set_value(0b1111);
    reg.clear(0b0010);
    EXPECT_EQ(reg.get_value(), 0b1101);
    reg.clear(0b1101);
    EXPECT_EQ(reg.get_value(), 0);
}

TEST_F(RegisterTest, test_set_if) {
    reg.set_if(0b0100, true);
    EXPECT_EQ(reg.get_value(), 0b0100);
    reg.set_if(0b0100, false);
    EXPECT_EQ(reg.get_value(), 0b0000);
}

TEST_F(RegisterTest, test_is_set) {
    reg.set_value(0b1010);
    EXPECT_TRUE(reg.is_set(0b1000));
    EXPECT_TRUE(reg.is_set(0b0010));
    EXPECT_FALSE(reg.is_set(0b0100));
    EXPECT_FALSE(reg.is_set(0b0001));
}

TEST_F(RegisterTest, test_is_clear) {
    reg.set_value(0b1010);
    EXPECT_TRUE(reg.is_clear(0b0001));
    EXPECT_TRUE(reg.is_clear(0b0100));
    EXPECT_FALSE(reg.is_clear(0b0010));
    EXPECT_FALSE(reg.is_clear(0b1000));
}

TEST_F(RegisterTest, test_get) {
    reg.set_value(0b10010010100);
    EXPECT_EQ(reg.get(0b11100), 0b101);
    EXPECT_EQ(reg.get(0b10000000000), 1);
    EXPECT_EQ(reg.get(0b1000000000), 0);
    EXPECT_EQ(reg.get(0b11110000000), 0b1001);
    EXPECT_EQ(reg.get(0b1), 0);
    EXPECT_EQ(reg.get(0b1100000), 0);
}

TEST_F(RegisterTest, test_increment) {
    reg += 2;
    EXPECT_EQ(reg.get_value(), 2);
    reg += 10;
    EXPECT_EQ(reg.get_value(), 12);
}

TEST_F(RegisterTest, test_decrement) {
    reg.set_value(15);
    reg -= 2;
    EXPECT_EQ(reg.get_value(), 13);
    reg -= 5;
    EXPECT_EQ(reg.get_value(), 8);
}