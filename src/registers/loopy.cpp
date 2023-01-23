#include "loopy.h"

uint8_t count_trailing_zeroes(uint16_t flag) {
    uint8_t count = 0;
    while ((flag & 0x1) == 0) {
        count++;
        flag >>= 1;
    }
    return count;
}

void LoopyRegister::increment(uint16_t by) {
    value += by;
}

void LoopyRegister::set(LoopyRegister::Flag flag, uint16_t data) {
    // Shift the data by the number of trailing zeroes for this flag.
    data <<= count_trailing_zeroes(flag);

    // Mask the data with the flag to ensure we don't modify any
    // bits outside the flag.
    data &= flag;

    // Disable the current flag in the register.
    value &= ~flag;

    // Enable the specified bits for the flag.
    value |= data;
}