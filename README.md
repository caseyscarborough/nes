## NES Emulator

This repository contains a basic NES emulator written in C++. It is currently a work in progress.

## Building

To build the emulator you will need the following:

- C Compiler
- CMake
- Make

On macOS, you can build it by running the following:

```bash
# Install cmake and make
brew install cmake make

# Create a build directory and cd into it
mkdir build && cd build

# Run cmake and build
cmake .. && make
```

## Running

You can then run the emulator with the following command (while still in the `build` directory after running `make`):

```bash
./nes /path/to/rom
```

## Testing

You can run the unit tests by running the following (while still in the `build` directory after running `make`):

```bash
./nes_test
```

## References

- [@javidx9 on YouTube](https://www.youtube.com/playlist?list=PLrOv9FMX8xJHqMvSGB_9G9nZZ_4IgteYf) / [@OneLoneCoder on GitHub](https://github.com/OneLoneCoder/olcNES)
- [Nesdev Wiki](https://www.nesdev.org/wiki/Nesdev_Wiki)
- [amhndu/SimpleNES](https://github.com/amhndu/SimpleNES)

