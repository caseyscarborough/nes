#ifndef NES_LOOPY_H
#define NES_LOOPY_H

#include <cstdint>
#include "register.h"

// Used for the "Loopy" registers inside the PPU (Current VRAM address and Temporary VRAM address)
// See https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
class LoopyRegister : public Register<uint16_t> {
public:
    // yyy NN YYYYY XXXXX
    // ||| || ||||| +++++-- coarse X scroll
    // ||| || +++++-------- coarse Y scroll
    // ||| ++-------------- nametable select
    // +++----------------- fine Y scroll
    enum Flag {
        CoarseX    = 0x001F, // 5 bits
        CoarseY    = 0x03E0, // 5 bits
        NametableX = 0x0400, // 1 bit
        NametableY = 0x0800, // 1 bit
        FineY      = 0x7000, // 3 bits
        Unused     = 0x8000, // 1 bit
    };

    void increment(uint16_t by);
    void set(LoopyRegister::Flag flag, uint16_t data);
};


#endif //NES_LOOPY_H
