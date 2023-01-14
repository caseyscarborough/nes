#include "cartridge.h"

bool Cartridge::load(std::string path)  {
    LOG("Loading ROM from path " << path)
    std::ifstream file(path, std::ios_base::binary | std::ios_base::in);
    if (!file || !file.is_open()) {
        LOG_ERROR("Could not open ROM file at path " << path)
        return false;
    }

    Header header{};
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        LOG_ERROR("Could not read iNES header");
        return false;
    }

    if (header.constant[0] != 'N' || header.constant[1] != 'E' || header.constant[2] != 'S' || header.constant[3] != '\x1A') {
        LOG_ERROR("iNES header is not in valid format. Possibly a different format?")
        LOG_ERROR("Expected: 4e 45 53 1a")
        LOG_ERROR("Got:      " << std::hex << unsigned(header.constant[0]) << " " << unsigned(header.constant[1]) << " " << unsigned(header.constant[2]) << " " << unsigned(header.constant[3]))
        return false;
    }
    return true;
}