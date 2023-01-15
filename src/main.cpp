
#include "cartridge.h"
#include "cpu.h"
#include "bus.h"
#include <thread>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "No ROM file was passed in!" << std::endl;
        std::cout << "Usage: " << argv[0] << " <rom>" << std::endl;
        return 1;
    }
    std::string path = argv[1];
    auto cartridge = new Cartridge();
    if (!cartridge->load(path)) {
        LOG_ERROR("Could not load ROM at path " << path)
        return 1;
    }
    auto *bus = new Bus();
    auto *cpu = new Cpu(bus);
    bus->connect_cpu(cpu);
    bus->load_cartridge(cartridge);
    cpu->initialize();

    // Each cycle is 1/60th of a second
    auto delay = std::chrono::microseconds(1000000 / 60);
    using clock = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
    while (true) {
        auto start = clock::now();

        // Perform one cycle.
        bus->cycle();

        // Sleep for the remainder of the delay (minus the amount of time the cycle took)
        auto duration = clock::now() - start;
        std::this_thread::sleep_for(std::chrono::duration_cast<microseconds>(delay - duration));
    }
}
