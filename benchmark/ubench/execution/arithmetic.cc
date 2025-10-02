#include "common.h"

extern "C" {

#ifndef OP
#define OP add
#endif // OP

#define JUST_AN_OP \
    asm volatile(                     \
        ".thumb                 \n"   \
        STR(OP) "s r0, #1       \n"   \
        STR(OP) "s r1, #1       \n"   \
        STR(OP) "s r2, #1       \n"   \
        STR(OP) "s r3, #1       \n"   \
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
    REPEAT_N(ARRAY_SIZE, JUST_AN_OP);  // No () needed for macro
}
}
