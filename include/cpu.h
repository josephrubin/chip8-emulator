#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include "constant.h"

/* Initialize the CPU, allocate the registers,
 * and prepare to run instruction cycles. */
enum bool Cpu_init(uint8_t *allocated_memory);

/* Run a single instruction cycle - fetch, decode, execute. Set
 * invalidate_display to 1 in a redraw is needed, and 0 otherwise. */
enum bool Cpu_cycle(enum bool *invalidate_display);

/* Write the current V0-VF and I register values to stdout. */
void Cpu_print_memory(void);

/* Free associated resources and disable the component
 * until it is initialized again. */
void Cpu_uninit(void);

#endif /* CHIP8_CPU_H */
