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