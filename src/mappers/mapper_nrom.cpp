#include "mapper.h"
#include "mapper_nrom.h"

MapperNROM::MapperNROM(uint8_t prg_rom_size) {
    type = NROM;
    this->prg_rom_size = prg_rom_size;
}

MapperNROM::~MapperNROM() = default;


uint16_t MapperNROM::map_address_prg(uint16_t address) {
    auto offset = address - 0x8000;
    return prg_rom_size == 1 ? offset & 0x3FFF : offset;
}

uint16_t MapperNROM::map_address_chr(uint16_t address) {
    // No mapping performed for CHR-ROM in Mapper 0
    return address;
}

