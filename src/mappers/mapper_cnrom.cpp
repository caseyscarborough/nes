#include "mapper_cnrom.h"

MapperCNROM::MapperCNROM() {
    type = CNROM;
}

MapperCNROM::~MapperCNROM() = default;

// TODO: Implement
uint8_t MapperCNROM::prg_read(uint16_t address) {
    return 0;
}

void MapperCNROM::prg_write(uint16_t address, uint8_t data) {

}

uint8_t MapperCNROM::chr_read(uint16_t address) {
    return 0;
}

void MapperCNROM::chr_write(uint16_t address, uint8_t data) {

}
