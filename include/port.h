#ifndef CHIP8_PORT_H
#define CHIP8_PORT_H

#include "constant.h"

/* -------------------------------------------------------------------------- */
/* Input -------------------------------------------------------------------- */

/* Return TRUE iff. `key_number` (0-F) is currently pressed. */
enum bool Port_is_pressed(uint8_t key_number);

/* Block until the next key press and return it (0-F). */
uint8_t Port_blocking_next(void);

/* -------------------------------------------------------------------------- */
/* Output ------------------------------------------------------------------- */

/* Visualize the display, showing the current frame. */
void Port_display_screen(uint8_t *display);

/* Reset the screen so a new frame may be shown. */
void Port_clear_screen(void);

/* -------------------------------------------------------------------------- */
/* Delay -------------------------------------------------------------------- */

/* Delay for `ms` milliseconds. */
void Port_delay(uint16_t ms);

#endif /* CHIP8_PORT_H */
