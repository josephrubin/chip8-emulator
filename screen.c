//
// Created by Joseph on 3/14/2019.
//

#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>

#include "screen.h"

static const int PIXEL_COUNT = 64 * 32;

/* todo: replace with bit vector. */
static uint8_t *gfx;

void Scr_init(void)
{
    gfx = calloc(PIXEL_COUNT, sizeof *gfx);
    assert(gfx);
}

void Scr_paint(int x, int y, uint8_t value)
{
    assert(x < 64);
    assert(y < 32);
    assert(value == 0 || value == 1);
    gfx[y * 64 + x] ^= value;
}

void Scr_clear()
{
    unsigned int i;

    for (i = 0; i < PIXEL_COUNT; i++) {
        gfx[i] = 0;
    }
}

void Scr_print()
{
    unsigned int i;

    for (i = 0; i < PIXEL_COUNT; i++) {
        if (i % 64 == 0) {
            printf("\n");
        }
        printf("%s ", gfx[i] ? "#" : "0");
    }

    printf("\n");
}
