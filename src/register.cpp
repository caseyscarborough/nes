#include "register.h"
#include <vector>

void StatusRegister::update(StatusRegister::Flag flag, bool condition) {
    if (condition) {
        set(flag);
    } else {
        clear(flag);
    }
}

void StatusRegister::update(uint8_t flags, uint16_t data) {
    if (flags & Carry) update(Carry, data > 0xFF);
    if (flags & Zero) update(Zero, data == 0x00);
    if (flags & Negative) update(Negative, data & 0x80);
}

uint8_t StatusRegister::get() const {
    return value;
}

void StatusRegister::set(uint8_t v) {
    // The unused flag should always be 1.
    this->value = (v | Flag::Unused);
}

void StatusRegister::set(StatusRegister::Flag flag) {
    value |= flag;
}

void StatusRegister::clear(StatusRegister::Flag flag) {
    value &= flag;
}


bool StatusRegister::is_set(StatusRegister::Flag flag) const {
    return (value & flag) == flag;
}

bool StatusRegister::is_clear(StatusRegister::Flag flag) const {
    return !is_set(flag);
}