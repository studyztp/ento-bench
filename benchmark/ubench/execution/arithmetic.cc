#include "common.h"

extern "C" {

#ifndef OP
#define OP add
#endif // OP

#define JUST_AN_OP \
    asm volatile(                     \
        ".thumb                 \n"   \
        #OP " r0, r0, #1        \n"   \
        :                             \
        :                             \
        : "r0", "memory");

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        JUST_AN_OP;
    }
}
}
