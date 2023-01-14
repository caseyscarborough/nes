#include "bus.h"
#include "log.h"

Bus::Bus(Cpu* cpu, Cartridge* cartridge): cpu(cpu), cartridge(cartridge) {
}

uint8_t Bus::read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        return memory[address];
    }
    if (address >= 0x2000 && address <= 0x3FFF) {
        return ppu_registers[address];
    }
    if (address >= 0x4000 && address <= 0x4017) {
        return apu_registers[address];
    }
    if (address >= 0x4020 && address <= 0xFFFF) {
        return cartridge_space[address];
    }

    LOG_ERROR("Invalid read at address 0x" << std::hex << address)
    return 0x0;
}

void Bus::write(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        memory[address] = data;
    }
    if (address >= 0x2000 && address <= 0x3FFF) {
        ppu_registers[address] = data;
    }
    if (address >= 0x4000 && address <= 0x4017) {
        apu_registers[address] = data;
    }
    if (address >= 0x4020 && address <= 0xFFFF) {
        cartridge_space[address] = data;
    }

    LOG_ERROR("Invalid write at address 0x" << std::hex << address)
}
