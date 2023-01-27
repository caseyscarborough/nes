#ifndef NES_BUS_H
#define NES_BUS_H

#include <cstdint>
#include <cstdlib>
#include <vector>
#include "cartridge.h"
#include "cpu.h"
#include "ppu.h"

class Bus {
public:
    Bus();
    virtual uint8_t read(uint16_t address);
    virtual void write(uint16_t address, uint8_t data);
    void cycle();
    void load_cartridge(Cartridge *cartridge);
    void connect_cpu(Cpu *cpu);
    void connect_ppu(Ppu *ppu);
private:
    enum class AddressType {
        CPU,
        PPU,
        APU,
        Controller,
        Cartridge,
    };
    struct AddressRange {
        uint16_t start;
        uint16_t end;
        AddressType type;
        uint8_t (Bus::*read_callback)(uint16_t);
        void (Bus::*write_callback)(uint16_t, uint8_t);
    };

    std::vector<AddressRange> ranges;
    Cpu *cpu;
    Ppu *ppu;
    Cartridge *cartridge;
    uint8_t memory[8192];

    uint8_t cpu_read(uint16_t address);
    void cpu_write(uint16_t address, uint8_t data);
    uint8_t ppu_read(uint16_t address);
    void ppu_write(uint16_t address, uint8_t data);
    uint8_t apu_read(uint16_t address);
    void apu_write(uint16_t address, uint8_t data);
    uint8_t controller_read(uint16_t address);
    void controller_write(uint16_t address, uint8_t data);
    uint8_t cartridge_read(uint16_t address);
    void cartridge_write(uint16_t address, uint8_t data);
    std::string get_address_type_name(AddressType type);
};

#endif //NES_BUS_H
