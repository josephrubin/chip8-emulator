//
// Created by Joseph on 3/5/2019.
//

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

void Cpu_init(uint8_t *mem);

void Cpu_cycle(void);

void Cpu_print_memory(void);

#endif //CHIP8_CPU_H
