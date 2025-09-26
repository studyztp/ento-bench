#ifndef COMMON_H
#define COMMON_H

// Common includes and definitions for the ubench benchmark
#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE 1024
#define STRIDE 4

#ifndef M5OP_DEST
#define M5OP_DEST 0x20020000u
#endif

// Define the M5 operation codes
#define M5OP_EXIT              0x21
#define M5OP_WORK_BEGIN        0x5A
#define M5OP_WORK_END          0x5B
#define M5OP_RESET_STATS       0x40
#define M5OP_DUMP_RESET_STATS  0x42

// Address encoding and halfword helpers
#define M5_ADDR(code) ((uint32_t)(M5OP_DEST) + 
                                            (((uint32_t)(code) & 0xFFu) << 8))

#define M5_POKE_IMM(op_)                                          \
  __asm__ volatile                                                     \
  ( ".syntax unified\n\t"                                             \
    "push {r0-r5}\n\t"      /* Save current state */                 \
    "mov r0, #0\n\t"        /* r0 = 0 (first arg) */                 \
    "mov r1, #0\n\t"        /* r1 = 0 (second arg) */                \
    "mov r2, #0\n\t"        /* r2 = 0 (third arg) */                 \
    "mov r3, #0\n\t"        /* r3 = 0 (fourth arg) */                \
    "mov r4, #0\n\t"        /* r4 = 0 (fifth arg) */                 \
    "ldr r5, =%c0\n\t"      /* Load M5 address */                    \
    "strb r0, [r5]\n\t"     /* Trigger M5 operation */               \
    "pop {r0-r5}\n\t"       /* Restore state */                      \
    :                                                                 \
    : "i"(M5_ADDR(M5OP_ ## op_))                                     \
    : "memory" );

// Exported functions (match your assembly intent)
static inline void m5_exit() {
  M5_POKE_IMM(EXIT);
}
static inline void m5_work_begin() {
  M5_POKE_IMM(WORK_BEGIN);
}
static inline void m5_work_end() {
  M5_POKE_IMM(WORK_END);
}

static inline void m5_reset_stats() {
  M5_POKE_IMM(RESET_STATS);
}
static inline void m5_dump_reset_stats() {
  M5_POKE_IMM(DUMP_RESET_STATS);
}

#endif // COMMON_H