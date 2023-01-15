#include "cpu.h"
#include "bus.h"

// The stack pointer (S) points to a byte on Page 1, whose byte is
// from 0x0100 to 0x01FF, where the last two digits are supplied by S.
const uint16_t STACK_PAGE = 0x0100;

// Convenience for status register flags
using Flag = StatusRegister::Flag;

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

// Write to the accumulator if we're in Accumulator mode,
// otherwise write to the bus.
void Cpu::write_acc(uint16_t address, uint8_t data) {
    if (current_mode == Accumulator) {
        a = data;
    } else {
        write(address, data);
    }
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
    // Hardware stack grows downward, so we start at the top of the page (0xFF).
    sp = 0xFF;
    status.set(Flag::InterruptDisable | Flag::Negative);
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
    // TODO: Come back to this.
}

// Logical AND
// A logical AND is performed, bit by bit, on the accumulator
// contents using the contents of a byte of memory.
void Cpu::AND() {
    uint8_t data = read(current_address);
    a &= data;
    status.update(Flag::Zero | Flag::Negative, a);
}

// Arithmetic Shift Left
// Shifts all the bits of the accumulator or memory contents one
// bit left, effectively multiplying by two.
void Cpu::ASL() {
    // Read from the current address and shift left.
    uint16_t data = read(current_address) << 1;
    write_acc(current_address, data & 0xFF);
    status.update(Flag::Carry | Flag::Zero | Flag::Negative, data);
}

// Branch if Carry Clear
// If the carry flag is clear then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BCC() {
    branch_if(status.is_clear(Flag::Carry));
}

// Branch if Carry Set
// If the carry flag is set then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BCS() {
    branch_if(status.is_set(Flag::Carry));
}

// Branch if Equal
// If the zero flag is set then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BEQ() {
    branch_if(status.is_set(Flag::Zero));
}

// Bit Test
// Used to test if one or more bits are set in a target memory location.
// The mask pattern in A is ANDed with the value in memory to set or clear
// the zero flag, but the result is not kept. Bits 7 and 6 of the value
// from memory are copied into the N and V flags.
void Cpu::BIT() {
    // AND the value in the accumulator with the value in memory.
    uint8_t data = a & read(current_address);

    // Copy the 6th bit from the resulting data into the V flag.
    status.update(Flag::Overflow, data & 0x40);

    // Update negative and zero register flags.
    status.update(Flag::Negative | Flag::Zero, data);
}

// Branch if Minus
// If the negative flag is set, then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BMI() {
    branch_if(status.is_set(Flag::Negative));
}

// Branch if Not Equal
// If the zero flag is clear then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BNE() {
    branch_if(status.is_clear(Flag::Zero));
}

// Branch if Positive
// If the negative flag is clear then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BPL() {
    branch_if(status.is_clear(Flag::Negative));
}

// Force Interrupt
// Forces the generation of an interrupt request. The program counter and
// processor status are pushed on the stack then the IRQ interrupt vector at
// $FFFE/F is loaded into the PC and the break flag in the status set to one.
void Cpu::BRK() {
    // TODO: Come back to this.
}

// Branch if Overflow Clear
// If the overflow flag is clear then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BVC() {
    branch_if(status.is_clear(Flag::Overflow));
}

// Branch if Overflow Set
// If the overflow flag is set then add the relative displacement to the
// program counter to cause a branch to a new location.
void Cpu::BVS() {
    branch_if(status.is_set(Flag::Overflow));
}

// Clear Carry Flag
// Set the Carry flag (C) to zero.
void Cpu::CLC() {
    status.clear(Flag::Carry);
}

// Clear Decimal Flag
// Set the decimal flag (D) to zero.
void Cpu::CLD() {
    status.clear(Flag::Decimal);
}

// Clear Interrupt Disable
// Clears the interrupt disable flag (I) allowing normal interrupt
// requests to be serviced.
void Cpu::CLI() {
    status.clear(Flag::InterruptDisable);
}

// Clear Overflow Flag
// Set the overflow flag (V) to zero.
void Cpu::CLV() {
    status.clear(Flag::Overflow);
}

// Compare
// Compares the contents of the accumulator with another memory held
// value and sets the zero and carry flags as appropriate.
void Cpu::CMP() {
    compare(a);
}

// Compare X Register
// Compares the contents of the X register with another memory held
// value and sets the zero and carry flags as appropriate.
void Cpu::CPX() {
    compare(x);
}

// Compare Y Register
// Compares the contents of the Y register with another memory held
// value and sets the zero and carry flags as appropriate.
void Cpu::CPY() {
    compare(y);
}

// Decrement Memory
// Subtracts one from the value held at a specified memory location
// setting the zero and negative flags as appropriate.
void Cpu::DEC() {
    uint8_t data = read(current_address) - 1;
    write(current_address, data);
    status.update(Flag::Zero | Flag::Negative, data);
}

// Decrement X Register
// Subtracts one from the X register setting the zero and negative
// flags as appropriate.
void Cpu::DEX() {
    x--;
    status.update(Flag::Zero | Flag::Negative, x);
}

// Decrement Y Register
// Subtracts one from the Y register setting the zero and negative
// flags as appropriate.
void Cpu::DEY() {
    y--;
    status.update(Flag::Zero | Flag::Negative, y);
}

// Exclusive OR
// An exclusive OR is performed, bit by bit, on the accumulator
// contents using the contents of a byte of memory.
void Cpu::EOR() {
    uint8_t data = read(current_address);
    a ^= data;
    status.update(Flag::Zero | Flag::Negative, a);
}

// Increment Memory
// Adds one to the value held at a specified memory location
// setting the zero and negative flags as appropriate.
void Cpu::INC() {
    uint8_t data = read(current_address) + 1;
    write(current_address, data);
    status.update(Flag::Zero | Flag::Negative, data);
}

// Increment X Register
// Adds one to the X register setting the zero and negative
// flags as appropriate.
void Cpu::INX() {
    x++;
    status.update(Flag::Zero | Flag::Negative, x);
}

// Increment Y Register
// Adds one to the Y register setting the zero and negative
// flags as appropriate.
void Cpu::INY() {
    y++;
    status.update(Flag::Zero | Flag::Negative, y);
}

// Jump
// Sets the program counter to the address specified by the operand.
void Cpu::JMP() {
    pc = current_address;
}

// Jump to Subroutine
// Pushes the address (minus one) of the return point on to the stack
// and then sets the program counter to the target memory address.
void Cpu::JSR() {
    pc--;
    stack_push_word(pc);
    pc = read(pc);
}

// Load Accumulator
// Loads a byte of memory into the accumulator setting the zero and
// negative flags as appropriate.
void Cpu::LDA() {
    a = read(current_address);
    status.update(Flag::Zero | Flag::Negative, a);
}

// Load X Register
// Loads a byte of memory into the X register setting the zero and
// negative flags as appropriate.
void Cpu::LDX() {
    x = read(current_address);
    status.update(Flag::Zero | Flag::Negative, x);
}

// Load Y Register
// Loads a byte of memory into the Y register setting the zero and
// negative flags as appropriate.
void Cpu::LDY() {
    y = read(current_address);
    status.update(Flag::Zero | Flag::Negative, y);
}

// Logical Shift Right
// Each of the bits in A or M is shift one place to the right. The bit that
// was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void Cpu::LSR() {
    uint8_t data = read(current_address);

    // Set the Carry flag to the value in the first bit before shifting.
    status.update(StatusRegister::Carry, data & 0x1);

    data >>= 1;
    status.update(Flag::Zero | Flag::Negative, data);
    write_acc(current_address, data);
}

// No Operation
// The NOP instruction causes no changes to the processor other than the
// normal incrementing of the program counter to the next instruction.
void Cpu::NOP() {
    // Nothing to do
}

// Logical Inclusive OR
// An inclusive OR is performed, bit by bit, on the accumulator
// contents using the contents of a byte of memory.
void Cpu::ORA() {
    uint8_t data = read(current_address);
    a |= data;
    status.update(Flag::Zero | Flag::Negative, a);
}

// Push Accumulator
// Pushes a copy of the accumulator on to the stack.
void Cpu::PHA() {
    stack_push(a);
}

// Push Processor Status onto Stack
// Pushes a copy of the status register on to the stack.
// The status register will be pushed with the break flag
// and bit 5 (unused) set to 1.
void Cpu::PHP() {
    stack_push(status.get() | Flag::Break | Flag::Unused);
}

// Pull Accumulator from Stack
// Pulls an 8 bit value from the stack and into the accumulator.
// The zero and negative flags are set as appropriate.
void Cpu::PLA() {
    a = stack_pop();
    status.update(Flag::Zero | Flag::Negative, a);
}

// Pull Processor Status from Stack
// Pulls an 8 bit value from the stack and into the processor flags.
// The flags will take on new states as determined by the value pulled.
// The status register will be pulled with the break flag and bit 5 ignored.
void Cpu::PLP() {
    status.set(stack_pop());
}

// Rotate Left
// Move each of the bits in either A or M one place to the left. Bit 0 is
// filled with the current value of the carry flag whilst the old bit 7
// becomes the new carry flag value.
void Cpu::ROL() {
    uint16_t data = read(current_address);
    data = (data << 1) | status.is_set(Flag::Carry);
    status.update(Flag::Carry | Flag::Zero | Flag::Negative, data);
    write_acc(current_address, data & 0xFF);
}

// Rotate Right
// Move each of the bits in either A or M one place to the right. Bit 7 is
// filled with the current value of the carry flag whilst the old bit 0
// becomes the new carry flag value.
void Cpu::ROR() {
    uint8_t data = read(current_address);
    uint8_t shifted = status.is_set(Flag::Carry) << 7 | (data >> 7);
    status.update(Flag::Carry, data & 0x01);
    status.update(Flag::Zero | Flag::Negative, shifted);
    write_acc(current_address, shifted);
}

// Return from Interrupt
// Used at the end of an interrupt processing routine. It pulls the
// processor flags from the stack followed by the program counter.
void Cpu::RTI() {
    status.set(stack_pop());
    status.set(status.get() & ~Flag::Break & ~Flag::Unused);
    pc = stack_pop() | (stack_pop() << 8);
}

// Return from Subroutine
// Used at the end of a subroutine to return to the calling routine.
// It pulls the program counter (minus one) from the stack.
void Cpu::RTS() {
    pc = stack_pop_word();
    pc++;
}

void Cpu::SBC() {
    // TODO: Come back to this.
}

// Set Carry Flag
void Cpu::SEC() {
    status.set(Flag::Carry);
}

// Set Decimal Flag
void Cpu::SED() {
    status.set(Flag::Decimal);
}

// Set Interrupt Disable
void Cpu::SEI() {
    status.set(Flag::InterruptDisable);
}

// Store Accumulator
// Stores the contents of the accumulator into memory.
void Cpu::STA() {
    write(current_address, a);
}

// Store X Register
// Stores the contents of the X register into memory.
void Cpu::STX() {
    write(current_address, x);
}

// Store Y Register
// Stores the contents of the Y register into memory.
void Cpu::STY() {
    write(current_address, y);
}

// Transfer Accumulator to X
// Copies the current contents of the accumulator into the X
// register and sets the zero and negative flags as appropriate.
void Cpu::TAX() {
    x = a;
    status.update(Flag::Zero | Flag::Negative, x);
}

// Transfer Accumulator to Y
// Copies the current contents of the accumulator into the Y
// register and sets the zero and negative flags as appropriate.
void Cpu::TAY() {
    y = a;
    status.update(Flag::Zero | Flag::Negative, y);

}

// Transfer Stack Pointer to X
// Copies the current contents of the stack register into the X
// register and sets the zero and negative flags as appropriate.
void Cpu::TSX() {
    x = sp;
    status.update(Flag::Zero | Flag::Negative, x);
}

// Transfer X to Accumulator
// Copies the current contents of the X register into the accumulator
// and sets the zero and negative flags as appropriate.
void Cpu::TXA() {
    a = x;
    status.update(Flag::Zero | Flag::Negative, a);
}

// Transfer X to Stack Pointer
// Copies the current contents of the X register into the stack register.
void Cpu::TXS() {
    sp = x;
}

// Transfer Y to Accumulator
// Copies the current contents of the Y register into the accumulator
// and sets the zero and negative flags as appropriate.
void Cpu::TYA() {
    a = y;
    status.update(Flag::Zero | Flag::Negative, y);
}

// Convenience method for branching instructions (e.g. BCS, BCC, BNE).
// Performs the branching operation if the condition is met.
void Cpu::branch_if(bool condition) {
    if (!condition) {
        return;
    }

    // Add one cycle if the branch succeeds.
    cycles++;

    // Add another cycle if a page boundary was crossed.
    if ((current_address & 0xFF00) != (pc & 0xFF00)) {
        cycles++;
    }

    // The current address has already been offset due
    // to relative addressing (see relative()), we just
    // need to assign it to the program counter.
    pc = current_address;
}

// Convenience method for compare instructions (e.g. CMP, CPX, CPY).
// Compares the data read from memory with the value in the specified
// register and sets the Carry, Zero, and Negative flags appropriately.
void Cpu::compare(uint8_t _register) {
    uint8_t data = read(current_address);
    status.update(Flag::Carry, _register >= data);

    uint8_t diff = _register - data;
    status.update(Flag::Zero | Flag::Negative, diff);
}

// Push a single byte onto the stack.
void Cpu::stack_push(uint8_t data) {
    write(STACK_PAGE | sp, data);
    // decrement the stack pointer because the 6502 stack fills downwards.
    sp--;
}

// Push two bytes onto the stack.
void Cpu::stack_push_word(uint16_t data) {
    stack_push((data >> 8) & 0xFF);
    stack_push(data & 0xFF);
}

// Pop a single byte from the stack.
uint8_t Cpu::stack_pop() {
    // Increment the stack pointer because the 6502 stack fills downwards.
    // We increment before reading because the current SP points to an
    // empty location in memory, the previous SP is the "top" of the stack.
    sp++;
    return read(STACK_PAGE | sp);
}

// Pop two bytes from the stack.
uint16_t Cpu::stack_pop_word() {
    uint8_t low = stack_pop();
    uint16_t hi = (stack_pop() << 8);
    return (hi | low);
}

//endregion