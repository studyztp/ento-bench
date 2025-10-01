#include "common.h"

extern "C" {

static volatile int always_false = 0;

__attribute__ ((noinline))
void branch_to () {asm volatile("nop.w");}

// This macro inserts an always-false branch instruction sequence.
// This matches the always-true version except the branch is never taken. 
// For STM32-G4, ldr.w (1 cycle cuz cached), cmp.w (1 cycle), 
// bne.w (not taken: 1 cycle), and 5 NOPs (1 cycle each).
// The STM32-G4 is a 3 stage pipeline (Fetch, Decode, Execute) so the expected
// taken penalty is 1+1+1+5 = 8 cycles, so the expected total cycles is:
// Measured_Cycles = ARRAY_SIZE × (8).
// We can use this to confirm the penalty of the always-true branch.

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
