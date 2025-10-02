#include "common.h"

extern "C" {

// This macro inserts 8 NOP instructions to match the instruction count.
// Each NOP takes 1 cycle on STM32-G4 so the expected total cycles is:
// Measured_Cycles = ARRAY_SIZE × (8). 
// Any extra cycles measured is the overhead of measurement itself.

// This uses .thumb mode and 16-bit NOPs to ensure that the instructions
// are packed tightly without gaps that could be filled by the prefetcher.
// 

#define JUST_NOPS_16BIT \
    asm volatile(                     \
        ".thumb                 \n"   \
        ".rept 8                \n"   \
        "  nop                  \n"   \
        ".endr                  \n"   \
        :                             \
        :: "memory");

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    asm volatile(".p2align 4");
    REPEAT_N(ARRAY_SIZE, JUST_NOPS_16BIT);
}
}
