//
// Created by Joseph on 3/14/2019.
//

#include "cpu.h"

void main()
{
    Cpu_init();

    for (;;) {
        Cpu_cycle();
    }
}