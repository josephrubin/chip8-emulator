//
// Created by Joseph on 3/14/2019.
//

#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include "input.h"

uint8_t *key;

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