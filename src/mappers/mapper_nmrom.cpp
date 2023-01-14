#include "mapper_nmrom.h"

MapperNMROM::MapperNMROM() {
    type = NMROM;
}

MapperNMROM::~MapperNMROM() {
}

uint8_t MapperNMROM::prg_read(uint16_t address) {
    return 0;
}

void MapperNMROM::prg_write(uint16_t address, uint8_t data) {
}

uint8_t MapperNMROM::chr_read(uint16_t address) {
    return 0;
}

void MapperNMROM::chr_write(uint16_t address, uint8_t data) {
}

