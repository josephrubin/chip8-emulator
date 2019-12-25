#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "constant.h"

void Port_display_screen(uint8_t *display) {
    unsigned int i, j;

    for (i = 0; i < PIXEL_COUNT / CHAR_BIT; i++) {
        if (i % (WIDTH_PIXEL_COUNT / CHAR_BIT) == 0) {
            printf("\n");
        }
        for (j = 0; j < CHAR_BIT; j++) {
            printf("%c ", (display[i] >> j) & 1u ? '#' : ' ');
        }
    }

    printf("\n");
}

void Port_clear_screen() {
    /* The terminal gets cleared when the following sequence is printed. */
    printf("\033[2J\033[H");
}

void Port_delay(uint16_t ms) {
    clock_t end_clocks = clock() + ms * (CLOCKS_PER_SEC / 1000.0);
    while (clock() < end_clocks);
}
