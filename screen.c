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

uint8_t Scr_paint(int x, int y, uint8_t value)
{
    assert(value == 0 || value == 1);

    /* x and y are allowed to be outside of the bounds of the screen.
       The correct behavior is wraparound. */
    x %= 64;
    y %= 32;

    gfx[y * 64 + x] ^= value;

    /* Our return value indicates collision. If we just turned off a pixel, return true. */
    return value && !gfx[y * 64 + x];
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
        printf("%s ", gfx[i] ? "#" : " ");
    }

    printf("\n");
}
