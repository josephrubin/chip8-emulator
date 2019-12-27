#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include "constant.h"

/* Turn on the CHIP-8 with the application in `rom_file_name` loaded in at
 * address APPLICATION_START. Only one instance of the CHIP-8 may be active at
 * once, so make sure the CHIP-8 is off first. */
enum bool Chip8_turn_on(char *rom_file_name);

#endif /* CHIP8_CHIP8_H */
