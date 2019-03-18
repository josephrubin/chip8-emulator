//
// Created by Joseph on 3/14/2019.
//

#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <unistd.h>
#include "input.h"

static const int KEY_COUNT = 16;
static uint8_t *key;

void Inp_init()
{
    key = calloc(16, sizeof *key);
    assert(key);
}

char Inp_is_pressed(uint8_t key_index)
{
    assert(key_index < 16);
    return key[key_index];
}

uint8_t Inp_blocking_next_key()
{
    return getch() - 'a';
}

void Inp_cycle(void)
{
    unsigned int i;

    for (i = 0; i < KEY_COUNT; i++) {
        key[i] = 0;
    }

    while (kbhit()) {
        key[getch() - 'a'] = 1;
    }
}
