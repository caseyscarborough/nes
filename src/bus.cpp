#include "bus.h"
#include "log.h"

Bus::Bus() = default;

uint8_t Bus::read(uint16_t address) {
    if (address < 0x2000) {
        LOG_TRACE("Reading from memory address 0x" << std::hex << std::uppercase << address)
        // The memory is mirrored every 0x07FF (2K) bytes.
        return memory[address & 0x07FF];
    } else if (address <= 0x4000) {
        // PPU only has 8 registers (mirrored all the way through 0x3FFF).
        return ppu->cpu_read(address & 0x2007);
    } else if (address < 0x4018) {
        // TODO: read from apu
    } else if (address < 0x4020) {
        // TODO: read from controller
    } else {
        LOG_TRACE("Reading from PRG-ROM at 0x" << std::hex << std::uppercase << address)
        return this->cartridge->prg_read(address);
    }

    LOG_ERROR("Invalid read at address 0x" << std::hex << address)
    return 0x0;
}

void Bus::write(uint16_t address, uint8_t data) {
    if (address < 0x2000) {
        LOG_TRACE("Writing data to memory address 0x" << std::hex << std::uppercase << address)
        // The memory is mirrored every 0x07FF (2K) bytes.
        memory[address & 0x07FF] = data;
        return;
    } else if (address < 0x4000) {
        // PPU has only 8 registers (mirrored all the way through 0x3FFF).
        ppu->cpu_write(address & 0x2007, data);
        return;
    } else if (address < 0x4018) {
        // TODO: write to apu
        return;
    } else if (address < 0x4020) {
        // TODO: write to controller
        return;
    } else {
        LOG_TRACE("Writing data to PRG-ROM at 0x" << std::hex << std::uppercase << address)
        this->cartridge->prg_write(address, data);
        return;
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

void Bus::connect_ppu(Ppu *ppu) {
    LOG_TRACE("Connecting PPU to the main bus...")
    this->ppu = ppu;
}
