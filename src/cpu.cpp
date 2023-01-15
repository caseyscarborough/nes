#include "cpu.h"
#include "bus.h"

// The stack pointer (S) points to a byte on Page 1, whose byte is
// from 0x0100 to 0x01FF, where the last two digits are supplied by S.
const uint16_t STACK_PAGE = 0x0100;

// Convenience for status register flags
using Flag = StatusRegister::Flag;

Cpu::Cpu(Bus *bus) : bus(bus) {
    initialize_lookup_table();
    reset();
}

// Read a single byte from the bus.
uint8_t Cpu::read(uint16_t address) {
    return bus->read(address);
}

// Read two bytes from the bus.
uint16_t Cpu::read_word(uint16_t address) {
    uint16_t lo = read(address);
    uint16_t hi = read(address + 1) << 8;
    return hi | lo;
}

// Write data to the bus.
void Cpu::write(uint16_t address, uint8_t data) {
    bus->write(address, data);
}

// Write to the accumulator if we're in Accumulator mode,
// otherwise write to the bus.
void Cpu::write_acc(uint16_t address, uint8_t data) {
    if (current_instruction.mode == AddressingMode::Accumulator) {
        a = data;
    } else {
        write(address, data);
    }
}

// The cycle for each "tick" of the processor.
void Cpu::cycle() {
    LOG_TRACE("Executing CPU cycle " << total_cycles++)
    // We perform all necessary cycles for one operation at a single time,
    // then "sleep" for the remaining cycles
    if (cycles != 0) {
        cycles--;
        return;
    }

    // Read the next operation from memory, then increment the program counter.
    opcode = read(pc++);

    // The opcode matches the index of the instruction in the lookup table,
    // which makes it easy to look up.
    current_instruction = instructions[opcode];

    // Add the predefined number of cycles for each operation.
    cycles += current_instruction.cycles;

    // Call the addressing mode function and execute the operation.
    LOG_TRACE("Executing opcode 0x" << std::hex << std::uppercase << current_instruction.opcode)
    (this->*current_instruction.ref_mode)();
    (this->*current_instruction.ref_operation)();

    // Decrement cycles since we just executed one for this operation.
    cycles--;
}

// Reset the CPU to it's initial state.
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
    current_address = a;
}

// Operate directly on the accumulator. This case is essentially covered
// with Implied addressing mode, so it may not be needed. TBD.
void Cpu::accumulator() {
    // nothing to be done
}

// The second byte of the instruction contains the operand.
void Cpu::immediate() {
    current_address = pc++;
}

// Zero page is the first 256 bytes of the CPU's address space (0x00-0xFF).
// Since the most significant byte of all zero-page addresses are 00, we
// only need a single byte to specify an address within it.
// The byte at this address will be the one operated on.
void Cpu::zero_page() {
    current_address = read(pc) & 0xFF;
}

// The same as Zero Page addressing but using the X register as an offset.
void Cpu::zero_page_x() {
    current_address = (read(pc) + x) & 0xFF;
}

// The same as Zero Page addressing but using the Y register as an offset.
void Cpu::zero_page_y() {
    current_address = (read(pc) + x) & 0xFF;
}

// Read a two byte address in the CPU's address space to be operated on.
void Cpu::absolute() {
    current_address = read_word(pc);
    pc += 2;
}

// The same as Absolute addressing but uses the X register as an offset.
// This mode will take an extra cycle if the page boundary is crossed.
void Cpu::absolute_x() {
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

// Branch instructions can only jump to a "relative" address
// in the same area (offset between -128 and 127).
void Cpu::relative() {
    uint16_t offset = read(pc++);
    current_address = offset + pc;
}

//endregion

//region Instructions

// Add with Carry
// Adds the contents of a memory location to the accumulator
// together with the carry bit. If overflow occurs the carry bit
// is set, this enables multiple byte addition to be performed.
void Cpu::ADC() {
    uint16_t data = read(current_address);
    uint16_t sum = a + data + status.is_set(Flag::Carry);
    status.update(Flag::Carry | Flag::Zero | Flag::Negative, sum);
    status.update(Flag::Overflow, ((uint16_t)a ^ data) & ((uint16_t)sum ^ data) & 0x80);
    a = sum & 0xFF;
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
    pc++;
    stack_push_word(pc);
    stack_push(status.get() | Flag::Break);

    uint16_t hi = read(0xFFFF) << 8;
    uint16_t lo = read(0xFFFE);
    pc = (hi | lo);
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

// Subtract with Carry
// Subtracts the contents of a memory location to the accumulator
// together with the not of the carry bit. If overflow occurs the carry
// bit is clear, this enables multiple byte subtraction to be performed.
void Cpu::SBC() {
    uint16_t data = ((uint16_t)read(current_address)) ^ 0x00FF;
    uint16_t diff = a + data + status.is_set(Flag::Carry);
    status.update(Flag::Carry | Flag::Zero | Flag::Negative, diff);
    status.update(Flag::Overflow, (diff ^ a) & (diff ^ data) & 0x80);
    a = diff & 0xFF;
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

// Initializes the mappings for all instructions.
// See https://www.masswerk.at/6502/6502_instruction_set.html
void Cpu::initialize_lookup_table() {
    using Type = Cpu::InstructionType;
    using Mode = Cpu::AddressingMode;
    //@formatter:off
    instructions = {
            { 0x00, Type::BRK, Mode::Implied,     &Cpu::BRK, &Cpu::implied,     1, 7 },
            { 0x01, Type::ORA, Mode::IndirectX,   &Cpu::ORA, &Cpu::indirect_x,  2, 6 },
            { 0x02, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x03, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x04, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x05, Type::ORA, Mode::ZeroPage,    &Cpu::ORA, &Cpu::zero_page,   2, 3 },
            { 0x06, Type::ASL, Mode::ZeroPage,    &Cpu::ASL, &Cpu::zero_page,   2, 5 },
            { 0x07, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x08, Type::PHP, Mode::Implied,     &Cpu::PHP, &Cpu::implied,     1, 3 },
            { 0x09, Type::ORA, Mode::Immediate,   &Cpu::ORA, &Cpu::immediate,   2, 2 },
            { 0x0A, Type::ASL, Mode::Accumulator, &Cpu::ASL, &Cpu::accumulator, 1, 2 },
            { 0x0B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x0C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x0D, Type::ORA, Mode::Absolute,    &Cpu::ORA, &Cpu::absolute,    3, 4 },
            { 0x0E, Type::ASL, Mode::Absolute,    &Cpu::ASL, &Cpu::absolute,    3, 6 },
            { 0x0F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x10, Type::BPL, Mode::Relative,    &Cpu::BPL, &Cpu::relative,    2, 2 },
            { 0x11, Type::ORA, Mode::IndirectY,   &Cpu::ORA, &Cpu::indirect_y,  2, 5 },
            { 0x12, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x13, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x14, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x15, Type::ORA, Mode::ZeroPageX,   &Cpu::ORA, &Cpu::zero_page_x, 2, 4 },
            { 0x16, Type::ASL, Mode::ZeroPageX,   &Cpu::ASL, &Cpu::zero_page_x, 2, 6 },
            { 0x17, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x18, Type::CLC, Mode::Implied,     &Cpu::CLC, &Cpu::implied,     1, 2 },
            { 0x19, Type::ORA, Mode::AbsoluteY,   &Cpu::ORA, &Cpu::absolute_y,  3, 4 },
            { 0x1A, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x1B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x1C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x1D, Type::ORA, Mode::AbsoluteX,   &Cpu::ORA, &Cpu::absolute_x,  3, 4 },
            { 0x1E, Type::ASL, Mode::AbsoluteX,   &Cpu::ASL, &Cpu::absolute_x,  3, 7 },
            { 0x1F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x20, Type::JSR, Mode::Absolute,    &Cpu::JSR, &Cpu::absolute,    3, 6 },
            { 0x21, Type::AND, Mode::IndirectX,   &Cpu::AND, &Cpu::indirect_x,  2, 6 },
            { 0x22, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x23, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x24, Type::BIT, Mode::ZeroPage,    &Cpu::BIT, &Cpu::zero_page,   2, 3 },
            { 0x25, Type::AND, Mode::ZeroPage,    &Cpu::AND, &Cpu::zero_page,   2, 3 },
            { 0x26, Type::ROL, Mode::ZeroPage,    &Cpu::ROL, &Cpu::zero_page,   2, 5 },
            { 0x27, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x28, Type::PLP, Mode::Implied,     &Cpu::PLP, &Cpu::implied,     1, 4 },
            { 0x29, Type::AND, Mode::Immediate,   &Cpu::AND, &Cpu::immediate,   2, 2 },
            { 0x2A, Type::ROL, Mode::Accumulator, &Cpu::ROL, &Cpu::accumulator, 1, 2 },
            { 0x2B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x2C, Type::BIT, Mode::Absolute,    &Cpu::BIT, &Cpu::absolute,    3, 4 },
            { 0x2D, Type::AND, Mode::Absolute,    &Cpu::AND, &Cpu::absolute,    3, 4 },
            { 0x2E, Type::ROL, Mode::Absolute,    &Cpu::ROL, &Cpu::absolute,    3, 6 },
            { 0x2F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x30, Type::BMI, Mode::Relative,    &Cpu::BMI, &Cpu::relative,    2, 2 },
            { 0x31, Type::AND, Mode::IndirectY,   &Cpu::AND, &Cpu::indirect_y,  2, 5 },
            { 0x32, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x33, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x34, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x35, Type::AND, Mode::ZeroPageX,   &Cpu::AND, &Cpu::zero_page_x, 2, 4 },
            { 0x36, Type::ROL, Mode::ZeroPageX,   &Cpu::ROL, &Cpu::zero_page_x, 2, 6 },
            { 0x37, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x38, Type::SEC, Mode::Implied,     &Cpu::SEC, &Cpu::implied,     1, 2 },
            { 0x39, Type::AND, Mode::AbsoluteY,   &Cpu::AND, &Cpu::absolute_y,  3, 4 },
            { 0x3A, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x3B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x3C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x3D, Type::AND, Mode::AbsoluteX,   &Cpu::AND, &Cpu::absolute_x,  3, 4 },
            { 0x3E, Type::ROL, Mode::AbsoluteX,   &Cpu::ROL, &Cpu::absolute_x,  3, 7 },
            { 0x3F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x40, Type::RTI, Mode::Implied,     &Cpu::RTI, &Cpu::implied,     1, 6 },
            { 0x41, Type::EOR, Mode::IndirectX,   &Cpu::EOR, &Cpu::indirect_x,  2, 6 },
            { 0x42, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x43, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x44, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x45, Type::EOR, Mode::ZeroPage,    &Cpu::EOR, &Cpu::zero_page,   2, 3 },
            { 0x46, Type::LSR, Mode::ZeroPage,    &Cpu::LSR, &Cpu::zero_page,   2, 5 },
            { 0x47, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x48, Type::PHA, Mode::Implied,     &Cpu::PHA, &Cpu::implied,     1, 3 },
            { 0x49, Type::EOR, Mode::Immediate,   &Cpu::EOR, &Cpu::immediate,   2, 2 },
            { 0x4A, Type::LSR, Mode::Accumulator, &Cpu::LSR, &Cpu::accumulator, 1, 2 },
            { 0x4B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x4C, Type::JMP, Mode::Absolute,    &Cpu::JMP, &Cpu::absolute,    3, 3 },
            { 0x4D, Type::EOR, Mode::Absolute,    &Cpu::EOR, &Cpu::absolute,    3, 4 },
            { 0x4E, Type::LSR, Mode::Absolute,    &Cpu::LSR, &Cpu::absolute,    3, 6 },
            { 0x4F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x50, Type::BVC, Mode::Relative,    &Cpu::BVC, &Cpu::relative,    2, 2 },
            { 0x51, Type::EOR, Mode::IndirectY,   &Cpu::EOR, &Cpu::indirect_y,  2, 5 },
            { 0x52, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x53, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x54, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x55, Type::EOR, Mode::ZeroPageX,   &Cpu::EOR, &Cpu::zero_page_x, 2, 4 },
            { 0x56, Type::LSR, Mode::ZeroPageX,   &Cpu::LSR, &Cpu::zero_page_x, 2, 6 },
            { 0x57, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x58, Type::CLI, Mode::Implied,     &Cpu::CLI, &Cpu::implied,     1, 2 },
            { 0x59, Type::EOR, Mode::AbsoluteY,   &Cpu::EOR, &Cpu::absolute_y,  3, 4 },
            { 0x5A, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x5B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x5C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x5D, Type::EOR, Mode::AbsoluteX,   &Cpu::EOR, &Cpu::absolute_x,  3, 4 },
            { 0x5E, Type::LSR, Mode::AbsoluteX,   &Cpu::LSR, &Cpu::absolute_x,  3, 7 },
            { 0x5F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x60, Type::RTS, Mode::Implied,     &Cpu::RTS, &Cpu::implied,     1, 6 },
            { 0x61, Type::ADC, Mode::IndirectX,   &Cpu::ADC, &Cpu::indirect_x,  2, 6 },
            { 0x62, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x63, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x64, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x65, Type::ADC, Mode::ZeroPage,    &Cpu::ADC, &Cpu::zero_page,   2, 3 },
            { 0x66, Type::ROR, Mode::ZeroPage,    &Cpu::ROR, &Cpu::zero_page,   2, 5 },
            { 0x67, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x68, Type::PLA, Mode::Implied,     &Cpu::PLA, &Cpu::implied,     1, 4 },
            { 0x69, Type::ADC, Mode::Immediate,   &Cpu::ADC, &Cpu::immediate,   2, 2 },
            { 0x6A, Type::ROR, Mode::Accumulator, &Cpu::ROR, &Cpu::accumulator, 1, 2 },
            { 0x6B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x6C, Type::JMP, Mode::Indirect,    &Cpu::JMP, &Cpu::indirect,    3, 5 },
            { 0x6D, Type::ADC, Mode::Absolute,    &Cpu::ADC, &Cpu::absolute,    3, 4 },
            { 0x6E, Type::ROR, Mode::Absolute,    &Cpu::ROR, &Cpu::absolute,    3, 6 },
            { 0x6F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x70, Type::BVS, Mode::Relative,    &Cpu::BVS, &Cpu::relative,    2, 2 },
            { 0x71, Type::ADC, Mode::IndirectY,   &Cpu::ADC, &Cpu::indirect_y,  2, 5 },
            { 0x72, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x73, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x74, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x75, Type::ADC, Mode::ZeroPageX,   &Cpu::ADC, &Cpu::zero_page_x, 2, 4 },
            { 0x76, Type::ROR, Mode::ZeroPageX,   &Cpu::ROR, &Cpu::zero_page_x, 2, 6 },
            { 0x77, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x78, Type::SEI, Mode::Implied,     &Cpu::SEI, &Cpu::implied,     1, 2 },
            { 0x79, Type::ADC, Mode::AbsoluteY,   &Cpu::ADC, &Cpu::absolute_y,  3, 4 },
            { 0x7A, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x7B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x7C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x7D, Type::ADC, Mode::AbsoluteX,   &Cpu::ADC, &Cpu::absolute_x,  3, 4 },
            { 0x7E, Type::ROR, Mode::AbsoluteX,   &Cpu::ROR, &Cpu::absolute_x,  3, 7 },
            { 0x7F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x80, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x81, Type::STA, Mode::IndirectX,   &Cpu::STA, &Cpu::indirect_x,  2, 6 },
            { 0x82, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x83, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x84, Type::STY, Mode::ZeroPage,    &Cpu::STY, &Cpu::zero_page,   2, 3 },
            { 0x85, Type::STA, Mode::ZeroPage,    &Cpu::STA, &Cpu::zero_page,   2, 3 },
            { 0x86, Type::STX, Mode::ZeroPage,    &Cpu::STX, &Cpu::zero_page,   2, 3 },
            { 0x87, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x88, Type::DEY, Mode::Implied,     &Cpu::DEY, &Cpu::implied,     1, 2 },
            { 0x89, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x8A, Type::TXA, Mode::Implied,     &Cpu::TXA, &Cpu::implied,     1, 2 },
            { 0x8B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x8C, Type::STY, Mode::Absolute,    &Cpu::STY, &Cpu::absolute,    3, 4 },
            { 0x8D, Type::STA, Mode::Absolute,    &Cpu::STA, &Cpu::absolute,    3, 4 },
            { 0x8E, Type::STX, Mode::Absolute,    &Cpu::STX, &Cpu::absolute,    3, 4 },
            { 0x8F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x90, Type::BCC, Mode::Relative,    &Cpu::BCC, &Cpu::relative,    2, 2 },
            { 0x91, Type::STA, Mode::IndirectY,   &Cpu::STA, &Cpu::indirect_y,  2, 6 },
            { 0x92, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x93, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x94, Type::STY, Mode::ZeroPageY,   &Cpu::STY, &Cpu::zero_page_y, 2, 4 },
            { 0x95, Type::STA, Mode::ZeroPageX,   &Cpu::STA, &Cpu::zero_page_x, 2, 4 },
            { 0x96, Type::STX, Mode::ZeroPageY,   &Cpu::STX, &Cpu::zero_page_y, 2, 4 },
            { 0x97, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x98, Type::TYA, Mode::Implied,     &Cpu::TYA, &Cpu::implied,     1, 2 },
            { 0x99, Type::STA, Mode::AbsoluteY,   &Cpu::STA, &Cpu::absolute_y,  3, 5 },
            { 0x9A, Type::TXS, Mode::Implied,     &Cpu::TXS, &Cpu::implied,     1, 2 },
            { 0x9B, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x9C, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x9D, Type::STA, Mode::AbsoluteX,   &Cpu::STA, &Cpu::absolute_x,  3, 5 },
            { 0x9E, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x9F, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xA0, Type::LDY, Mode::Immediate,   &Cpu::LDY, &Cpu::immediate,   2, 2 },
            { 0xA1, Type::LDA, Mode::IndirectX,   &Cpu::LDA, &Cpu::indirect_x,  2, 6 },
            { 0xA2, Type::LDX, Mode::Immediate,   &Cpu::LDX, &Cpu::immediate,   2, 2 },
            { 0xA3, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xA4, Type::LDY, Mode::ZeroPage,    &Cpu::LDY, &Cpu::zero_page,   2, 3 },
            { 0xA5, Type::LDA, Mode::ZeroPage,    &Cpu::LDA, &Cpu::zero_page,   2, 3 },
            { 0xA6, Type::LDX, Mode::ZeroPage,    &Cpu::LDX, &Cpu::zero_page,   2, 3 },
            { 0xA7, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xA8, Type::TAY, Mode::Implied,     &Cpu::TAY, &Cpu::implied,     1, 2 },
            { 0xA9, Type::LDA, Mode::Immediate,   &Cpu::LDA, &Cpu::immediate,   2, 2 },
            { 0xAA, Type::TAX, Mode::Implied,     &Cpu::TAX, &Cpu::implied,     1, 2 },
            { 0xAB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xAC, Type::LDY, Mode::Absolute,    &Cpu::LDY, &Cpu::absolute,    3, 4 },
            { 0xAD, Type::LDA, Mode::Absolute,    &Cpu::LDA, &Cpu::absolute,    3, 4 },
            { 0xAE, Type::LDX, Mode::Absolute,    &Cpu::LDX, &Cpu::absolute,    3, 4 },
            { 0xAF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xB0, Type::BCS, Mode::Relative,    &Cpu::BCS, &Cpu::relative,    2, 2 },
            { 0xB1, Type::LDA, Mode::IndirectY,   &Cpu::LDA, &Cpu::indirect_y,  2, 5 },
            { 0x82, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0x83, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xB4, Type::LDY, Mode::ZeroPageX,   &Cpu::LDY, &Cpu::zero_page_x, 2, 4 },
            { 0xB5, Type::LDA, Mode::ZeroPageX,   &Cpu::LDA, &Cpu::zero_page_x, 2, 4 },
            { 0xB6, Type::LDX, Mode::ZeroPageY,   &Cpu::LDX, &Cpu::zero_page_y, 2, 4 },
            { 0x87, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xB8, Type::CLV, Mode::Implied,     &Cpu::CLV, &Cpu::implied,     1, 2 },
            { 0xB9, Type::LDA, Mode::AbsoluteY,   &Cpu::LDA, &Cpu::absolute_y,  3, 4 },
            { 0xBA, Type::TSX, Mode::Implied,     &Cpu::TSX, &Cpu::implied,     1, 2 },
            { 0xBB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xBC, Type::LDY, Mode::AbsoluteX,   &Cpu::LDY, &Cpu::absolute_x,  3, 4 },
            { 0xBD, Type::LDA, Mode::AbsoluteX,   &Cpu::LDA, &Cpu::absolute_x,  3, 4 },
            { 0xBE, Type::LDX, Mode::AbsoluteY,   &Cpu::LDX, &Cpu::absolute_y,  3, 4 },
            { 0xBF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xC0, Type::CPY, Mode::Immediate,   &Cpu::CPY, &Cpu::immediate,   2, 2 },
            { 0xC1, Type::CMP, Mode::IndirectX,   &Cpu::CMP, &Cpu::indirect_x,  2, 6 },
            { 0xC2, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xC3, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xC4, Type::CPY, Mode::ZeroPage,    &Cpu::CPY, &Cpu::zero_page,   2, 3 },
            { 0xC5, Type::CMP, Mode::ZeroPage,    &Cpu::CMP, &Cpu::zero_page,   2, 3 },
            { 0xC6, Type::DEC, Mode::ZeroPage,    &Cpu::DEC, &Cpu::zero_page,   2, 5 },
            { 0xC7, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xC8, Type::INY, Mode::Implied,     &Cpu::INY, &Cpu::implied,     1, 2 },
            { 0xC9, Type::CMP, Mode::Immediate,   &Cpu::CMP, &Cpu::immediate,   2, 2 },
            { 0xCA, Type::DEX, Mode::Implied,     &Cpu::DEX, &Cpu::implied,     1, 2 },
            { 0xCB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xCC, Type::CPY, Mode::Absolute,    &Cpu::CPY, &Cpu::absolute,    3, 4 },
            { 0xCD, Type::CMP, Mode::Absolute,    &Cpu::CMP, &Cpu::absolute,    3, 4 },
            { 0xCE, Type::DEC, Mode::Absolute,    &Cpu::DEC, &Cpu::absolute,    3, 6 },
            { 0xCF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xD0, Type::BNE, Mode::Relative,    &Cpu::BNE, &Cpu::relative,    2, 2 },
            { 0xD1, Type::CMP, Mode::IndirectY,   &Cpu::CMP, &Cpu::indirect_y,  2, 5 },
            { 0xD2, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xD3, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xD4, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xD5, Type::CMP, Mode::ZeroPageX,   &Cpu::CMP, &Cpu::zero_page_x, 2, 4 },
            { 0xD6, Type::DEC, Mode::ZeroPageX,   &Cpu::DEC, &Cpu::zero_page_x, 2, 6 },
            { 0xD7, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xD8, Type::CLD, Mode::Implied,     &Cpu::CLD, &Cpu::implied,     1, 2 },
            { 0xD9, Type::CMP, Mode::AbsoluteY,   &Cpu::CMP, &Cpu::absolute_y,  3, 4 },
            { 0xDA, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xDB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xDC, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xDD, Type::CMP, Mode::AbsoluteX,   &Cpu::CMP, &Cpu::absolute_x,  3, 4 },
            { 0xDE, Type::DEC, Mode::AbsoluteX,   &Cpu::DEC, &Cpu::absolute_x,  3, 7 },
            { 0xDF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xE0, Type::CPX, Mode::Immediate,   &Cpu::CPX, &Cpu::immediate,   2, 2 },
            { 0xE1, Type::SBC, Mode::IndirectX,   &Cpu::SBC, &Cpu::indirect_x,  2, 6 },
            { 0xE2, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xE3, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xE4, Type::CPX, Mode::ZeroPage,    &Cpu::CPX, &Cpu::zero_page,   2, 3 },
            { 0xE5, Type::SBC, Mode::ZeroPage,    &Cpu::SBC, &Cpu::zero_page,   2, 3 },
            { 0xE6, Type::INC, Mode::ZeroPage,    &Cpu::INC, &Cpu::zero_page,   2, 5 },
            { 0xE7, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xE8, Type::INX, Mode::Implied,     &Cpu::INX, &Cpu::implied,     1, 2 },
            { 0xE9, Type::SBC, Mode::Immediate,   &Cpu::SBC, &Cpu::immediate,   2, 2 },
            { 0xEA, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xEB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xEC, Type::CPX, Mode::Absolute,    &Cpu::CPX, &Cpu::absolute,    3, 4 },
            { 0xED, Type::SBC, Mode::Absolute,    &Cpu::SBC, &Cpu::absolute,    3, 4 },
            { 0xEE, Type::INC, Mode::Absolute,    &Cpu::INC, &Cpu::absolute,    3, 6 },
            { 0xEF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xF0, Type::BEQ, Mode::Relative,    &Cpu::BEQ, &Cpu::relative,    2, 2 },
            { 0xF1, Type::SBC, Mode::IndirectY,   &Cpu::SBC, &Cpu::indirect_y,  2, 5 },
            { 0xF2, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xF3, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xF4, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xF5, Type::SBC, Mode::ZeroPageX,   &Cpu::SBC, &Cpu::zero_page_x, 2, 4 },
            { 0xF6, Type::INC, Mode::ZeroPageX,   &Cpu::INC, &Cpu::zero_page_x, 2, 6 },
            { 0xF7, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xF8, Type::SED, Mode::Implied,     &Cpu::SED, &Cpu::implied,     1, 2 },
            { 0xF9, Type::SBC, Mode::AbsoluteY,   &Cpu::SBC, &Cpu::absolute_y,  3, 4 },
            { 0xFA, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xFB, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xFC, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
            { 0xFD, Type::SBC, Mode::AbsoluteX,   &Cpu::SBC, &Cpu::absolute_x,  3, 4 },
            { 0xFE, Type::INC, Mode::AbsoluteX,   &Cpu::INC, &Cpu::absolute_x,  3, 7 },
            { 0xFF, Type::NOP, Mode::Implied,     &Cpu::NOP, &Cpu::implied,     1, 2 },
    };
    //@formatter:on
}

//endregion