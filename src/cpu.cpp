#include "cpu.h"

Cpu::Cpu(Bus *bus) : bus(bus) {
}

uint8_t Cpu::read(uint16_t address) {
    return bus->read(address);
}

void Cpu::write(uint16_t address, uint8_t data) {
    bus->write(address, data);
}
