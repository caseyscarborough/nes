#ifndef NES_PPU_H
#define NES_PPU_H

#include <array>
#include <cstdint>
#include "registers/ppuctrl.h"
#include "registers/ppumask.h"
#include "registers/ppustatus.h"
#include "registers/loopy.h"

class Ppu {
public:
    Ppu();
    ~Ppu() = default;
    struct Color {
        uint8_t r, g, b;
        Color() : r(0), g(0), b(0) {};
        Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {};
    };

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);
    void clock();
    void reset();
private:
    enum Registers {
        PPUCTRL = 0x2000,
        PPUMASK = 0x2001,
        PPUSTATUS = 0x2002,
        OAMADDR = 0x2003,
        OAMDATA = 0x2004,
        PPUSCROLL = 0x2005,
        PPUADDR = 0x2006,
        PPUDATA = 0x2007,
        OAMDMA = 0x4014,
    };
    // registers
    PpuCtrl control;
    PpuMask mask;
    PpuStatus status;
    LoopyRegister vram_address;
    LoopyRegister vram_address_temp;
    uint8_t fine_x; // technically only 3 bits
    uint8_t ppu_data_buffer;
    bool write_toggle;
    std::array<Color, 64> palette;
};


#endif //NES_PPU_H
