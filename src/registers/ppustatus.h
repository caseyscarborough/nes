#ifndef NES_PPUSTATUS_H
#define NES_PPUSTATUS_H


#include "register.h"

// PPUSTATUS register for the PPU.
// See https://www.nesdev.org/wiki/PPU_registers#PPUSTATUS
class PpuStatus : public Register {
public:
    //@formatter:off
    enum Flag {
        PpuOpenBus     = 0x001F, // PPU Open Bus (first 5 bits)
        SpriteOverflow = 1 << 5, // Sprite Overflow
        SpriteZeroHit  = 1 << 6, // Sprite 0 Hit
        VerticalBlank  = 1 << 7, // Vertical Blank started
    };
    //@formatter:on
};


#endif //NES_PPUSTATUS_H
