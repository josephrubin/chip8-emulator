#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <mem.h>

#include "screen.h"
#include "constant.h"

/* A bit vector representing the system's monochrome screen. Each bit represents the on/off state of a single pixel. */
static uint8_t *display;

enum INIT_STATUS Scr_init(void)
{
    /* The screen size should always be a whole number of bytes. */
    assert(PIXEL_COUNT % CHAR_BIT == 0);

    display = calloc(PIXEL_COUNT / CHAR_BIT, sizeof *display);
    return display ? INIT_STATUS_SUCCESS : INIT_STATUS_FAILURE;
}

uint8_t Scr_paint(uint8_t x, uint8_t y, uint8_t value)
{
    uint16_t pixel_index;
    uint8_t pixel_byte, pixel_bit;

    assert(value == 0 || value == 1);

    /* The x and y coordinates are allowed to be outside of the bounds of the screen (they are wrapped around). */
    x %= 64;
    y %= 32;

    pixel_index = y * 64 + x;
    pixel_byte = pixel_index / CHAR_BIT;
    pixel_bit = pixel_index % CHAR_BIT;

    /* On the CHIP-8, pixels are XORed onto the screen when they are painted. */
    display[pixel_byte] ^= (value << (CHAR_BIT - pixel_bit - 1));

    /* Our return value indicates collision. If we just turned off a pixel, return true. */
    return value && !((display[pixel_byte] >> (CHAR_BIT - pixel_bit - 1)) & 1);
}

void Scr_clear(void)
{
    memset(display, 0, PIXEL_COUNT / CHAR_BIT);
}

void Scr_print(void)
{
    unsigned int i, j;

    for (i = 0; i < PIXEL_COUNT / CHAR_BIT; i++) {
        if (i % (64 / CHAR_BIT) == 0) {
            printf("\n");
        }
        for (j = 0; j < CHAR_BIT; j++) {
            printf("%c ", (display[i] >> (CHAR_BIT - j - 1)) & 1 ? '#' : ' ');
        }
    }

    printf("\n");
}
