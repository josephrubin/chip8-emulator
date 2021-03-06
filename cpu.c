#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cpu.h"
#include "screen.h"
#include "input.h"
#include "constant.h"

/* Pointer to the system memory. */
static uint8_t *memory;

/* Counts down at 60hz if above 0. */
static int16_t delay_timer;

/* Counts down at 60hz and emits a tone if above 0. */
static int16_t sound_timer;

/* -------------------------------------------------------------------------- */
/* CPU Managed Registers ---------------------------------------------------- */

/* Points to the memory address currently being executed. */
static uint16_t program_counter;

/* Stores return addresses for subroutine calls. */
static uint16_t *stack;

/* Points to the top of the stack, the next place that a return address will
 * be placed. */
static int16_t stack_pointer;

/* -------------------------------------------------------------------------- */
/* Application Managed Registers -------------------------------------------- */

/* The CPU's 16 main registers, V0-VF. */
static uint8_t *register_v;

/* The last register, VF, is used as a carry/overflow indicator. */
static const int F = 0xF;

/* An additional register I is often used by the application to hold a memory address. */
static uint16_t I;

/* -------------------------------------------------------------------------- */
/* Private Interface -------------------------------------------------------- */

/* Abort the program if the CPU is in an invalid state. */
static void check_invariants(void) {
    /* Program counter points to two bytes at once, aligned at an even
     * address. We may not execute in the interpreter data. */
    assert(program_counter % 2 == 0);
    assert(program_counter >= APPLICATION_START);
    assert(program_counter < MEMORY_SIZE);

    /* Stack pointer increments after a push, so it may point to one past
     * the end of the stack if the stack is full. */
    assert(stack_pointer >= 0);
    assert(stack_pointer <= STACK_SIZE);

    /* Timers count down until zero, then deactivate. */
    assert(delay_timer >= 0);
    assert(sound_timer >= 0);
}

enum bool Cpu_init(uint8_t *allocated_memory) {
    memory = allocated_memory;

    delay_timer = 0;
    sound_timer = 0;

    /* Recall that the ROM is loaded in at APPLICATION_START,
     * not at address 0 (which is used by the interpreter). */
    program_counter = APPLICATION_START;

    /* Allocate the registers and clear them. */
    I = 0;
    register_v = calloc(0xF, sizeof *register_v);
    if (!register_v) {
        return FALSE;
    }

    /* Allocate the stack and point to the top of it. */
    stack_pointer = 0;
    stack = malloc(STACK_SIZE * sizeof *stack);
    if (!stack) {
        free(register_v);
        return FALSE;
    }

    /* Seed the RNG. */
    srand((unsigned int) time(NULL));
    
    check_invariants();
    return TRUE;
}

enum bool Cpu_cycle(enum bool *invalidate_display) {
    check_invariants();

    /* Set when an opcode cannot be successfully decoded. */
    enum bool unknown_opcode = FALSE;

    /* Fetch current two byte instruction. */
    uint8_t first_byte = memory[program_counter];
    uint8_t second_byte = memory[program_counter + 1];
    uint16_t opcode = first_byte << 8u | second_byte;

    uint8_t first_nibble = (opcode >> 12) & 0x0F;
    uint8_t second_nibble = (opcode >> 8) & 0x0F;
    uint8_t third_nibble = (opcode >> 4) & 0x0F;
    uint8_t fourth_nibble = (opcode >> 0) & 0x0F;

    /* Set when an opode that draws to the screen is executed. */
    *invalidate_display = FALSE;

    /* Decode and execute instruction. */
    switch (first_nibble) {
        case 0x0:
            if ((opcode & 0x0FFFu) == 0x0E0) {
                /* 00E0: Clear the screen. */
                Screen_clear();
                *invalidate_display = TRUE;
                program_counter += 2;
            }
            else if ((opcode & 0x0FFFu) == 0x0EE) {
                /* 00EE: Return from subroutine. */
                stack_pointer--;
                program_counter = stack[stack_pointer];
                program_counter += 2;
            }
            else {
                unknown_opcode = TRUE;
            }
            break;

        case 0x1:
            /* 1NNN: Goto address NNN. */
            program_counter = opcode & 0x0FFF;
            break;

        case 0x2:
            /* 2NNN: Call address NNN. */
            stack[stack_pointer] = program_counter;
            stack_pointer++;
            program_counter = opcode & 0x0FFF;
            break;

        case 0x3:
            /* 3XNN: Skip next instruction if VX == NN. */
            if (register_v[second_nibble] == second_byte) {
                program_counter += 2;
            }
            program_counter += 2;
            break;

        case 0x4:
            /* 4XNN: Skip next instruction if VX != NN. */
            if (register_v[second_nibble] != second_byte) {
                program_counter += 2;
            }
            program_counter += 2;
            break;

        case 0x5:
            if (fourth_nibble == 0) {
                /* 5XY0: Skip next instruction if VX == VY. */
                if (register_v[second_nibble] == register_v[third_nibble]) {
                    program_counter += 2;
                }
                program_counter += 2;
            }
            else {
                unknown_opcode = TRUE;
            }
            break;

        case 0x6:
            /* 6XNN: VX = NN. */
            register_v[second_nibble] = second_byte;
            program_counter += 2;
            break;

        case 0x7:
            /* 7XNN: VX += NN. */
            register_v[second_nibble] += second_byte;
            program_counter += 2;
            break;

        case 0x8:
            switch (fourth_nibble) {
                case 0x0:
                    /* 8XY0: VX = VY. */
                    register_v[second_nibble] = register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x1:
                    /* 8XY1: VX |= VY. */
                    register_v[second_nibble] |= register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x2:
                    /* 8XY2: VX &= VY. */
                    register_v[second_nibble] &= register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x3:
                    /* 8XY3: VX ^= VY. */
                    register_v[second_nibble] ^= register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x4:
                    /* 8XY4: VX += VY. */
                    register_v[second_nibble] += register_v[third_nibble];
                    register_v[F] =
                        register_v[second_nibble] < register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x5:
                    /* 8XY5: VX -= VY. */
                    register_v[F] =
                        register_v[second_nibble] > register_v[third_nibble];
                    register_v[second_nibble] -= register_v[third_nibble];
                    program_counter += 2;
                    break;

                case 0x6:
                    /* 8XY6: VX >>= 1. */
                    /* Specs on this operation differ; this assertion ensures
                     * that application writers do not assume functionality
                     * unsupported by this emulator.
                     * This assertion has never failed in my testing of CHIP-8
                     * ROMS available online. */
                    assert(second_nibble == third_nibble);
                    register_v[F] = register_v[second_nibble] & 1;
                    register_v[second_nibble] >>= 1;
                    program_counter += 2;
                    break;

                case 0x7:
                    /* 8XY7: VX = VY - VX. */
                    register_v[F] =
                        register_v[third_nibble] > register_v[second_nibble];
                    register_v[second_nibble] =
                        register_v[third_nibble] - register_v[second_nibble];
                    program_counter += 2;
                    break;

                case 0xE:
                    /* 8XYE: VX <<= 1. */
                    /* Specs on this operation differ; this assertion ensures
                     * that application writers do not assume
                     * functionality unsupported by this emulator.
                     * This assertion has never failed in my testing of CHIP-8
                     * ROMS available online. */
                    assert(second_nibble == third_nibble);
                    register_v[F] = (register_v[second_nibble]
                                     & (1u << (CHAR_BIT_COUNT - 1))) != 0;
                    register_v[second_nibble] <<= 1;
                    program_counter += 2;
                    break;

                default:
                    unknown_opcode = TRUE;
                    break;
            }
            break;

        case 0x9:
            if (fourth_nibble == 0) {
                /* 9XY0: Skip next instruction if VX != VY. */
                assert(fourth_nibble == 0);
                if (register_v[second_nibble] != register_v[third_nibble]) {
                    program_counter += 2;
                }
                program_counter += 2;
            }
            else {
                unknown_opcode = TRUE;
            }
            break;

        case 0xA:
            /* ANNN: I = NNN. */
            I = opcode & 0x0FFF;
            program_counter += 2;
            break;

        case 0xB:
            /* BNNN: goto V0 + NNN. */
            program_counter = register_v[0] + (opcode & 0x0FFF);
            break;

        case 0xC:
            /* CXNN: VX = random byte & NN. */
            /* TODO: Uniform randomness. */
            register_v[second_nibble] = (rand() % 256) & second_byte;
            program_counter += 2;
            break;

        case 0xD: {
            /* DXYN: Draw sprite at (x,y)=(VX,VY), (width,height)=(8,N).
             * V[F] is set if collision, otherwise cleared. */
            unsigned int i, j;
            uint16_t bitstring_location;
            uint8_t sprite_row;

            register_v[F] = 0;
            bitstring_location = I;
            for (i = 0; i < fourth_nibble; i++) {
                sprite_row = memory[bitstring_location];
                for (j = 0; j < 8; j++) {
                    if (Screen_paint(register_v[second_nibble] + j,
                                     register_v[third_nibble] + i,
                                     (sprite_row >> 7) & 1)) {
                        register_v[F] = 1;
                    }
                    sprite_row <<= 1;
                }
                bitstring_location++;
            }
            *invalidate_display = TRUE;
            program_counter += 2;
            break;
        }

        case 0xE:
            if (second_byte == 0x9E) {
                /* EX9E: skip if VX key is pressed. */
                if (Inp_is_pressed(register_v[second_nibble])) {
                    program_counter += 2;
                }
                program_counter += 2;
            }
            else if (third_nibble == 0xA) {
                /* EXA1: skip if VX key isn't pressed. */
                if (!Inp_is_pressed(register_v[second_nibble])) {
                    program_counter += 2;
                }
                program_counter += 2;
            }
            else {
                unknown_opcode = TRUE;
            }
            break;

        case 0xF:
            switch (second_byte) {
                case 0x07:
                    /* FX07: VX = delay timer. */
                    register_v[second_nibble] = delay_timer;
                    program_counter += 2;
                    break;

                case 0x0A:
                    /* FX0A: VX = next key pressed (block until input). */
                    register_v[second_nibble] = Inp_blocking_next();
                    program_counter += 2;
                    break;

                case 0x15:
                    /* FX15: delay timer = VX. */
                    delay_timer = register_v[second_nibble];
                    program_counter += 2;
                    break;

                case 0x18:
                    /* FX18: sound timer = VX. */
                    sound_timer = register_v[second_nibble];
                    program_counter += 2;
                    break;

                case 0x1E:
                    /* FX1E: I += VX. */
                    I += register_v[second_nibble];
                    program_counter += 2;
                    break;

                case 0x29:
                    /* FX29: I = address of sprite specified by VX. */
                    assert(register_v[second_nibble] <= 0xF);
                    I = DIGIT_SPRITE_LOCATION[register_v[second_nibble]];
                    program_counter += 2;
                    break;

                case 0x33: {
                    /* FX33: store the decimal representation of value at
                     * VX (hundreds, tens, units) in I, I+1, I+2. */
                    uint8_t decimal_value = register_v[second_nibble];
                    memory[I] = decimal_value / 100;
                    decimal_value %= 100;
                    memory[I + 1] = decimal_value / 10;
                    decimal_value %= 10;
                    memory[I + 2] = decimal_value;
                    program_counter += 2;
                    break;
                }

                case 0x55:
                    /* FX55: store V0 through VX in memory starting at I. */
                    assert(second_nibble <= 0xF);
                    memcpy(memory + I, register_v, second_nibble + 1);
                    program_counter += 2;
                    break;

                case 0x65:
                    /* FX65: load V0 through VX from memory starting at I. */
                    assert(second_nibble <= 0xF);
                    memcpy(register_v, memory + I, second_nibble + 1);
                    program_counter += 2;
                    break;

                default:
                    unknown_opcode = TRUE;
            }
            break;

        default:
            unknown_opcode = TRUE;
    }

    /* todo: timer depletion should be done at 60hz somewhere else, independent of the cpu cycle. */
    if (sound_timer > 0) {
        sound_timer--;
        /* todo: make sound. */
    }
    if (delay_timer > 0) {
        delay_timer--;
    }

    if (unknown_opcode) {
        /* If there was an issue decoding the opcode,
         * there was no execution. */

        /* Output to stderr allows the user to know that the program is not
         * operating perfectly on the given ROM. */
        fprintf(stderr, "Unknown opcode: %04x\n", opcode);

        /* Increment the program counter so we can continue execution if the
         * system decides to ignore this error. */
        program_counter += 2;

        check_invariants();
        return FALSE;
    }

    check_invariants();
    return TRUE;
}

void Cpu_print_memory(void)
{
    check_invariants();

    uint8_t i;

    for (i = 0; i < 16; i++) {
        printf("%02x ", register_v[i]);
    }

    printf("I=%04x \n", I);

    check_invariants();
}

void Cpu_uninit(void)
{
    check_invariants();

    free(stack);
    free(register_v);
}
