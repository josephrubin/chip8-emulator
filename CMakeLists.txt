cmake_minimum_required(VERSION 3.10)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

add_executable(chip8 cpu.c cpu.h chip8.c screen.c screen.h input.c input.h)