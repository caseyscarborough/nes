
#include "cartridge.h"
#include "cpu.h"
#include "bus.h"
#include <thread>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "No ROM file was passed in!" << std::endl;
        std::cout << "Usage: ./nes <rom>" << std::endl;
        return 1;
    }
    std::string path = argv[1];
    auto cartridge = new Cartridge();
    if (!cartridge->load(path)) {
        LOG_ERROR("Could not load ROM at path " << path)
        return 1;
    }
    Bus *bus = new Bus();
    Cpu *cpu = new Cpu(bus);
    bus->connect_cpu(cpu);
    bus->load_cartridge(cartridge);
    cpu->initialize();
    while (true) {
        bus->cycle();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000 / 60));
    }
}
