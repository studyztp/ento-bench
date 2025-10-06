#include "common.h"

extern "C" {

#ifndef OP
#define OP add
#endif // OP

#define OP_16_BITS \
    asm volatile(                     \
        ".thumb                 \n"   \
        STR(OP) "s r0, #1       \n"   \
        STR(OP) "s r1, #1       \n"   \
        STR(OP) "s r2, #1       \n"   \
        STR(OP) "s r3, #1       \n"   \
        :                             \
        :                             \
        : "r0", "r1", "r2", "r3");

#define OP_32_BITS \
    asm volatile(                     \
        STR(OP) "w r0, r0, #1    \n"  \
        STR(OP) "w r1, r1, #1    \n"  \
        STR(OP) "w r2, r2, #1    \n"  \
        STR(OP) "w r3, r3, #1    \n"  \
        :                             \
        :                             \
        : "r0", "r1", "r2", "r3");


__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    asm volatile(".p2align 4");
#ifdef USE_32BIT_NOPS
    REPEAT_N(ARRAY_SIZE, OP_32_BITS);
#elif defined(USE_16BIT_NOPS)
    REPEAT_N(ARRAY_SIZE, OP_16_BITS);
#else
#error "Either USE_16BIT_NOPS or USE_32BIT_NOPS must be defined"
#endif
}
}
