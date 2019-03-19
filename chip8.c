#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "cpu.h"
#include "input.h"
#include "screen.h"
#include "constant.h"

void Ch8_power_on(char *rom_file_name);

/* The system's main memory (RAM). */
static uint8_t *memory;

/* Emulate the CHIP-8 system, loading in a ROM from the file specified by the first command line argument. */
int main(int argc, char *argv[])
{
    assert(argc == 2);

    Ch8_power_on(argv[1]);

    return 0;
}

void Ch8_power_on(char *rom_file_name)
{
    unsigned int i;
    FILE *interpreter_data, *rom;

    interpreter_data = fopen(interpreter_data_file_name, "rb");
    assert(interpreter_data);

    rom = fopen(rom_file_name, "rb");
    assert(rom);

    /* Allocate main memory for the system. */
    memory = calloc(MEMORY_SIZE, sizeof *memory);
    assert(memory);

    /* Load in the interpreter data which is constant regardless of the ROM. */
    fread(memory, sizeof *memory, APPLICATION_START, interpreter_data);
    fclose(interpreter_data);

    /* Load in the ROM. */
    fread(memory + APPLICATION_START, sizeof *memory, MEMORY_SIZE - APPLICATION_START, rom);
    fclose(rom);

    /* Initialize all hardware modules and enable the cpu last. */
    Scr_init();
    Inp_init();
    Cpu_init(memory);

    /* Driving the system consists of collecting user input and continuously cycling the cpu. */
    for (;;) {
        for (i = 0; i < 500; i++) {
            Inp_cycle();
            Cpu_cycle();
        }
        Cpu_print_memory();
        Scr_print();
        sleep(1);
    }
}