#include "common.h"
extern "C" {
static volatile int always_true = 1;

// This macro inserts an always-true branch instruction sequence into the code.
// For STM32-G4 with single bank flash, it has 64-bit instruction lines
// (2 × 32-bit instructions). The prefetcher fetches ahead to fill its buffer.
// We align the branch (.p2align 4) and add NOPs to ensure the branch target
// is outside the current prefetch buffer, allowing us to measure the branch
// prediction and prefetch behavior accurately.
// Additionally, the ICache line size is 64 bits, with streaming branches, the
#define ALWAYS_TRUE_BENCH \
    asm volatile(                     \
        ".p2align 4             \n"   \
        "  ldr.w    r2, [%0]    \n"   \
        "  cmp.w    r2, #0      \n"   \
        "  bne.w    1f          \n"   \
        "  nop.w                \n"   \
        ".p2align 4             \n"   \
        PAYLOAD_NOPS(4)               \
        "1: nop.w               \n"   \
        :                             \
        : "r"(&always_true)           \
        : "r2", "cc", "memory");      

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // Initialize the target pointer to point to a valid memory location.
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    REPEAT_N(ARRAY_SIZE, ALWAYS_TRUE_BENCH);
}// ICache also can not affect the misprediction penalty.
}