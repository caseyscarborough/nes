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
    opcode = read(pc);
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
    uint8_t address = read(pc++);
    uint8_t lo = read((address + x) & 0x00FF);
    uint8_t hi = read((address + x + 1) & 0x00FF);
    current_address = (hi << 8) | lo;
}

// A single byte address indexes a location in the zero page.
// We add an extra cycle if the page boundary is crossed.
void Cpu::indirect_y() {
    uint16_t address = read(pc++);
    uint16_t lo = read(address & 0x00FF);
    uint16_t hi = read((address + 1) & 0x00FF) << 8;
    current_address = (hi | lo) + y;
    if ((current_address & 0xFF00) != hi) {
        cycles++;
    }
}

// Used for branch instructions.
// TODO: Don't fully understand this yet, look into it more.
void Cpu::relative() {
    current_address = read(pc++);
    if (current_address & 0x80) {
        // negative
        current_address |= 0xFF00;
    }
}
