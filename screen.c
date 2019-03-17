//
// Created by Joseph on 3/14/2019.
//

#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include "screen.h"

static const int PIXEL_COUNT = 64 * 32;

static uint8_t *gfx;

void Scr_init(void)
{
    gfx = calloc(PIXEL_COUNT, sizeof(uint8_t));
    assert(gfx);
}

void Scr_clear()
{
    unsigned int i;

    for (i = 0; i < PIXEL_COUNT; i++) {
        gfx[i] = 0;
    }
}