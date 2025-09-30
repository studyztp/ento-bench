#ifndef ROI_H
#define ROI_H

#ifdef STM32_BUILD
#include <ento-mcu/timing.h>
#endif

#ifdef GEM5
// #include <sys/syscalls.c>
//#include <gem5/m5ops.h>
#endif

#include <cstring>
#include <cstdio>
#include <cstdint>
  
#ifdef ARM_GEM5
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
#define M5_ADDR(code) ((uint32_t)(M5OP_DEST) + \
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
#endif


/*
#ifdef ARM_GEM5
//#define M5OP_RESET_STATS 
// Define the macro for m5op functions
#define M5OP_FUNC(name, func) \
  __asm__ __volatile__ ( \
    ".syntax unified\n\t" \
    ".thumb\n\t" \
    ".align 2\n\t" \
    ".globl " #name "\n\t" \
    ".thumb_func\n\t" \
    #name ":\n\t" \
    ".short 0xEE00 | (" #func ")\n\t" \
    ".short 0x0110\n\t" \
    "bx lr\n\t" \
    : : : "r0", "r1", "r2", "r3" \
  )
// Use the macro to define the required m5op functions
M5OP_FUNC(m5_reset_stats, 0x40)       // Example function, replace func values as needed
M5OP_FUNC(m5_dump_stats, 0x41)
M5OP_FUNC(m5_dump_reset_stats, 0x42)
M5OP_FUNC(m5_exit, 0x21)

#undef M5OP_FUNC
#endif
*/
// Add other m5op functions as needed below.
//
// ==============================================


// ==============================================
// ROI

// Struct to encapsulate all ROI metrics
struct ROIMetrics {
    uint64_t elapsed_cycles;
    uint32_t delta_cpi;
    uint32_t delta_fold;
    uint32_t delta_lsu;
    uint32_t delta_exc;
};

// Global last counts
extern volatile uint32_t last_cycle_count;
extern volatile uint32_t last_cpi_count;
extern volatile uint32_t last_fold_count;
extern volatile uint32_t last_lsu_count;
extern volatile uint32_t last_exc_count;

// Inline function prototypes
static inline void init_roi_tracking();
static inline void start_roi(void);
static inline void end_roi(void);
static inline ROIMetrics get_roi_stats();

// Inline Implementations
static inline
void init_roi_tracking()
{
#if defined(STM32_BUILD)
  disable_dwt();
  if (!is_cycle_counter_enabled()) init_cycle_counter();
  if (!is_cpi_counter_enabled()) init_cpi_counter();
  if (!is_fold_counter_enabled()) init_fold_counter();
  if (!is_lsu_counter_enabled()) init_lsu_counter();
  if (!is_exc_counter_enabled()) init_exc_counter();
#endif
#if defined(STM32_BUILD) & defined(LATENCY_MEASUREMENT)
  latency_pin_enable();
  trigger_pin_enable();
#endif
}


static inline
void start_roi(void)
{
  __asm__ volatile("" ::: "memory");
#ifdef STM32_BUILD
  //@TODO Disable Clear it Enable it. All in one go
  disable_and_reset_all_counters(); // Disables and resets counters
#if !defined(STM32G0)
  last_fold_count = 0;
  last_exc_count = 0;
  last_lsu_count = 0;
  last_cpi_count = 0;
  last_cycle_count = 0;
#endif
#if defined(LATENCY_MEASUREMENT)
  latency_pin_high();
#endif
  enable_all_counters();
  //last_fold_count = get_fold_count();
  //last_exc_count = get_exc_count();
  //last_lsu_count = get_lsu_count();
  //last_cpi_count = get_cpi_count();
  //last_cycle_count = get_cycle_count();
#elif defined(RISCV_GEM5) || defined(ARM_GEM5)
  // M5OP_RESET_STATS;
  m5_work_begin();
#endif
  __asm__ volatile("" ::: "memory");
}

static inline
void end_roi(void)
{
  __asm__ volatile("" ::: "memory");
#if defined(STM32_BUILD)
  //@TODO Disable all in one go.
  disable_all_counters();
#if defined(LATENCY_MEASUREMENT)
  latency_pin_low();
#endif // defined(LATENCY_MEASUREMENT)
#elif defined(RISCV_GEM5) || defined(ARM_GEM5)
  // M5OP_DUMP_RESET_STATS;
  m5_work_end();
#endif // defined(STM32_BUILD)
  __asm__ volatile("" ::: "memory");
}

static inline
ROIMetrics get_roi_stats(void)
{
#ifdef STM32_BUILD
  uint32_t current_cycle_count = get_cycle_count();
  uint32_t current_cpi_count = get_cpi_count();
  uint32_t current_fold_count = get_fold_count();
  uint32_t current_lsu_count = get_lsu_count();
  uint32_t current_exc_count = get_exc_count();

#if !defined(STM32G0)
  // Calculate deltas and populate the struct
  ROIMetrics metrics = {
    .elapsed_cycles = calculate_elapsed(last_cycle_count, current_cycle_count),
    .delta_cpi = calculate_elapsed(last_cpi_count, current_cpi_count),
    .delta_fold = calculate_elapsed(last_fold_count, current_fold_count),
    .delta_lsu = calculate_elapsed(last_lsu_count, current_lsu_count),
    .delta_exc = calculate_elapsed(last_exc_count, current_exc_count)
  };
#else
  ROIMetrics metrics = {};
#endif // !defined(STM32G0)
  return metrics;
#elif defined(RISCV_GEM5) || defined(ARM_GEM5)
  ROIMetrics metrics = {};
  return metrics;
#elif defined(NATIVE)
  ROIMetrics metrics = {};
  return metrics;
#endif
}

#endif // ROI_H
