//
// Created by Joseph on 3/16/2019.
//

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "cpu.h"
#include "input.h"
#include "screen.h"

static const int K = 1024;
static const uint16_t APPLICATION_START = 0x200;

static uint8_t *memory;

/* Emulate the CHIP-8 system. */
void main(int argc, char *argv[])
{
    unsigned int i;
    FILE *rom;

    assert(argc == 2);
    rom = fopen(argv[1], "rb");
    assert(rom);

    /* Allocate main memory for the system. */
    memory = calloc(4 * K, sizeof *memory);
    assert(memory);

    /* Load in the desired ROM. */
    fread(memory + APPLICATION_START, sizeof *memory, (4 * K) - APPLICATION_START, rom);

    /* Initialize all hardware modules and enable the cpu last. */
    Scr_init();
    Inp_init();
    Cpu_init(memory);

    /* Driving the system consists of collecting user input and continuously cycling the cpu. */
    for (;;) {
        printf("loop\n");
        for (i = 0; i < 500; ++i) {
            Inp_cycle();
            Cpu_cycle();
        }
        sleep(1);
    }
}