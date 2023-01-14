#ifndef NES_CPU_H
#define NES_CPU_H


#include <cstdint>
#include <string>
#include "bus.h"

class Cpu {
public:
    Cpu(Bus *bus);

    void cycle();
    void reset();
private:
    // Flags for the status register
    enum Status {
        C = 1 << 0, // carry
        Z = 1 << 1, // zero
        I = 1 << 2, // interrupt disable
        D = 1 << 3, // decimal (has no effect on the NES)
        B = 1 << 4, // break
        V = 1 << 6, // overflow
        N = 1 << 7, // negative
    };

    enum AddressingMode {
        Implied,
        Accumulator,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        IndirectX,
        IndirectY,
        Relative,
    };

    uint8_t a;        // accumulator
    uint8_t x;        // x register
    uint8_t y;        // y register
    uint16_t pc;      // program counter
    uint8_t sp;       // stack pointer
    uint8_t status;   // status register (P)
    uint16_t cycles;  // total clock cycles
    uint16_t opcode;  // current operation

    uint16_t current_address;    // current address
    AddressingMode current_mode; // current addressing mode

    // the main bus and methods for communicating with other components
    Bus *bus;
    uint8_t read(uint16_t address);
    uint16_t read_word(uint16_t address);
    void write(uint16_t address, uint8_t data);

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
