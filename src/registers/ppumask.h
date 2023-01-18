#ifndef NES_PPUMASK_H
#define NES_PPUMASK_H


#include "register.h"

// PPUMASK register for the PPU
// See https://www.nesdev.org/wiki/PPU_registers#PPUMASK
class PpuMask : public Register {
public:
    //@formatter:off
    enum Flag {
        Grayscale          = 1 << 0,
        ShowLeftBackground = 1 << 1,
        ShowLeftSprites    = 1 << 2,
        ShowBackground     = 1 << 3,
        ShowSprites        = 1 << 4,
        EmphasizeRed       = 1 << 5,
        EmphasizeGreen     = 1 << 6,
        EmphasizeBlue      = 1 << 7,
    };
    //@formatter:on
};


#endif //NES_PPUMASK_H
