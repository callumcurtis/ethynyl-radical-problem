cmake_minimum_required(VERSION 3.22)
project(ethynyl-radical-problem C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
        src/framework/logging.h
        src/framework/logging.c
        src/framework/main.c
        src/solution/kosmos.h
        src/solution/kosmos.c)
add_executable(ethynyl-radical-problem ${SOURCES})
target_include_directories(ethynyl-radical-problem PRIVATE src)
