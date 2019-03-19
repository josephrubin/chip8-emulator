#ifndef CHIP8_CONSTANT_H
#define CHIP8_CONSTANT_H

#include <stdint.h>

/* The memory offset that a ROM is loaded into in memory. */
static const uint16_t APPLICATION_START = 0x200;

/* The number of bytes in the system RAM. */
static const int MEMORY_SIZE = 4 * 1024;

/* Name of file containing data for the interpreter including provided digit sprites. */
static const char *interpreter_data_file_name = "interpreter_data";

/* Memory locations that the provided digit sprites are located at in the interpreter data. */
static const uint16_t DIGIT_SPRITE_LOCATION[] = {0x00, 0x05, 0x0A, 0x0F, 0x14, 0x19, 0x1E, 0x23, 0x28, 0x2D, 0x32, 0x37,
                                            0x3C, 0x41, 0x46, 0x4B};

#endif /* CHIP8_CONSTANT_H */
