#ifndef NES_MAPPER_CNROM_H
#define NES_MAPPER_CNROM_H


#include "mapper.h"

class MapperCNROM: public Mapper {
public:
    MapperCNROM();
    ~MapperCNROM();
    uint8_t prg_read(uint16_t address) override;
    void prg_write(uint16_t address, uint8_t data) override;
    uint8_t chr_read(uint16_t address) override;
    void chr_write(uint16_t address, uint8_t data) override;
};


#endif //NES_MAPPER_CNROM_H
