#include <stdio.h>
#include <stdint.h>
#include <time.h>

void Port_clear_screen() {
    printf("\033[2J\033[H");
}

void Port_delay(uint16_t ms) {
    clock_t end_clocks = clock() + ms * (CLOCKS_PER_SEC / 1000.0);
    while (clock() < end_clocks);
}
