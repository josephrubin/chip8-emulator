#ifndef CHIP8_CONSTANT_H
#define CHIP8_CONSTANT_H

#include <stdint.h>

/* Boolean data type. */
enum bool {FALSE = 0, TRUE};

/* The number of bits per byte. */
extern const uint8_t CHAR_BIT_COUNT;

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

/* Number of CPU execution cycles per second; emulator clock speed. */
extern const uint16_t CYCLES_PER_SECOND;

/* Number of cycles to group together before pausing. */
extern const uint16_t CYCLES_PER_DELAY;

/* Number of ms to pause for after CYCLES_PER_DELAY cycles. */
#define DELAY_MS ((uint16_t) (1000 * (CYCLES_PER_DELAY) / (CYCLES_PER_SECOND)))

/* Number of pixels in the system's screen's width. */
extern const uint16_t WIDTH_PIXEL_COUNT;

/* Number of pixels in the system's screen's height. */
extern const uint16_t HEIGHT_PIXEL_COUNT;

/* Total number of pixels on the system's screen. */
#define PIXEL_COUNT ((uint16_t) ((WIDTH_PIXEL_COUNT) * (HEIGHT_PIXEL_COUNT)))

#endif /* CHIP8_CONSTANT_H */
