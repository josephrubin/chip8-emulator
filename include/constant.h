#ifndef CHIP8_CONSTANT_H
#define CHIP8_CONSTANT_H

#include <stdint.h>

/* Boolean data type. */
enum bool {FALSE, TRUE};

/* The number of bytes in the system RAM. */
extern const uint16_t MEMORY_SIZE;

/* The number of entries (each capable of holding an address) in the stack. */
extern const uint8_t STACK_SIZE;

/* The memory offset that a ROM is loaded into in memory. */
extern const uint16_t APPLICATION_START;

/* Name of file containing data for the
 * interpreter including provided digit sprites. */
extern const char *INTERPRETER_DATA_FILE_NAME;

/* Memory locations that the provided digit
 * sprites are located at in the interpreter data. */
extern const uint16_t DIGIT_SPRITE_LOCATION[];

/* Number of pixels in the system's screen's width. */
extern const uint16_t WIDTH_PIXEL_COUNT;

/* Number of pixels in the system's screen's height. */
extern const uint16_t HEIGHT_PIXEL_COUNT;

/* Total number of pixels on the system's screen. */
#define PIXEL_COUNT ((uint16_t) ((WIDTH_PIXEL_COUNT) * (HEIGHT_PIXEL_COUNT)))

#endif /* CHIP8_CONSTANT_H */
