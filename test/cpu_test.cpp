#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../src/bus.h"

class MockBus : public Bus {
};

class TestCpu : public Cpu {
public:
    TestCpu(Bus *bus) : Cpu(bus) {
    }

    MOCK_METHOD(uint8_t, read, (uint16_t address), (override));
    MOCK_METHOD(void, write, (uint16_t address, uint8_t data), (override));

    uint8_t get_a() {
        return a;
    }

    uint8_t get_x() {
        return x;
    }

    uint8_t get_y() {
        return y;
    }

    uint16_t get_pc() {
        return pc;
    }

    uint16_t get_cycles() {
        return cycles;
    }

    uint32_t get_total_cycles() {
        return total_cycles;
    }

    Instruction get_current_instruction() {
        return current_instruction;
    }

    uint16_t get_current_address() {
        return current_address;
    }
};

class CpuTests : public ::testing::Test {
public:

    CpuTests() {
        bus = new MockBus();
        cpu = new TestCpu(bus);
    }

    ~CpuTests() override {
        delete cpu;
        delete bus;
    }

    void SetUp() override {
        using ::testing::Return;

        // Always initialize the program counter to 0x0001
        expect_read(0xFFFC, 0x01);
        expect_read(0xFFFD, 0x00);
        cpu->initialize();
    }

    void TearDown() override {
    }

    MockBus *bus;
    TestCpu *cpu;

    void expect_read(const uint16_t &address, const uint8_t &data) const {
        using ::testing::Return;
        EXPECT_CALL(*cpu, read(address)).WillOnce(Return(data));
    }
};

TEST_F(CpuTests, test_lda_with_absolute_addressing) {
    EXPECT_EQ(cpu->get_a(), 0);

    // We will load the LDA opcode from memory
    expect_read(0x0001, 0xAD);

    // LDA loads the next two bytes from memory to get the
    // memory address from which to retrieve the data to load
    // into the Accumulator.
    expect_read(0x0002, 0x04);
    expect_read(0x0003, 0x00);

    // We'll load 0x33 from memory for it to be set in the Accumulator.
    expect_read(0x0004, 0x33);

    cpu->cycle();

    EXPECT_EQ(cpu->get_current_instruction().opcode, 0xAD);
    EXPECT_EQ(cpu->get_current_instruction().type, Cpu::InstructionType::LDA);

    // Confirm the accumulator was actually set.
    EXPECT_EQ(cpu->get_a(), 0x33);

    // LDA Takes 4 cycles, so after this one cycle we should have 3 to go.
    EXPECT_EQ(cpu->get_cycles(), 3);
    EXPECT_EQ(cpu->get_total_cycles(), 1);
}
