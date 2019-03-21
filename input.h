#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

#include <stdbool.h>
#include "constant.h"

/* Initialize the input hardware. */
bool Inp_init(void);

/* Return true if `key_number` (0-F) is currently pressed. */
uint8_t Inp_is_pressed(uint8_t key_number);

/* Block until the next key press, and return it (0-F). */
uint8_t Inp_blocking_next(void);

/* Print the current keypad state to stdout. */
void Inp_print(void);

/* Free associated resources and disable the component until it is initialized again. */
void Inp_uninit(void);

#endif /* CHIP8_INPUT_H */
