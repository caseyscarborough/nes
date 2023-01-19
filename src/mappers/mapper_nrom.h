#ifndef NES_MAPPER_NROM_H
#define NES_MAPPER_NROM_H


#include "mapper.h"

class MapperNROM : public Mapper {
public:
    MapperNROM(uint8_t prg_rom_size);
    ~MapperNROM();
    uint16_t map_address_prg(uint16_t address) override;
    uint16_t map_address_chr(uint16_t address) override;
private:
    uint8_t prg_rom_size;
};


#endif //NES_MAPPER_NROM_H
