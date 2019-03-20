#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "chip8.h"
#include "cpu.h"
#include "input.h"
#include "screen.h"

/* The system's main memory (RAM). */
static uint8_t *memory;

/* Emulate the CHIP-8 system, loading in a ROM from the file specified by the first command line argument. */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <rom_file>", argv[0]);
        return EXIT_FAILURE;
    }

    if (Ch8_init(argv[1]) == INIT_STATUS_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

enum INIT_STATUS Ch8_init(char *rom_file_name)
{
    FILE *interpreter_data, *rom;
    unsigned int i;

    /* The chip-8 requires some data intrinsic to the interpreter in memory before the ROM. */
    interpreter_data = fopen(INTERPRETER_DATA_FILE_NAME, "rb");
    if (!interpreter_data) {
        fprintf(stderr, "The interpreter data could not be loaded. "
                        "Ensure that the '%s' file is present in the working directory.", INTERPRETER_DATA_FILE_NAME);
        return INIT_STATUS_FAILURE;
    }

    rom = fopen(rom_file_name, "rb");
    if (!rom) {
        fclose(interpreter_data);
        fprintf(stderr, "The ROM file could not be loaded. "
                        "Ensure that the '%s' file is present in the working directory.", rom_file_name);
        return INIT_STATUS_FAILURE;
    }

    /* Allocate main memory for the system. */
    memory = calloc(MEMORY_SIZE, sizeof *memory);
    if (!memory) {
        fclose(interpreter_data);
        fclose(rom);
        return INIT_STATUS_FAILURE;
    }

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

    /* Driving the system consists of continuously cycling the cpu and updating the screen. */
    for (;;) {
        for (i = 0; i < 30; i++) {
            Cpu_cycle();
        }
        //Cpu_print_memory();
        system("cls");
        Scr_print();
        //sleep(1);
    }

    return INIT_STATUS_SUCCESS;
}