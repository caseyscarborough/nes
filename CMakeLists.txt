cmake_minimum_required(VERSION 3.24)
project(nes)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES src/*.cpp src/mappers/*.cpp src/registers/*.cpp)
add_executable(nes ${SOURCES})
