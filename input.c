//
// Created by Joseph on 3/14/2019.
//

#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <afxres.h>
#include <conio.h>

#include "input.h"

/* The system uses a hexadecimal keypad. */
static const int INPUT_PAD[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
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
    for (;;) {
        int c = getch();
        if (c >= '0' && c <= '9') {
            return (uint8_t) (c - '0');
        }
        if (c >= 'a' && c <= 'f') {
            return (uint8_t) (c - 'a');
        }
    }
}

void Inp_cycle(void)
{
    unsigned int i;

    for (i = 0; i < KEY_COUNT; i++) {
        /* Using windows API for keyboard state. */
        unsigned short key_state = (unsigned short) GetAsyncKeyState(INPUT_PAD[i]);
        key[i] = (uint8_t) (key_state > 0);
    }
}

void Inp_print(void)
{
    unsigned int i;

    printf("%s", "Key: ");
    for (i = 0; i < KEY_COUNT; i++) {
        printf("%d ", key[i]);
    }
    printf("\n");
}
