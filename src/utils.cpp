#include "utils.h"

uint8_t utils::count_trailing_zeroes(uint16_t bits)  {
    if (bits == 0) return 0;
#if defined __has_builtin && __has_builtin(__builtin_ctz)
    return __builtin_ctz(bits);
#else
    uint8_t count = 0;
    while ((bits & 0x1) == 0) {
        count++;
        bits >>= 1;
    }
    return count;
#endif
}