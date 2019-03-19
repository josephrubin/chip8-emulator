#ifndef CHIP8_CONSTANT_H
#define CHIP8_CONSTANT_H

#include <stdint.h>

/* The memory offset that a ROM is loaded into in memory. */
static const uint16_t APPLICATION_START = 0x200;

/* The number of bytes in the system RAM. */
static const int MEMORY_SIZE = 4 * 1024;

#endif //CHIP8_CONSTANT_H
