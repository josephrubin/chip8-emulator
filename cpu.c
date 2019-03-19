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

    uint8_t first_byte = (opcode >> 8) & 0xFF;
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
            } else if (second_byte == 0xEE) {
                /* 00EE: Return from subroutine. */
                sp--;
                pc = stack[sp];
            } else {
                /* Purposely not implemented, as it is not needed. */
                fprintf(stderr, "Unknown opcode: %x\n", opcode);
                assert(0);
            }
            break;

        case 0x1:
            /* 1NNN: Goto address NNN. */
            pc = opcode & 0x0FFF;
            break;

        case 0x2:
            // 1NNN: Call address NNN.
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;

        case 0x3:
            // 3XNN: Skip next instruction if VX == NN.
            if (V[second_nibble] == second_byte) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x4:
            // 4XNN: Skip next instruction if VX != NN.
            if (V[second_nibble] != second_byte) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x5:
            // 5XY0: Skip next instruction if VX != VY.
            if (V[second_nibble] != V[third_nibble]) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0x6:
            // 6XNN: VX = NN.
            V[second_nibble] = second_byte;
            pc += 2;
            break;

        case 0x7:
            // 7XNN: VX += NN.
            V[second_nibble] += second_byte;
            pc += 2;
            break;

        case 0x8:
            switch (fourth_nibble) {
                case 0x0:
                    // 8XY0: VX = VY.
                    V[second_nibble] = V[third_nibble];
                    break;

                case 0x1:
                    // 8XY1: VX |= VY.
                    V[second_nibble] |= V[third_nibble];
                    break;

                case 0x2:
                    // 8XY2: VX &= VY.
                    V[second_nibble] &= V[third_nibble];
                    break;

                case 0x3:
                    // 8XY3: VX ^= VY.
                    V[second_nibble] ^= V[third_nibble];
                    break;

                case 0x4:
                    // 8XY4: VX += VY.
                    V[F] = 0;
                    if ((uint16_t) V[second_nibble] + V[third_nibble] < V[second_nibble]) {
                        V[F] = 1;
                    }
                    V[second_nibble] += V[third_nibble];
                    break;

                case 0x5:
                    // 8XY5: VX -= VY.
                    V[F] = 0;
                    if ((uint16_t) V[second_nibble] - V[third_nibble] > V[second_nibble]) {
                        V[F] = 1;
                    }
                    V[second_nibble] -= V[third_nibble];
                    break;

                case 0x6:
                    // 8XY6: VX >>= VY.
                    assert(second_nibble == third_nibble);
                    V[F] = V[second_nibble] & 1;
                    V[second_nibble] >>= 1;
                    break;

                case 0x7:
                    // 8XY7: VX = VY - VX.
                    V[F] = 0;
                    if ((uint16_t) V[third_nibble] - V[second_nibble] > V[third_nibble]) {
                        V[F] = 1;
                    }
                    V[second_nibble] = V[third_nibble] - V[second_nibble];
                    break;

                case 0xE:
                    // 8XYE: VX <<= VY.
                    assert(second_nibble == third_nibble);
                    V[F] = V[second_byte] & ~(~0 >> 1);
                    V[second_nibble] <<= 1;
                    break;
            }
            pc += 2;
            break;

        case 0x9:
            // 9XY0: Skip next instruction if VX != VY.
            if (V[second_nibble] != V[third_nibble]) {
                pc += 2;
            }
            pc += 2;
            break;

        case 0xA:
            // ANNN: I = NNN.
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB:
            // BNNN: goto V0 + NNN.
            pc = V[0] + (opcode & 0x0FFF);
            break;

        case 0xC:
            // CXNN: VX = random byte & NN.
            /* TODO: Uniform randomness. */
            V[second_nibble] = (rand() % 256) & second_byte;
            pc += 2;
            break;

        case 0xD:
            /* DXYN: Draw sprite at (x,y)=(VX,VY), (width,height)=(8,N). */
            // todo: VF
            {
                unsigned int i, j;
                uint16_t bitstring_location;

                V[F] = 0;
                bitstring_location = I;
                for (i = 0; i < fourth_nibble; i++) {
                    for (j = 0; j < 8; j++) {
                        Scr_paint(V[second_nibble], V[third_nibble], (memory[bitstring_location] >> (8 - j)) & 1);
                    }
                    bitstring_location++;
                }
            }
            break;

        case 0xE:
            if (third_nibble == 0x9) {
                // EX9E: skip if VX key is pressed.
                if (Inp_is_pressed(V[second_byte])) {
                    pc += 2;
                }
            }
            else if (third_nibble == 0xA) {
                // EXA1: skip if VX key isn't pressed.
                if (!Inp_is_pressed(V[second_byte])) {
                    pc += 2;
                }
            }

            pc += 2;
            break;

        case 0xF:
            switch (second_byte) {
                case 0x07:
                    V[second_nibble] = delay_timer;
                    break;

                case 0x0A:
                    V[second_nibble] = Inp_blocking_next_key();
                    break;

                case 0x15:
                    delay_timer = V[second_nibble];
                    break;

                case 0x18:
                    sound_timer = V[second_nibble];
                    break;

                case 0x1E:
                    I += V[second_nibble];
                    break;

                case 0x29:
                    // todo:
                    break;

                case 0x33:
                    // todo:
                    break;

                case 0x55:
                    memcpy(memory + I, V, 16);
                    break;

                case 0x65:
                    memcpy(V, memory + I, 16);
                    break;

                default:
                    assert(0 /* Invalid opcode. */);
            }
            pc += 2;
            break;

        default:
            assert(0 /* Invalid opcode. */);
    }

    if (sound_timer > 0) {
        sound_timer--;
        if (sound_timer == 0) {
            // TODO: make sound. Call sound device stub.
        }
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