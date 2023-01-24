#include "loopy.h"
#include "../utils.h"

void LoopyRegister::increment(uint16_t by) {
    value += by;
}

void LoopyRegister::set(LoopyRegister::Flag flag, uint16_t data) {
    // Shift the data by the number of trailing zeroes for this flag.
    data <<= utils::count_trailing_zeroes(flag);

    // Mask the data with the flag to ensure we don't modify any
    // bits outside the flag.
    data &= flag;

    // Disable the current flag in the register.
    value &= ~flag;

    // Enable the specified bits for the flag.
    value |= data;
}