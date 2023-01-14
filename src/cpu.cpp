#include "cpu.h"

Cpu::Cpu(Bus *bus) : bus(bus) {
    reset();
}

uint8_t Cpu::read(uint16_t address) {
    return bus->read(address);
}

uint16_t Cpu::read_word(uint16_t address) {
    uint16_t lo = read(address);
    uint16_t hi = read(address + 1) << 8;
    return hi | lo;
}

void Cpu::write(uint16_t address, uint8_t data) {
    bus->write(address, data);
}

void Cpu::cycle() {
    total_cycles++;
    if (cycles != 0) {
        cycles--;
        return;
    }

    opcode = read(pc);
    pc++;

    //@formatter:off
    switch (opcode) {
        case 0x69: { immediate();   ADC(); break; }
        case 0x65: { zero_page();   ADC(); break; }
        case 0x75: { zero_page_x(); ADC(); break; }
        case 0x6D: { absolute();    ADC(); break; }
        case 0x7D: { absolute_x();  ADC(); break; }
        case 0x79: { absolute_y();  ADC(); break; }
        case 0x61: { indirect_x();  ADC(); break; }
        case 0x71: { indirect_y();  ADC(); break; }
        case 0x29: { immediate();   AND(); break; }
        case 0x25: { zero_page();   AND(); break; }
        case 0x35: { zero_page_x(); AND(); break; }
        case 0x2D: { absolute();    AND(); break; }
        case 0x3D: { absolute_x();  AND(); break; }
        case 0x39: { absolute_y();  AND(); break; }
        case 0x21: { indirect_x();  AND(); break; }
        case 0x31: { indirect_y();  AND(); break; }
        case 0x0A: { accumulator(); ASL(); break; }
        case 0x06: { zero_page();   ASL(); break; }
        case 0x16: { zero_page_x(); ASL(); break; }
        case 0x0E: { absolute();    ASL(); break; }
        case 0x1E: { absolute_x();  ASL(); break; }
        case 0x90: { relative();    BCC(); break; }
        case 0xB0: { relative();    BCS(); break; }
        case 0xF0: { relative();    BEQ(); break; }
        case 0x24: { zero_page();   BIT(); break; }
        case 0x2C: { absolute();    BIT(); break; }
        case 0x30: { relative();    BMI(); break; }
        case 0xD0: { relative();    BNE(); break; }
        case 0x10: { relative();    BPL(); break; }
        case 0x00: { implied();     BRK(); break; }
        case 0x50: { relative();    BVC(); break; }
        case 0x70: { relative();    BVS(); break; }
        case 0x18: { implied();     CLC(); break; }
        case 0xD8: { implied();     CLD(); break; }
        case 0x58: { implied();     CLI(); break; }
        case 0xB8: { implied();     CLV(); break; }
        case 0xC9: { immediate();   CMP(); break; }
        case 0xC5: { zero_page();   CMP(); break; }
        case 0xD5: { zero_page_x(); CMP(); break; }
        case 0xCD: { absolute();    CMP(); break; }
        case 0xDD: { absolute_x();  CMP(); break; }
        case 0xD9: { absolute_y();  CMP(); break; }
        case 0xC1: { indirect_x();  CMP(); break; }
        case 0xD1: { indirect_y();  CMP(); break; }
        case 0xE0: { immediate();   CPX(); break; }
        case 0xE4: { zero_page();   CPX(); break; }
        case 0xEC: { absolute();    CPX(); break; }
        case 0xC0: { immediate();   CPY(); break; }
        case 0xC4: { zero_page();   CPY(); break; }
        case 0xCC: { absolute();    CPY(); break; }
        case 0xC6: { zero_page();   DEC(); break; }
        case 0xD6: { zero_page_x(); DEC(); break; }
        case 0xCE: { absolute();    DEC(); break; }
        case 0xDE: { absolute_x();  DEC(); break; }
        case 0xCA: { implied();     DEX(); break; }
        case 0x88: { implied();     DEY(); break; }
        case 0x49: { immediate();   EOR(); break; }
        case 0x45: { zero_page();   EOR(); break; }
        case 0x55: { zero_page_x(); EOR(); break; }
        case 0x4D: { absolute();    EOR(); break; }
        case 0x5D: { absolute_x();  EOR(); break; }
        case 0x59: { absolute_y();  EOR(); break; }
        case 0x41: { indirect_x();  EOR(); break; }
        case 0x51: { indirect_y();  EOR(); break; }
        case 0xE6: { zero_page();   INC(); break; }
        case 0xF6: { zero_page_x(); INC(); break; }
        case 0xEE: { absolute();    INC(); break; }
        case 0xFE: { absolute_x();  INC(); break; }
        case 0xE8: { implied();     INX(); break; }
        case 0xC8: { implied();     INY(); break; }
        case 0x4C: { absolute();    JMP(); break; }
        case 0x6C: { indirect();    JMP(); break; }
        case 0x20: { absolute();    JSR(); break; }
        case 0xA9: { immediate();   LDA(); break; }
        case 0xA5: { zero_page();   LDA(); break; }
        case 0xB5: { zero_page_x(); LDA(); break; }
        case 0xAD: { absolute();    LDA(); break; }
        case 0xBD: { absolute_x();  LDA(); break; }
        case 0xB9: { absolute_y();  LDA(); break; }
        case 0xA1: { indirect_x();  LDA(); break; }
        case 0xB1: { indirect_y();  LDA(); break; }
        case 0xA2: { immediate();   LDX(); break; }
        case 0xA6: { zero_page();   LDX(); break; }
        case 0xB6: { zero_page_y(); LDX(); break; }
        case 0xAE: { absolute();    LDX(); break; }
        case 0xBE: { absolute_y();  LDX(); break; }
        case 0xA0: { immediate();   LDY(); break; }
        case 0xA4: { zero_page();   LDY(); break; }
        case 0xB4: { zero_page_x(); LDY(); break; }
        case 0xAC: { absolute();    LDY(); break; }
        case 0xBC: { absolute_x();  LDY(); break; }
        case 0x4A: { accumulator(); LSR(); break; }
        case 0x46: { zero_page();   LSR(); break; }
        case 0x56: { zero_page_x(); LSR(); break; }
        case 0x4E: { absolute();    LSR(); break; }
        case 0x5E: { absolute_x();  LSR(); break; }
        case 0x09: { immediate();   ORA(); break; }
        case 0x05: { zero_page();   ORA(); break; }
        case 0x15: { zero_page_x(); ORA(); break; }
        case 0x0D: { absolute();    ORA(); break; }
        case 0x1D: { absolute_x();  ORA(); break; }
        case 0x19: { absolute_y();  ORA(); break; }
        case 0x01: { indirect_x();  ORA(); break; }
        case 0x11: { indirect_y();  ORA(); break; }
        case 0x48: { implied();     PHA(); break; }
        case 0x08: { implied();     PHP(); break; }
        case 0x68: { implied();     PLA(); break; }
        case 0x28: { implied();     PLP(); break; }
        case 0x2A: { accumulator(); ROL(); break; }
        case 0x26: { zero_page();   ROL(); break; }
        case 0x36: { zero_page_x(); ROL(); break; }
        case 0x2E: { absolute();    ROL(); break; }
        case 0x3E: { absolute_x();  ROL(); break; }
        case 0x6A: { accumulator(); ROR(); break; }
        case 0x66: { zero_page();   ROR(); break; }
        case 0x76: { zero_page_x(); ROR(); break; }
        case 0x6E: { absolute();    ROR(); break; }
        case 0x7E: { absolute_x();  ROR(); break; }
        case 0x40: { implied();     RTI(); break; }
        case 0x60: { implied();     RTS(); break; }
        case 0xE9: { immediate();   SBC(); break; }
        case 0xE5: { zero_page();   SBC(); break; }
        case 0xF5: { zero_page_x(); SBC(); break; }
        case 0xED: { absolute();    SBC(); break; }
        case 0xFD: { absolute_x();  SBC(); break; }
        case 0xF9: { absolute_y();  SBC(); break; }
        case 0xE1: { indirect_x();  SBC(); break; }
        case 0xF1: { indirect_y();  SBC(); break; }
        case 0x38: { implied();     SEC(); break; }
        case 0xF8: { implied();     SED(); break; }
        case 0x78: { implied();     SEI(); break; }
        case 0x85: { zero_page();   STA(); break; }
        case 0x95: { zero_page_x(); STA(); break; }
        case 0x8D: { absolute();    STA(); break; }
        case 0x9D: { absolute_x();  STA(); break; }
        case 0x99: { absolute_y();  STA(); break; }
        case 0x81: { indirect_x();  STA(); break; }
        case 0x91: { indirect_y();  STA(); break; }
        case 0x86: { zero_page();   STX(); break; }
        case 0x96: { zero_page_y(); STX(); break; }
        case 0x8E: { absolute();    STX(); break; }
        case 0x84: { zero_page();   STY(); break; }
        case 0x94: { zero_page_y(); STY(); break; }
        case 0x8C: { absolute();    STY(); break; }
        case 0xAA: { implied();     TAX(); break; }
        case 0xA8: { implied();     TAY(); break; }
        case 0xBA: { implied();     TSX(); break; }
        case 0x8A: { implied();     TXA(); break; }
        case 0x9A: { implied();     TXS(); break; }
        case 0x98: { implied();     TYA(); break; }
        case 0xEA:
        default:   { implied();     NOP(); break; }
    }
    //@formatter:on
}

void Cpu::reset() {
    pc = read_word(0xFFFC);
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFF;
    status = Cpu::Status::I | Cpu::Status::N;
    cycles = 8;
}

//region Addressing Modes

// Operate directly on one or more registers internal to the CPU.
// This is for operations like CLC (Clear Carry Flag), TXA (transfer
// contents of the X-register to the accumulator), etc.
void Cpu::implied() {
    current_mode = Implied;
    current_address = a;
}

// Operate directly on the accumulator. This case is essentially covered
// with Implied addressing mode, so it may not be needed. TBD.
void Cpu::accumulator() {
    current_mode = Accumulator;
    // nothing to be done
}

// The second byte of the instruction contains the operand.
void Cpu::immediate() {
    current_mode = Immediate;
    current_address = pc++;
}

// Zero page is the first 256 bytes of the CPU's address space (0x00-0xFF).
// Since the most significant byte of all zero-page addresses are 00, we
// only need a single byte to specify an address within it.
// The byte at this address will be the one operated on.
void Cpu::zero_page() {
    current_mode = ZeroPage;
    current_address = read(pc) & 0xFF;
}

// The same as Zero Page addressing but using the X register as an offset.
void Cpu::zero_page_x() {
    current_mode = ZeroPageX;
    current_address = (read(pc) + x) & 0xFF;
}

// The same as Zero Page addressing but using the Y register as an offset.
void Cpu::zero_page_y() {
    current_mode = ZeroPageY;
    current_address = (read(pc) + x) & 0xFF;
}

// Read a two byte address in the CPU's address space to be operated on.
void Cpu::absolute() {
    current_mode = Absolute;
    current_address = read_word(pc);
    pc += 2;
}

// The same as Absolute addressing but uses the X register as an offset.
// This mode will take an extra cycle if the page boundary is crossed.
void Cpu::absolute_x() {
    current_mode = AbsoluteX;
    uint16_t address = read_word(pc);
    current_address = address + x;
    if ((current_address & 0xFF00) != (address & 0xFF00)) {
        cycles++;
    }
    pc += 2;
}

// The same as Absolute addressing but uses the Y register as an offset.
// This mode will take an extra cycle if the page boundary is crossed.
void Cpu::absolute_y() {
    current_mode = AbsoluteY;
    uint16_t address = read_word(pc);
    current_address = address + y;
    if ((current_address & 0xFF00) != (address & 0xFF00)) {
        cycles++;
    }
    pc += 2;
}

// Uses the contents of the retrieved address as the effective address.
// These will not overflow into the next page but wrap back around.
void Cpu::indirect() {
    current_mode = Indirect;
    uint16_t lo = read(pc++);
    uint16_t hi = read(pc++);

    uint16_t address = (hi << 8) | lo;
    // Handle bug where the high byte doesn't change when page boundary
    // is crossed.
    uint16_t upper = (address & 0x00FF) == 0x00FF ? address & 0xFF00 : address + 1;
    current_address = read(upper) << 8 | read(address);
}

// Similar to Indirect addressing mode but with a single byte address
// offset by the X register. This will also not overflow but wrap back
// around.
void Cpu::indirect_x() {
    current_mode = IndirectX;
    uint8_t address = read(pc++);
    uint8_t lo = read((address + x) & 0x00FF);
    uint8_t hi = read((address + x + 1) & 0x00FF);
    current_address = (hi << 8) | lo;
}

// A single byte address indexes a location in the zero page.
// We add an extra cycle if the page boundary is crossed.
void Cpu::indirect_y() {
    current_mode = IndirectY;
    uint16_t address = read(pc++);
    uint16_t lo = read(address & 0x00FF);
    uint16_t hi = read((address + 1) & 0x00FF) << 8;
    current_address = (hi | lo) + y;
    if ((current_address & 0xFF00) != hi) {
        cycles++;
    }
}

// Branch instructions can only jump to a "relative" address
// in the same area (offset between -128 and 127).
void Cpu::relative() {
    current_mode = Relative;
    uint16_t offset = read(pc++);
    current_address = offset + pc;
}

//endregion

//region Instructions

void Cpu::ADC() {

}

void Cpu::AND() {

}

void Cpu::ASL() {

}

void Cpu::BCC() {

}

void Cpu::BCS() {

}

void Cpu::BEQ() {

}

void Cpu::BIT() {

}

void Cpu::BMI() {

}

void Cpu::BNE() {

}

void Cpu::BPL() {

}

void Cpu::BRK() {

}

void Cpu::BVC() {

}

void Cpu::BVS() {

}

void Cpu::CLC() {

}

void Cpu::CLD() {

}

void Cpu::CLI() {

}

void Cpu::CLV() {

}

void Cpu::CMP() {

}

void Cpu::CPX() {

}

void Cpu::CPY() {

}

void Cpu::DEC() {

}

void Cpu::DEX() {

}

void Cpu::DEY() {

}

void Cpu::EOR() {

}

void Cpu::INC() {

}

void Cpu::INX() {

}

void Cpu::INY() {

}

void Cpu::JMP() {

}

void Cpu::JSR() {

}

void Cpu::LDA() {

}

void Cpu::LDX() {

}

void Cpu::LDY() {

}

void Cpu::LSR() {

}

void Cpu::NOP() {

}

void Cpu::ORA() {

}

void Cpu::PHA() {

}

void Cpu::PHP() {

}

void Cpu::PLA() {

}

void Cpu::PLP() {

}

void Cpu::ROL() {

}

void Cpu::ROR() {

}

void Cpu::RTI() {

}

void Cpu::RTS() {

}

void Cpu::SBC() {

}

void Cpu::SEC() {

}

void Cpu::SED() {

}

void Cpu::SEI() {

}

void Cpu::STA() {

}

void Cpu::STX() {

}

void Cpu::STY() {

}

void Cpu::TAX() {

}

void Cpu::TAY() {

}

void Cpu::TSX() {

}

void Cpu::TXA() {

}

void Cpu::TXS() {

}

void Cpu::TYA() {

}

//endregion