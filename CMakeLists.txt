cmake_minimum_required(VERSION 3.24)
project(nes)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES src/*.cpp src/**/*.cpp)
file(GLOB TESTS test/**.cpp test/**/*.cpp)

add_executable(nes main.cpp ${SOURCES})
add_executable(nes_test ${SOURCES} ${TESTS})

# Setup GoogleTest
enable_testing()
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
FetchContent_MakeAvailable(googletest)
target_link_libraries(nes_test GTest::gtest_main GTest::gmock_main)
include(GoogleTest)
gtest_discover_tests(nes_test)