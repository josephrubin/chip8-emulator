#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "chip8.h"
#include "cpu.h"
#include "input.h"
#include "port.h"
#include "screen.h"

/* The system's main memory (RAM). */
static uint8_t *memory;

/* Emulate the CHIP-8 system, loading in a ROM from the file specified by the
 * first command line argument. */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <rom_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return Chip8_turn_on(argv[1]) ? EXIT_SUCCESS : EXIT_FAILURE;
}

enum bool Chip8_turn_on(char *rom_file_name) {
    FILE *interpreter_data, *rom;

    /* The chip-8 requires some data intrinsic to the interpreter in memory
     * before the ROM. */
    interpreter_data = fopen(INTERPRETER_DATA_FILE_NAME, "rb");
    if (!interpreter_data) {
        fprintf(stderr, "The interpreter data could not be loaded. "
                        "Ensure that the '%s' file is present in the "
                        "working directory.\n", INTERPRETER_DATA_FILE_NAME);
        return FALSE;
    }

    rom = fopen(rom_file_name, "rb");
    if (!rom) {
        fclose(interpreter_data);
        fprintf(stderr, "The ROM file could not be loaded. "
                        "Ensure that the '%s' file is present in the "
                        "working directory.\n", rom_file_name);
        return FALSE;
    }

    /* Allocate main memory for the system. */
    memory = calloc(MEMORY_SIZE, sizeof *memory);
    if (!memory) {
        fclose(interpreter_data);
        fclose(rom);
        return FALSE;
    }

    /* Load in the interpreter data which is constant
     * regardless of the ROM. */
    fread(memory, sizeof *memory, APPLICATION_START, interpreter_data);
    fclose(interpreter_data);

    /* Load in the ROM. */
    fread(memory + APPLICATION_START, sizeof *memory,
          MEMORY_SIZE - APPLICATION_START, rom);
    fclose(rom);

    /* Initialize all hardware modules. */
    if (!Screen_init()) {
        free(memory);
        return FALSE;
    }
    if (!Inp_init()) {
        Screen_uninit();
        free(memory);
        return FALSE;
    }
    if (!Cpu_init(memory)) {
        Inp_uninit();
        Screen_uninit();
        free(memory);
        return FALSE;
    }

    /* Driving the system consists of continuously cycling the cpu and
     * updating the screen. */
    for (;;) {
        enum bool draw = FALSE;
        unsigned int i;

        /* Run a number of CPU cycles at once. */
        for (i = 0; i < 10; i++) {
            enum bool invalidate_display;
            if (!Cpu_cycle(&invalidate_display)) {
                /* Invalid execution or bad CPU state, kill the emulator. */
                Cpu_uninit();
                Inp_uninit();
                Screen_uninit();
                free(memory);
                return FALSE;
            }

            draw = draw || invalidate_display;
        }

        if (draw) {
            Port_clear_screen();
            Screen_display();
        }

        Port_delay(DELAY_MS);
    }
    
    /* We should never reach this because the chip8 can't be shut off. */
    assert(0);
}
