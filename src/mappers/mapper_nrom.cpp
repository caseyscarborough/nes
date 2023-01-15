#include "mapper.h"
#include "mapper_nrom.h"

MapperNROM::MapperNROM() {
    type = NROM;
}

uint8_t MapperNROM::prg_read(uint16_t address) {
    return 0;
}

void MapperNROM::prg_write(uint16_t address, uint8_t data) {
}

uint8_t MapperNROM::chr_read(uint16_t address) {
    return 0;
}

void MapperNROM::chr_write(uint16_t address, uint8_t data) {
}

MapperNROM::~MapperNROM() = default;

