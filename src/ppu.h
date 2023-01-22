#ifndef NES_PPU_H
#define NES_PPU_H

#include <array>
#include <cstdint>
#include "registers/ppuctrl.h"
#include "registers/ppumask.h"
#include "registers/ppustatus.h"

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
private:
    // registers
    PpuCtrl control;
    PpuMask mask;
    PpuStatus status;
    uint8_t ppu_data_buffer;
    uint16_t ppu_address;
    std::array<Color, 64> palette;
};


#endif //NES_PPU_H
