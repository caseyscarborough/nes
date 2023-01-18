#ifndef NES_PPUCTRL_H
#define NES_PPUCTRL_H


#include "register.h"

// PPUCTRL register for the PPU.
// See https://www.nesdev.org/wiki/PPU_registers#PPUCTRL
class PpuCtrl : public Register {
public:
    //@formatter:off
    enum Flag {
        Nametable      = 0x0003, // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
        IncrementMode  = 1 << 2, // VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
        SpriteTile     = 1 << 3, // Sprite pattern table address for 8×8 sprites (0: $0000; 1: $1000; ignored in 8×16 mode)
        BackgroundTile = 1 << 4, // Background pattern table address (0: $0000; 1: $1000)
        SpriteHeight   = 1 << 5, // Sprite size (0: 8×8 pixels; 1: 8×16 pixels
        PpuMasterSlave = 1 << 6, // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
        NmiEnable      = 1 << 7, // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
    };
    //@formatter:on
};


#endif //NES_PPUCTRL_H
