#include "common.h"

extern "C" {

static volatile int always_false = 0;

__attribute__ ((noinline))
void branch_to () {asm volatile("nop.w");}

// This macro inserts an always-false branch instruction sequence.
// This matches the always-true version except the branch is never taken. By
// matching the instruction sequence, we can compare the behavior of 
// always-true and always-false branches to have fair comparison.
// TODO: there is one more NOP in always-false version, need to fix it.
#define ALWAYS_FALSE_BENCH \
    asm volatile(                     \
        ".p2align 4             \n"   \
        "  ldr.w    r2, [%0]    \n"   \
        "  cmp.w    r2, #0      \n"   \
        "  bne.w    branch_to   \n"   \
        PAYLOAD_NOPS(2)               \
        :                             \
        : "r"(&always_false)          \
        : "r2", "cc", "memory");      

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    REPEAT_N(ARRAY_SIZE, ALWAYS_FALSE_BENCH);
}
}
