#ifndef NES_CPU_H
#define NES_CPU_H


#include <cstdint>
#include <string>
#include <vector>
#include "registers/status.h"

class Bus;

class Cpu {
public:
    Cpu(Bus *bus);

    void initialize();
    void cycle();
    void reset();
    void nmi();
    void irq();
private:
    enum class InstructionType {
        ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
        CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
        JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
        RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA
    };
    enum class AddressingMode {
        Implied, Accumulator, Immediate, ZeroPage, ZeroPageX, ZeroPageY,
        Absolute, AbsoluteX, AbsoluteY, Indirect, IndirectX, IndirectY, Relative,
    };
    enum class InterruptType {
        NMI,
        IRQ,
        BRK,
    };

    struct Instruction {
        uint8_t opcode;
        InstructionType type;
        AddressingMode mode;
        void (Cpu::*ref_operation)();
        void (Cpu::*ref_mode)();
        uint8_t bytes;
        uint8_t cycles;
    };

    uint8_t a;        // accumulator
    uint8_t x;        // x register
    uint8_t y;        // y register
    uint16_t pc;      // program counter
    uint8_t sp;       // stack pointer
    uint16_t cycles;  // current cycles
    StatusRegister status; // status register (P)
    uint16_t total_cycles; // total clock cycles

    uint16_t current_address; // current address
    Instruction current_instruction; // current instruction
    std::vector<Instruction> instructions; // instruction lookup table

    // the main bus and methods for communicating with other components
    Bus *bus;
    uint8_t read(uint16_t address);
    uint16_t read_word(uint16_t address);
    void write(uint16_t address, uint8_t data);
    // same as write, except writes to the accumulator instead
    // of memory if the current addressing mode is accumulator.
    void write_acc(uint16_t address, uint8_t data);
    void stack_push(uint8_t data);
    void stack_push_word(uint16_t data);
    uint8_t stack_pop();
    uint16_t stack_pop_word();
    std::string get_instruction_name(InstructionType type);
    std::string get_addressing_mode_name(AddressingMode mode);

    //region instructions
    void ADC();
    void AND();
    void ASL();
    void BCC();
    void BCS();
    void BEQ();
    void BIT();
    void BMI();
    void BNE();
    void BPL();
    void BRK();
    void BVC();
    void BVS();
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void CMP();
    void CPX();
    void CPY();
    void DEC();
    void DEX();
    void DEY();
    void EOR();
    void INC();
    void INX();
    void INY();
    void JMP();
    void JSR();
    void LDA();
    void LDX();
    void LDY();
    void LSR();
    void NOP();
    void ORA();
    void PHA();
    void PHP();
    void PLA();
    void PLP();
    void ROL();
    void ROR();
    void RTI();
    void RTS();
    void SBC();
    void SEC();
    void SED();
    void SEI();
    void STA();
    void STX();
    void STY();
    void TAX();
    void TAY();
    void TSX();
    void TXA();
    void TXS();
    void TYA();

    // Convenience method for branch instructions (BCC, BCS, etc.).
    void branch_if(bool condition);

    // Convenience method for compare instructions (CPX, CPY, CMP).
    void compare(uint8_t _register);

    // Convenience method for working with interrupts (NMI, IRQ, BRK)
    void interrupt(InterruptType type);

    //endregion instructions

    //region Addressing modes used for each instruction
    // see:
    // - https://www.nesdev.org/wiki/CPU_addressing_modes
    // - https://www.nesdev.org/obelisk-6502-guide/reference.html
    // - http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    // - https://www.masswerk.at/6502/6502_instruction_set.html
    void implied();
    void accumulator();
    void immediate();
    void zero_page();
    void zero_page_x();
    void zero_page_y();
    void absolute();
    void absolute_x();
    void absolute_y();
    void indirect();
    void indirect_x();
    void indirect_y();
    void relative();
    //endregion
};


#endif //NES_CPU_H
