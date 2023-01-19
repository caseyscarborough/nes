#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H


#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "log.h"
#include "mappers/mapper.h"

class Cartridge {

    // see https://www.nesdev.org/wiki/INES
    struct Header {
        char constant[4];     // ASCII "NES" followed by MS-DOS EOF (0x1A)
        uint8_t prg_rom_size; // Size of PRG ROM in 16 KB units
        uint8_t chr_rom_size; // Size of CHR ROM in 8 KB units
        uint8_t flags6;       // Mapper, mirroring, battery, trainer
        uint8_t flags7;       // Mapper, VS/Playchoice, NES 2.0
        uint8_t flags8;       // PRG-RAM size (rarely used extension)
        uint8_t flags9;       // TV system (rarely used extension)
        uint8_t flags10;      // TV system, PRG-RAM presence (unofficial, rarely used extension)
        uint8_t padding[5];   // Unused padding
    };

    enum Flags6 {
        Mirroring = 1 << 0,       // Mirroring enabled, horizontal or vertical
        HasPrgRam = 1 << 1,       // Cartridge has PRG RAM ($6000-7FFF) or other persistent memory
        Trainer = 1 << 2,         // Trainer exists
        IgnoreMirroring = 1 << 2, // Ignore mirroring, provide four-screen VRAM
    };

    enum Flags7 {
        VSUnisystem = 1 << 0,
        PlayChoice10 = 1 << 1,
        Nes2FormatA = 1 << 2,
        Nes2FormatB = 1 << 3,
    };

    enum Mirror {
        Horizontal,
        Vertical,
        FourScreen,
    };

    enum TvSystem {
        NTSC,
        PAL,
    };

    std::string path;
    TvSystem system;
    Mirror mirror;
    uint8_t mapper_id;
    uint8_t prg_rom_size;
    uint8_t chr_rom_size;
    uint8_t prg_ram_size;
    uint8_t version;

    Mapper *mapper;
    std::vector<uint8_t> prg_memory;
    std::vector<uint8_t> chr_memory;

    static bool has_flag(uint8_t flags, uint8_t flag);
public:
    bool load(const std::string& path);
    uint8_t prg_read(uint16_t address);
    void prg_write(uint16_t address, uint8_t data);
    uint8_t chr_read(uint16_t address);
    void chr_write(uint16_t address, uint8_t data);
};


#endif //NES_CARTRIDGE_H
