#ifndef CHIP8_CONSTANT_H
#define CHIP8_CONSTANT_H

#include <stdint.h>

/* The number of bytes in the system RAM. */
extern const uint16_t MEMORY_SIZE;

/* The memory offset that a ROM is loaded into in memory. */
extern const uint16_t APPLICATION_START;

/* Name of file containing data for the interpreter including provided digit sprites. */
extern const char *INTERPRETER_DATA_FILE_NAME;

/* Memory locations that the provided digit sprites are located at in the interpreter data. */
extern const uint16_t DIGIT_SPRITE_LOCATION[];

/* Number of pixels on the system's screen. */
extern const uint16_t PIXEL_COUNT;

/* Indicator of success or failure of hardware component to initialize. */
enum INIT_STATUS {INIT_STATUS_SUCCESS, INIT_STATUS_FAILURE};

#endif /* CHIP8_CONSTANT_H */
