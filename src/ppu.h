#ifndef NES_PPU_H
#define NES_PPU_H


#include <cstdint>
#include "registers/ppuctrl.h"
#include "registers/ppumask.h"
#include "registers/ppustatus.h"

class Ppu {
public:
    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);
private:
    // registers
    PpuCtrl control;
    PpuMask mask;
    PpuStatus status;
};


#endif //NES_PPU_H
