#include "mapper.h"
#include "mapper_nrom.h"

MapperNROM::MapperNROM(Cartridge *cartridge) {
    type = NROM;
    this->cartridge = cartridge;
}

uint8_t MapperNROM::prg_read(uint16_t address) {
    auto offset = address - 0x8000;
    if (cartridge->prg_rom_size == 1) {
        return cartridge->prg_memory[offset & 0x3FFF];
    } else {
        return cartridge->prg_memory[offset];
    }
}

void MapperNROM::prg_write(uint16_t address, uint8_t data) {
    LOG_ERROR("Attempted to write into Read-Only Cartridge PRG-ROM space "
              "at address 0x" << std::hex << std::uppercase << address)
}

uint8_t MapperNROM::chr_read(uint16_t address) {
    return 0;
}

void MapperNROM::chr_write(uint16_t address, uint8_t data) {
}

MapperNROM::~MapperNROM() = default;

