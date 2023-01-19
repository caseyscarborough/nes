#ifndef NES_MAPPER_H
#define NES_MAPPER_H


#include <cstdint>

class Mapper {
public:
    enum Type {
        NROM = 0,
        MMC1 = 1,
        CNROM = 3,
    };
    virtual uint16_t map_address_prg(uint16_t address) = 0;
    virtual uint16_t map_address_chr(uint16_t address) = 0;
    Type get_type();
protected:
    Type type;
};


#endif //NES_MAPPER_H
