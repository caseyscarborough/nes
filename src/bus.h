#ifndef NES_BUS_H
#define NES_BUS_H

#include <cstdint>
#include <cstdlib>
#include "cpu.h"
#include "cartridge.h"

class Bus {
public:
    Bus(Cpu *cpu, Cartridge *cartridge);
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

private:
    Cpu *cpu;
    Cartridge *cartridge;
    uint8_t memory[8192];           // 0x0000 -> 0x07FF (mirrored 3 additional times)
    uint8_t ppu_registers[8192];    // 0x2000 -> 0x2007 (mirrored 255 additional times)
    uint8_t apu_registers[18];      // 0x4000 -> 0x4017
    uint8_t cartridge_space[49120]; // 0x4020 -> 0xFFFF
};

#endif //NES_BUS_H
