#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

/* Initialize the CPU, allocate the registers, and prepare to run instruction cycles. */
enum INIT_STATUS Cpu_init(uint8_t *allocated_memory);

/* Run a single instruction cycle -- fetch, decode, execute. */
void Cpu_cycle(void);

/* Write the current V0-VF and I register values to stdout. */
void Cpu_print_memory(void);

#endif /* CHIP8_CPU_H */
