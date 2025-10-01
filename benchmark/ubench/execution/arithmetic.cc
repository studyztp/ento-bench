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
        : "r0", "memory");

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        JUST_AN_OP;
        asm volatile("" ::: "memory");
    }
}
}
