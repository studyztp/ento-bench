#include "common.h"

extern "C" {

// This macro generates a block of 4 16-bit nop in assembly because the
// instruction line is 64-bit wide for STM32G4.
// This is used to test the behavior of prefetching and instruction refill.

#define NOPS_16BIT \
    asm volatile(                     \
        ".thumb                 \n"   \
        ".rept 4                \n"   \
        "  nop                  \n"   \
        ".endr                  \n"   \
        :                             \
        :: "memory");

// This macro generates a block of 2 32-bit nop in assembly because the
// instruction line is 64-bit wide for STM32G4.

#define NOPS_32BIT \
    asm volatile(                     \
        ".thumb                 \n"   \
        ".rept 4                \n"   \
        "  nop.w                \n"   \
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
#ifdef USE_32BIT_NOPS
    REPEAT_N(ARRAY_SIZE, NOPS_32BIT);
#elif defined(USE_16BIT_NOPS)
    REPEAT_N(ARRAY_SIZE, NOPS_16BIT);
#else
#error "Either USE_16BIT_NOPS or USE_32BIT_NOPS must be defined"
#endif  
}
}
