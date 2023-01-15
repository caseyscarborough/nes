#include "bus.h"
#include "log.h"

Bus::Bus() = default;

uint8_t Bus::read(uint16_t address) {
    LOG_TRACE("Reading from address 0x" << std::hex << std::uppercase << address)
    if (address < 0x2000) {
        return memory[address];
    } else if (address <= 0x4000) {
        // TODO: read from ppu
    } else if (address < 0x4018) {
        // TODO: read from apu
    } else if (address < 0x4020) {
        // TODO: read from ???
    } else {
        return this->cartridge->prg_read(address);
    }

    LOG_ERROR("Invalid read at address 0x" << std::hex << address)
    return 0x0;
}

void Bus::write(uint16_t address, uint8_t data) {
    LOG_TRACE("Writing data to address 0x" << std::hex << std::uppercase << address)
    if (address < 0x2000) {
        memory[address] = data;
        return;
    } else if (address < 0x4000) {
        // TODO: write to ppu
        return;
    } else if (address < 0x4018) {
        // TODO: write to apu
        return;
    } else if (address < 0x4020) {
        // TODO: write to ???
        return;
    } else {
        this->cartridge->prg_write(address, data);
    }

    LOG_ERROR("Invalid write at address 0x" << std::hex << address)
}

void Bus::cycle() {
    this->cpu->cycle();
}

void Bus::connect_cpu(Cpu *cpu) {
    LOG_TRACE("Connecting CPU to the main bus...")
    this->cpu = cpu;
}

void Bus::load_cartridge(Cartridge *cartridge) {
    LOG_TRACE("Connecting cartridge to the main bus...")
    this->cartridge = cartridge;
}