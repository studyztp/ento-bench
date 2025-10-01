#include "common.h"
extern "C" {
static volatile uint16_t always_true = 1;

// This macro inserts an always-true branch instruction sequence into the code.
// For STM32-G4 with dual bank flash, it has 64-bit instruction lines
// (4 × 16-bit instructions). The prefetcher fetches ahead to fill its buffer.
// We align the branch (.p2align 2) and add NOPs to ensure the branch target
// is outside the current prefetch buffer, allowing us to measure the branch
// prediction and prefetch behavior accurately.
// Additionally, the ICache line size is 64 bits, with streaming branches, the
// ICache won't affect the taken penalty.
// There will be ARRAY_SIZE branches taken with ldr.w (1 cycle cuz cached), 
// cmp.w (1 cycle), bne.w (taken:1+P cycles), and 4 NOPs (1 cycle each).
// The STM32-G4 is a 3 stage pipeline (Fetch, Decode, Execute) so the expected
// taken penalty is 1+1+1+P+4 = 7+P cycles, so the expected total cycles is:
// Measured_Cycles = ARRAY_SIZE × (7 + P).
#define ALWAYS_TRUE_BENCH \
    asm volatile(                     \
        "  ldr      r2, [%0]    \n"   \
        "  cmp      r2, #0      \n"   \
        "  bne      1f          \n"   \
        PAYLOAD_NOPS_16_BITS(5)               \
        "1: nop                 \n"   \
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