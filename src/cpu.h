#ifndef NES_CPU_H
#define NES_CPU_H


#include <cstdint>
#include <string>

class Cpu {
public:
    Cpu();

private:
    uint8_t a;      // accumulator
    uint8_t x;      // x register
    uint8_t y;      // y register
    uint16_t pc;    // program counter
    uint8_t sp;     // stack pointer
    uint8_t status; // status register

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

    uint8_t implied();
    uint8_t accumulator();
    uint8_t immediate();
    uint8_t zero_page();
    uint8_t zero_page_x();
    uint8_t relative();
    uint8_t absolute();
    uint8_t absolute_x();
    uint8_t absolute_y();
    uint8_t indirect();
    uint8_t indexed_indirect();
    uint8_t indirect_indexed();

    //endregion

    void cycle();
    void reset();
};


#endif //NES_CPU_H
