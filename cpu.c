#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <mem.h>
#include <stdio.h>
#include <time.h>

#include "cpu.h"
#include "screen.h"
#include "input.h"

static const int K = 1024;
static const uint16_t APPLICATION_START = 0x200;

static uint16_t opcode;
static uint8_t *memory;
static uint8_t *V;
static const int F = 15;
static uint16_t I;
static uint16_t pc;

// Only really need one bit each, Replace with bit vector.
// uint8_t gfx[64 * 32];

static uint8_t delay_timer;
static uint8_t sound_timer;

static uint16_t *stack;
static uint16_t sp;

void Cpu_init() {
    sp = 0;
    pc = APPLICATION_START;
    opcode = 0;
    I = 0;

    delay_timer = 0;
    sound_timer = 0;

    // Reserve space for RAM and registers, and clear the memory.
    memory = calloc(4 * K, sizeof *memory);
    assert(memory);
    V = calloc(16, sizeof *V);
    assert(V);
    stack = calloc(8, sizeof *stack);
    assert(stack);

    // Seed the RNG.
    srand(time(NULL));
}

void Cpu_cycle() {
    // Fetch current instruction.
    opcode = (uint16_t) (memory[pc] << 8U) | memory[pc + 1];

    // Decode instruction.
    uint8_t first_byte = (opcode >> 8) & 0xFF;
    uint8_t second_byte = (opcode >> 0) & 0xFF;

    uint8_t first_nibble = (opcode >> 12) & 0xF;
    uint8_t second_nibble = (opcode >> 8) & 0xF;
    uint8_t third_nibble = (opcode >> 4) & 0xF;
    uint8_t fourth_nibble = (opcode >> 0) & 0xF;

    switch (first_nibble) {
        case 0x0:
            if (second_nibble == 0x0) {
                if (fourth_nibble == 0x0) {
                    Scr_clear();
                }
                else if (fourth_nibble == 0xE) {
                    // Return from subroutine.
                    sp--;
                    pc = stack[sp];
                }
            }
            else {
                // TODO: unimplemented.
            }
            break;

        case 0x1:
            // 1NNN: Goto address NNN.
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
                    // todo:
                    break;

                case 0x15:
                    delay_timer = V[second_nibble];
                    break;

                case 0x18:
                    sound_timer = V[second_nibble];
                    break;

                case 0x1E:
                    I = V[second_nibble];
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
                    // todo:
                    break;
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

// Test
int main() {
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