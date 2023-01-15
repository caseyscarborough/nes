#include "cartridge.h"

#include <memory>
#include "mappers/mapper_nrom.h"
#include "mappers/mapper_mmc1.h"
#include "mappers/mapper_cnrom.h"

bool Cartridge::load(const std::string &path) {
    LOG("Loading ROM from path " << path)
    std::ifstream file(path, std::ios_base::binary | std::ios_base::in);
    if (!file || !file.is_open()) {
        LOG_ERROR("Could not open ROM file at path " << path)
        return false;
    }
    this->path = path;

    Header header{};
    if (!file.read(reinterpret_cast<char *>(&header), sizeof(header))) {
        LOG_ERROR("Could not read iNES header from ROM! " << path);
        return false;
    }

    if (header.constant[0] != 'N' || header.constant[1] != 'E' || header.constant[2] != 'S' ||
        header.constant[3] != '\x1A') {
        LOG_ERROR("iNES header is not in valid format. Possibly a different format?")
        LOG_ERROR("Expected: 4e 45 53 1a")
        LOG_ERROR("Got:      " << std::hex << unsigned(header.constant[0]) << " " << unsigned(header.constant[1]) << " "
                               << unsigned(header.constant[2]) << " " << unsigned(header.constant[3]))
        return false;
    }

    if (header.prg_rom_size == 0) {
        LOG_ERROR("Cartridge has no PRG-ROM! " << path);
        return false;
    }


    if (has_flag(header.flags6, Trainer)) {
        // Skip the 512-byte trainer if it exists
        file.seekg(512, std::ios::cur);
    }

    // The upper half of flags 6 and 7 make up the mapper ID.
    mapper_id = header.flags6 >> 4 | (header.flags7 & 0xF0);
    prg_rom_size = header.prg_rom_size;
    chr_rom_size = header.chr_rom_size;
    version = has_flag(header.flags7, Nes2FormatA | Nes2FormatB) ? 2 : 1;
    mirror = has_flag(header.flags6, Mirroring) ? Vertical : Horizontal;
    system = has_flag(header.flags9, 1) ? PAL : NTSC;

    // Read PRG-ROM into memory
    prg_memory.resize(0x4000 * prg_rom_size);
    file.read(reinterpret_cast<char *>(&prg_memory[0]), prg_memory.size());

    // Read CHR-ROM into memory
    chr_memory.resize(0x2000 * chr_rom_size);
    file.read(reinterpret_cast<char *>(&chr_memory[0]), chr_memory.size());

    switch (mapper_id) {
        case Mapper::NROM:
            mapper = new MapperNROM(this);
            break;
        case Mapper::MMC1:
            mapper = new MapperMMC1();
            break;
        case Mapper::CNROM:
            mapper = new MapperCNROM();
            break;
        default:
            LOG_ERROR("Mapper " << unsigned(mapper_id) << " is not implemented!")
            return false;
    }

    LOG("Successfully loaded ROM!")
    LOG(" - Path: " << this->path)
    LOG(" - PRG-ROM Banks (16KB): " << unsigned(prg_rom_size) << " (" << unsigned(prg_memory.size() / 1024) << "KB)")
    LOG(" - CHR-ROM Banks (8KB): " << unsigned(chr_rom_size) << " (" << unsigned(chr_memory.size() / 1024) << "KB)")
    LOG(" - iNES Format: " << unsigned(version))
    LOG(" - Mapper ID: " << unsigned(mapper_id))
    LOG(" - TV System: " << (system == NTSC ? "NTSC" : "PAL"))
    return true;
}

bool Cartridge::has_flag(uint8_t flags, uint8_t flag) {
    return (flags & flag) == flag;
}

uint8_t Cartridge::prg_read(uint16_t address) {
    return this->mapper->prg_read(address);
}

void Cartridge::prg_write(uint16_t address, uint8_t data) {
    this->mapper->prg_write(address, data);
}

uint8_t Cartridge::chr_read(uint16_t address) {
    return this->mapper->chr_read(address);
}

void Cartridge::chr_write(uint16_t address, uint8_t data) {
    this->mapper->chr_write(address, data);
}
