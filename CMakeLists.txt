cmake_minimum_required(VERSION 3.24)
project(nes)

set(CMAKE_CXX_STANDARD 20)

add_executable(nes src/main.cpp src/cpu.cpp src/cpu.h src/cartridge.cpp src/cartridge.h)
