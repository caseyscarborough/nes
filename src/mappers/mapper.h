#ifndef NES_MAPPER_H
#define NES_MAPPER_H


#include <cstdint>

class Mapper {
public:
    enum Type {
        NROM = 0,
    };
    virtual uint8_t prg_read(uint16_t address) = 0;
    virtual void prg_write(uint16_t address, uint8_t data) = 0;
    virtual uint8_t chr_read(uint16_t address) = 0;
    virtual void chr_write(uint16_t address, uint8_t data) = 0;
protected:
    Type type;
};


#endif //NES_MAPPER_H
