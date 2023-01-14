#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H


#include <string>
#include <fstream>
#include "log.h"

class Cartridge {

    // see https://www.nesdev.org/wiki/INES
    struct Header {
        char constant[4];     // ASCII "NES" followed by MS-DOS EOF
        uint8_t prg_rom_size; // Size of PRG ROM in 16 KB units
        uint8_t chr_rom_size; // Size of CHR ROM in 8 KB units
        uint8_t flags6;       // Mapper, mirroring, battery, trainer
        uint8_t flags7;       // Mapper, VS/Playchoice, NES 2.0
        uint8_t flags8;       // PRG-RAM size (rarely used extension)
        uint8_t flags9;       // TV system (rarely used extension)
        uint8_t flags10;      // TV system, PRG-RAM presence (unofficial, rarely used extension)
        uint8_t padding[5];   // Unused padding
    };

public:
    bool load(std::string path);
};


#endif //NES_CARTRIDGE_H
