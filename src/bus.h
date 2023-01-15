#ifndef NES_BUS_H
#define NES_BUS_H

#include <cstdint>
#include <cstdlib>
#include "cpu.h"
#include "cartridge.h"

class Bus {
public:
    Bus();
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    void cycle();
    void load_cartridge(Cartridge * cartridge);
    void connect_cpu(Cpu* cpu);
private:
    Cpu *cpu;
    Cartridge *cartridge;
    uint8_t memory[8192];           // 0x0000 -> 0x07FF (mirrored 3 additional times)
};

#endif //NES_BUS_H
