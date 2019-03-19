//
// Created by Joseph on 3/14/2019.
//

#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

void Inp_init(void);

void Inp_cycle(void);

char Inp_is_pressed(uint8_t key_index);

uint8_t Inp_blocking_next_key();

void Inp_print(void);

#endif //CHIP8_INPUT_H
