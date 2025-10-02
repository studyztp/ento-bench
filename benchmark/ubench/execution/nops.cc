#include "common.h"

extern "C" {

// This macro generates a block of 4 16-bit nop in assembly because the
// instruction line is 64-bit wide for STM32G4.
// This is used to test the behavior of prefetching and instruction refill.

#define JUST_NOPS_16BIT \
    asm volatile(                     \
        ".thumb                 \n"   \
        ".rept 4                \n"   \
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
