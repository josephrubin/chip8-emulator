#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <time.h>

#include "cpu.h"
#include "screen.h"
#include "input.h"
#include "constant.h"

static uint8_t *memory;
static uint8_t *V;
static const int F = 15;
static uint16_t I;
static uint16_t pc;

static uint8_t delay_timer;
static uint8_t sound_timer;

static uint16_t *stack;
static uint16_t sp;

void Cpu_init(uint8_t *mem) {
    memory = mem;
    sp = 0;
    pc = APPLICATION_START;
    I = 0;

    delay_timer = 0;
    sound_timer = 0;

    /* Reserve space for RAM and registers, and clear the memory. */
    V = calloc(16, sizeof *V);
    assert(V);
    stack = calloc(8, sizeof *stack);
    assert(stack);

    /* Seed the RNG. */
    srand(time(NULL));
}

void Cpu_cycle() {
    /* Fetch current two byte instruction. */
    uint16_t opcode = memory[pc] << 8U | memory[pc + 1];

    uint8_t second_byte = (opcode >> 0) & 0xFF;

    uint8_t first_nibble = (opcode >> 12) & 0xF;
    uint8_t second_nibble = (opcode >> 8) & 0xF;
    uint8_t third_nibble = (opcode >> 4) & 0xF;
    uint8_t fourth_nibble = (opcode >> 0) & 0xF;

    /* Decode and execute instruction. */
    switch (first_nibble) {
        case 0x0:
            if (second_byte == 0xE0) {
                /* 00E0: Clear the screen. */
                Scr_clear();
                pc += 2;
            } else if (second_byte == 0xEE) {
                /* 00EE: Return from subroutine. */
                sp--;
                pc = stack[sp];
                pc += 2;
            } else {
                /* Purposely not implemented, as it is not needed. */
                fprintf(stderr, "Unknown opcode: %04x\n", opcode);
                assert(0);
            }
            break;

        case 0x1:
            /* 1NNN: Goto address NNN. */
            pc = opcode & 0x0FFF;
            break;

        case 0x2:
            /* 2NNN: Call address NNN. */
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;

        case 0x3:
            /* 3XNN: Skip next instruction if VX == NN. */
            if (V[second_nibble] == second_byte) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x4:
            /* 4XNN: Skip next instruction if VX != NN. */
            if (V[second_nibble] != second_byte) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x5:
            /* 5XY0: Skip next instruction if VX != VY. */
            assert(fourth_nibble == 0);
            if (V[second_nibble] != V[third_nibble]) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x6:
            /* 6XNN: VX = NN. */
            V[second_nibble] = second_byte;
            pc += 2;
            break;

        case 0x7:
            /* 7XNN: VX += NN. */
            V[second_nibble] += second_byte;
            pc += 2;
            break;

        case 0x8:
            switch (fourth_nibble) {
                case 0x0:
                    /* 8XY0: VX = VY. */
                    V[second_nibble] = V[third_nibble];
                    pc += 2;
                    break;

                case 0x1:
                    /* 8XY1: VX |= VY. */
                    V[second_nibble] |= V[third_nibble];
                    pc += 2;
                    break;

                case 0x2:
                    /* 8XY2: VX &= VY. */
                    V[second_nibble] &= V[third_nibble];
                    pc += 2;
                    break;

                case 0x3:
                    /* 8XY3: VX ^= VY. */
                    V[second_nibble] ^= V[third_nibble];
                    pc += 2;
                    break;

                case 0x4:
                    /* 8XY4: VX += VY. */
                    V[second_nibble] += V[third_nibble];
                    V[F] = (V[second_nibble] < V[third_nibble]);
                    pc += 2;
                    break;

                case 0x5:
                    /* 8XY5: VX -= VY. */
                    V[F] = V[second_nibble] > V[third_nibble];
                    V[second_nibble] -= V[third_nibble];
                    pc += 2;
                    break;

                case 0x6:
                    /* 8XY6: VX >>= VY. */
                    assert(second_nibble == third_nibble);
                    V[F] = V[second_nibble] & 1;
                    V[second_nibble] >>= 1;
                    pc += 2;
                    break;

                case 0x7:
                    /* 8XY7: VX = VY - VX. */
                    V[F] = V[third_nibble] > V[second_nibble];
                    V[second_nibble] = V[third_nibble] - V[second_nibble];
                    pc += 2;
                    break;

                case 0xE:
                    /* 8XYE: VX <<= VY. */
                    assert(second_nibble == third_nibble);
                    V[F] = V[second_nibble] & ~(~0 >> 1);
                    V[second_nibble] <<= 1;
                    pc += 2;
                    break;

                default:
                    assert(0);
            }
            break;

        case 0x9:
            /* 9XY0: Skip next instruction if VX != VY. */
            assert(fourth_nibble == 0);
            if (V[second_nibble] != V[third_nibble]) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0xA:
            /* ANNN: I = NNN. */
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB:
            /* BNNN: goto V0 + NNN. */
            pc = V[0] + (opcode & 0x0FFF);
            break;

        case 0xC:
            /* CXNN: VX = random byte & NN. */
            /* TODO: Uniform randomness. */
            V[second_nibble] = (rand() % 256) & second_byte;
            pc += 2;
            break;

        case 0xD: {
            /* DXYN: Draw sprite at (x,y)=(VX,VY), (width,height)=(8,N). V[F] is set if collision, otherwise cleared. */
            unsigned int i, j;
            uint16_t bitstring_location;
            uint8_t sprite_row;

            V[F] = 0;
            bitstring_location = I;
            for (i = 0; i < fourth_nibble; i++) {
                sprite_row = memory[bitstring_location];
                for (j = 0; j < 8; j++) {
                    if(Scr_paint(V[second_nibble] + j, V[third_nibble] + i, (sprite_row >> 7) & 1)) {
                        V[F] = 1;
                    }
                    sprite_row <<= 1;
                }
                bitstring_location++;
            }
            pc += 2;
            break;
        }

        case 0xE:
            if (second_byte == 0x9E) {
                /* EX9E: skip if VX key is pressed. */
                if (Inp_is_pressed(V[second_nibble])) {
                    pc += 2;
                }
                pc += 2;
            }
            else if (third_nibble == 0xA) {
                /* EXA1: skip if VX key isn't pressed. */
                if (!Inp_is_pressed(V[second_nibble])) {
                    pc += 2;
                }
                pc += 2;
            }
            else {
                assert(0);
            }
            break;

        case 0xF:
            switch (second_byte) {
                case 0x07:
                    /* FX07: VX = delay timer. */
                    V[second_nibble] = delay_timer;
                    pc += 2;
                    break;

                case 0x0A:
                    /* FX0A: VX = next key pressed (block until next input). */
                    V[second_nibble] = Inp_blocking_next_key();
                    pc += 2;
                    break;

                case 0x15:
                    /* FX15: delay timer = VX. */
                    delay_timer = V[second_nibble];
                    pc += 2;
                    break;

                case 0x18:
                    /* FX18: sound timer = VX. */
                    sound_timer = V[second_nibble];
                    pc += 2;
                    break;

                case 0x1E:
                    /* FX1E: I += VX. */
                    I += V[second_nibble];
                    pc += 2;
                    break;

                case 0x29:
                    /* FX29: I = address of sprite specified by VX. */
                    assert(V[second_nibble] <= 0xF);
                    I = DIGIT_SPRITE_LOCATION[V[second_nibble]];
                    pc += 2;
                    break;

                case 0x33: {
                    /* FX33: store the decimal representation of value at VX (hundreds, tens, units) in I, I+1, I+2. */
                    uint8_t decimal_value = V[second_nibble];
                    memory[I] = decimal_value / 100;
                    decimal_value %= 100;
                    memory[I + 1] = decimal_value / 10;
                    decimal_value %= 10;
                    memory[I + 2] = decimal_value;
                    pc += 2;
                    break;
                }

                case 0x55:
                    /* FX55: store V0 through VX in memory starting at I. */
                    assert(second_nibble <= 0xF);
                    memcpy(memory + I, V, second_nibble + 1);
                    pc += 2;
                    break;

                case 0x65:
                    /* FX65: load V0 through VX from memory starting at I. */
                    assert(second_nibble <= 0xF);
                    memcpy(V, memory + I, second_nibble + 1);
                    pc += 2;
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: %04x\n", opcode);
                    assert(0 /* Invalid opcode. */);
            }
            break;

        default:
            fprintf(stderr, "Unknown opcode: %04x\n", opcode);
            assert(0 /* Invalid opcode. */);
    }

    if (sound_timer > 0) {
        sound_timer--;
        // TODO: make sound. Call sound device stub.
    }
    if (delay_timer > 0) {
        delay_timer--;
    }
}

void Cpu_print_memory(void)
{
    unsigned int i;

    for (i = 0; i < 16; i++) {
        printf("%02x ", V[i]);
    }

    printf("I=%04x ", I);

    printf("\n");
}

/*
int main() {
    Scr_init();
    Inp_init();
    Cpu_init();

    assert(pc == 0x200);
    assert(sp == 0x0);
    assert(opcode == 0x0);
    assert(I == 0x0);
    assert(V[0] == 0);

    // Load in some sample memory.
    uint8_t sample[] = {0x70, 0x01,
                        0x73, 0x35,
                        0xA1, 0x11,
                        0xC2, 0xFF};
    memcpy(memory + 0x200, sample, sizeof sample);

    Cpu_cycle();
    assert(V[0] == 0x1);
    assert(pc == 0x202);

    Cpu_cycle();
    assert(V[3] == 0x35);

    Cpu_cycle();
    assert(I == 0x0111);

    // TODO: Check randomness.
    Cpu_cycle();
    // printf("%d", V[2]);

    printf("%s", "All tests passed.");

    return 0;
}
 */