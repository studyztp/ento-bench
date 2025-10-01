#include "common.h"

extern "C" {

// 32 cache lines of 4 x 64 or 2 x 128 bits on ICode (1 KB RAM)
// 8 cache lines of 4 x 64 bits or 2 x 128 on DCode (256B RAM)


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
    REPEAT_N(ARRAY_SIZE, JUST_NOPS_16BIT);
}
}
