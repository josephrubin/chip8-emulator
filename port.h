#ifndef CHIP8_PORT_H
#define CHIP8_PORT_H

#include "constant.h"

/* Return true if `key_number` (0-F) is currently pressed. */
uint8_t Port_is_pressed(uint8_t key_number);

/* Block until the next key press, and return it (0-F). */
uint8_t Port_blocking_next(void);

/* Print the current keypad state to stdout. */
void Port_display_screen(void);

/* Reset the screen so a new frame may be painted. */
void Port_clear_screen(void);

/* Delay for `ms` milliseconds. */
void Port_delay(uint16_t ms);

#endif /* CHIP8_PORT_H */
