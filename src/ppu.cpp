#include "ppu.h"

// TODO: Implement read/write functions
uint8_t Ppu::cpu_read(uint16_t address) {
    switch (address) {
        case 0x2002: {
            uint8_t data = status.get_value() & 0xE0; // mask the top three bits
            status.clear(PpuStatus::Flag::VerticalBlank);
            // TODO: Handle loopy register
            return data;
        }
        case 0x2004: // TODO: Handle OAM
        case 0x2007: // TODO: Handle PPUDATA
        default:
            return 0x0;
    }
}

void Ppu::cpu_write(uint16_t address, uint8_t data) {

}

uint8_t Ppu::ppu_read(uint16_t address) {
    return 0;
}

void Ppu::ppu_write(uint16_t address, uint8_t data) {

}
