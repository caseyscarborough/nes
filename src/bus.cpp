#include "bus.h"
#include "log.h"


Bus::Bus() {
    ranges = {
        {0x0000, 0x1FFF, AddressType::CPU,        &Bus::cpu_read,        &Bus::cpu_write},
        {0x2000, 0x3FFF, AddressType::PPU,        &Bus::ppu_read,        &Bus::ppu_write},
        {0x4015, 0x4015, AddressType::APU,        &Bus::apu_read,        &Bus::apu_write},
        {0x4016, 0x4017, AddressType::Controller, &Bus::controller_read, &Bus::controller_write},
        {0x8000, 0xFFFF, AddressType::Cartridge,  &Bus::cartridge_read,  &Bus::cartridge_write},
    };
}

uint8_t Bus::read(uint16_t address) {
    for (auto range: ranges) {
        if (address >= range.start && address <= range.end) {
            std::string name = get_address_type_name(range.type);
            LOG_TRACE("Reading from " << name << " at address 0x" << std::hex << std::uppercase << address)
            return (this->*range.read_callback)(address);
        }
    }
    LOG_ERROR("Invalid read at address 0x" << std::hex << address)
    return 0x0;
}

void Bus::write(uint16_t address, uint8_t data) {
    for (auto range: ranges) {
        if (address >= range.start && address <= range.end) {
            std::string name = get_address_type_name(range.type);
            LOG_TRACE("Writing to " << name << " at address 0x" << std::hex << std::uppercase << address)
            (this->*range.write_callback)(address, data);
            return;
        }
    }
    LOG_ERROR("Invalid write at address 0x" << std::hex << address)
}

void Bus::cycle() {
    this->cpu->cycle();
}

void Bus::connect_cpu(Cpu *cpu) {
    LOG_TRACE("Connecting CPU to the main bus...")
    this->cpu = cpu;
}

void Bus::load_cartridge(Cartridge *cartridge) {
    LOG_TRACE("Connecting cartridge to the main bus...")
    this->cartridge = cartridge;
}

void Bus::connect_ppu(Ppu *ppu) {
    LOG_TRACE("Connecting PPU to the main bus...")
    this->ppu = ppu;
}

uint8_t Bus::cpu_read(uint16_t address) {
    return memory[address];
}

void Bus::cpu_write(uint16_t address, uint8_t data) {
    memory[address] = data;
}

uint8_t Bus::ppu_read(uint16_t address) {
    return ppu->cpu_read(address);
}

void Bus::ppu_write(uint16_t address, uint8_t data) {
    ppu->cpu_write(address, data);
}

uint8_t Bus::apu_read(uint16_t address) {
    // TODO: Implement APU read
    return 0;
}

void Bus::apu_write(uint16_t address, uint8_t data) {
    // TODO: Implement APU write ?
}

uint8_t Bus::controller_read(uint16_t address) {
    // TODO: Implement controller read
    return 0;
}

void Bus::controller_write(uint16_t address, uint8_t data) {
    // TODO: Implement controller write ?
}

uint8_t Bus::cartridge_read(uint16_t address) {
    return cartridge->prg_read(address);
}

void Bus::cartridge_write(uint16_t address, uint8_t data) {
    cartridge->prg_write(address, data);
}

std::string Bus::get_address_type_name(Bus::AddressType type) {
    switch (type) {
        case AddressType::CPU: return "CPU";
        case AddressType::APU: return "APU";
        case AddressType::PPU: return "PPU";
        case AddressType::Controller: return "Controller";
        case AddressType::Cartridge: return "Cartridge PRG-ROM";
        default: return "Unknown";
    }
}
