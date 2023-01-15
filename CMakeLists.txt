cmake_minimum_required(VERSION 3.24)
project(nes)

set(CMAKE_CXX_STANDARD 20)

add_executable(nes src/main.cpp src/cpu.cpp src/cpu.h src/cartridge.cpp src/cartridge.h src/bus.cpp src/bus.h src/bus.cpp src/bus.h src/log.cpp src/log.h src/mappers/mapper.cpp src/mappers/mapper.h src/mappers/mapper_nmrom.cpp src/mappers/mapper_nmrom.h src/register.cpp src/register.h)
