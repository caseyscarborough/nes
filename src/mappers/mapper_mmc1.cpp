#include "mapper_mmc1.h"

MapperMMC1::MapperMMC1() {
    type = MMC1;
}

MapperMMC1::~MapperMMC1() = default;

// TODO: Implement
uint8_t MapperMMC1::prg_read(uint16_t address) {
    return 0;
}

void MapperMMC1::prg_write(uint16_t address, uint8_t data) {

}

uint8_t MapperMMC1::chr_read(uint16_t address) {
    return 0;
}

void MapperMMC1::chr_write(uint16_t address, uint8_t data) {

}