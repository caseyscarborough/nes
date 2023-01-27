#include "ppu.h"
#include <cstdlib>

size_t SCREEN_WIDTH = 256;
size_t SCREEN_HEIGHT = 240;
size_t HBLANK_PIXELS = 85;
size_t VBLANK_SCANLINES = 22;
size_t SCANLINE_CYCLES = SCREEN_WIDTH + HBLANK_PIXELS;
size_t SCREEN_CYCLES = SCANLINE_CYCLES * (SCREEN_HEIGHT + VBLANK_SCANLINES);

Ppu::Ppu() {
    // See https://www.nesdev.org/wiki/PPU_palettes#2C02
    palette = {
        Ppu::Color(84, 84, 84),
        Ppu::Color(0, 30, 116),
        Ppu::Color(8, 16, 144),
        Ppu::Color(48, 0, 136),
        Ppu::Color(68, 0, 100),
        Ppu::Color(92, 0, 48),
        Ppu::Color(84, 4, 0),
        Ppu::Color(60, 24, 0),
        Ppu::Color(32, 42, 0),
        Ppu::Color(8, 58, 0),
        Ppu::Color(0, 64, 0),
        Ppu::Color(0, 60, 0),
        Ppu::Color(0, 50, 60),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
        Ppu::Color(152, 150, 152),
        Ppu::Color(8, 76, 196),
        Ppu::Color(48, 50, 236),
        Ppu::Color(92, 30, 228),
        Ppu::Color(136, 20, 176),
        Ppu::Color(160, 20, 100),
        Ppu::Color(152, 34, 32),
        Ppu::Color(120, 60, 0),
        Ppu::Color(84, 90, 0),
        Ppu::Color(40, 114, 0),
        Ppu::Color(8, 124, 0),
        Ppu::Color(0, 118, 40),
        Ppu::Color(0, 102, 120),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
        Ppu::Color(236, 238, 236),
        Ppu::Color(76, 154, 236),
        Ppu::Color(120, 124, 236),
        Ppu::Color(176, 98, 236),
        Ppu::Color(228, 84, 236),
        Ppu::Color(236, 88, 180),
        Ppu::Color(236, 106, 100),
        Ppu::Color(212, 136, 32),
        Ppu::Color(160, 170, 0),
        Ppu::Color(116, 196, 0),
        Ppu::Color(76, 208, 32),
        Ppu::Color(56, 204, 108),
        Ppu::Color(56, 180, 204),
        Ppu::Color(60, 60, 60),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
        Ppu::Color(236, 238, 236),
        Ppu::Color(168, 204, 236),
        Ppu::Color(188, 188, 236),
        Ppu::Color(212, 178, 236),
        Ppu::Color(236, 174, 236),
        Ppu::Color(236, 174, 212),
        Ppu::Color(236, 180, 176),
        Ppu::Color(228, 196, 144),
        Ppu::Color(204, 210, 120),
        Ppu::Color(180, 222, 120),
        Ppu::Color(168, 226, 144),
        Ppu::Color(152, 226, 180),
        Ppu::Color(160, 214, 228),
        Ppu::Color(160, 162, 160),
        Ppu::Color(0, 0, 0),
        Ppu::Color(0, 0, 0),
    };
}

uint8_t Ppu::cpu_read(uint16_t address) {
    switch (address) {
        case PPUSTATUS: {
            uint8_t data = status.get_value() & 0xE0; // mask the top three bits
            status.clear(PpuStatus::Flag::VerticalBlank);
            write_toggle = false;
            return data;
        }
        case OAMDATA:
            return 0x0; // TODO: Handle Object Attribute Memory
        case PPUDATA: {
            uint8_t data = ppu_data_buffer;
            ppu_data_buffer = ppu_read(vram_address.get_value());
            if (vram_address >= 0x3F00) {
                data = ppu_data_buffer;
            }
            vram_address += control.get_increment();
            return data;
        }
        default:
            // Other addresses are not readable
            return 0x0;
    }
}

void Ppu::cpu_write(uint16_t address, uint8_t data) {
    switch (address) {
        case PPUCTRL:
            control.set_value(data);
            vram_address_temp.set(LoopyRegister::NametableX, control.get(PpuCtrl::NametableX));
            vram_address_temp.set(LoopyRegister::NametableY, control.get(PpuCtrl::NametableY));
            break;
        case PPUMASK:
            mask.set_value(data);
            break;
        case PPUSCROLL:
            if (!write_toggle) {
                fine_x = data & 0x07;
                vram_address_temp.set(LoopyRegister::CoarseX, data >> 3);
            } else {
                vram_address_temp.set(LoopyRegister::FineY, data & 0x07);
                vram_address_temp.set(LoopyRegister::CoarseY, data >> 3);
            }
            write_toggle = !write_toggle;
        case PPUADDR:
            if (!write_toggle) {
                vram_address_temp.set_value(((uint16_t)(data & 0x3F) << 8) | (vram_address_temp.get_value() & 0x00FF));
            } else {
                vram_address_temp.set_value((vram_address_temp.get_value() & 0xFF00) | data);
            }
            write_toggle = !write_toggle;
        case PPUDATA:
            ppu_write(vram_address.get_value(), data);
            vram_address += control.get_increment();
        default:
            break;
    }
}

uint8_t Ppu::ppu_read(uint16_t address) {
    // TODO: Implement
    return 0;
}

void Ppu::ppu_write(uint16_t address, uint8_t data) {
    // TODO: Implement
}

void Ppu::clock() {

}

void Ppu::reset() {
    // TODO: Implement
    write_toggle = false;
}
