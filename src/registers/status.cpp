#include "status.h"

void StatusRegister::set_defaults(uint8_t flags, uint16_t data) {
    set_if(Carry, flags & Carry && data > 0xFF);
    set_if(Zero, flags & Zero && data == 0x00);
    set_if(Negative, flags & Negative && data & 0x80);
}

void StatusRegister::set_value(uint8_t value) {
    // The unused flag should always be 1.
    Register::set_value(value | Flag::Unused);
}