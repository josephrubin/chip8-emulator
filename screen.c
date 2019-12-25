#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "constant.h"
#include "port.h"
#include "screen.h"

/* A bit vector representing the system's monochrome screen.
 * Each bit represents the on/off state of a single pixel. */
static uint8_t *display;

enum bool Screen_init(void)
{
    /* The screen size should always be a whole number of bytes. */
    assert(PIXEL_COUNT % CHAR_BIT == 0);

    display = calloc(PIXEL_COUNT / CHAR_BIT, sizeof *display);
    return display == NULL ? FALSE : TRUE;
}

enum bool Screen_paint(uint8_t x, uint8_t y, uint8_t value)
{
    uint16_t pixel_index;
    uint8_t pixel_byte, pixel_bit;
    uint8_t old_value;

    assert(value == 0 || value == 1);

    /* The x and y coordinates are allowed to be outside of the bounds of the
     * screen (they are wrapped around). */
    x %= WIDTH_PIXEL_COUNT;
    y %= HEIGHT_PIXEL_COUNT;

    pixel_index = y * WIDTH_PIXEL_COUNT + x;
    pixel_byte = pixel_index / CHAR_BIT;
    pixel_bit = pixel_index % CHAR_BIT;

    /* On the CHIP-8, pixels are XORed onto the screen when they are painted. */
    old_value = (display[pixel_byte] >> pixel_bit) & 1u;
    display[pixel_byte] ^= value << pixel_bit;

    /* Our return value indicates collision.
     * If we just turned off a pixel, return true. */
    return old_value && value ? TRUE : FALSE;
}

//todo: change name to make clear diff btwn this and Port_clear_display
void Screen_clear(void)
{
    memset(display, 0, PIXEL_COUNT / CHAR_BIT);
}

void Screen_display(void) {
    Port_display_screen(display);
}

void Screen_uninit(void)
{
    free(display);
}
