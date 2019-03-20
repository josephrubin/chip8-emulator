#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <afxres.h>
#include <conio.h>

#include "input.h"

/* The system uses a hexadecimal keypad with these labels. */
static const int INPUT_PAD[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

/* The number of input keys to the system. */
static const int KEY_COUNT = 16;

enum INIT_STATUS Inp_init(void)
{
    return INIT_STATUS_SUCCESS;
}

uint8_t Inp_is_pressed(uint8_t key_number)
{
    assert(key_number < KEY_COUNT);

    /* Using windows API for keyboard state. */
    unsigned short key_state = (unsigned short) GetAsyncKeyState(INPUT_PAD[key_number]);
    return (uint8_t) (key_state > 0);
}

uint8_t Inp_blocking_next(void)
{
    for (;;) {
        int c = getch();
        if (c >= '0' && c <= '9') {
            return (uint8_t) (c - '0');
        }
        if (c >= 'a' && c <= 'f') {
            return (uint8_t) (9 + c - 'a');
        }
    }
}

void Inp_print(void)
{
    uint8_t i;

    printf("%s", "Key: ");
    for (i = 0; i < KEY_COUNT; i++) {
        printf("%d ", Inp_is_pressed(i));
    }

    printf("\n");
}
