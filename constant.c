#include <stdint.h>

#include "constant.h"

/* See declarations in constant.h for explanatory comments. */

const uint16_t MEMORY_SIZE = 4 * 1024;

const uint16_t APPLICATION_START = 0x200;

const char *INTERPRETER_DATA_FILE_NAME = "interpreter_data";

const uint16_t DIGIT_SPRITE_LOCATION[] = {0x00, 0x05, 0x0A, 0x0F, 0x14, 0x19, 0x1E, 0x23, 0x28, 0x2D, 0x32, 0x37,
                                          0x3C, 0x41, 0x46, 0x4B};

const uint16_t PIXEL_COUNT = 64 * 32;
