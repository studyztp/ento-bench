#ifndef COMMON_H
#define COMMON_H

#include <ento-bench/harness.h>
#include <ento-util/file_path_util.h>
#include <ento-util/debug.h>
#include <ento-util/unittest.h>
#include <ento-bench/bench_config.h>

#ifndef GEM5
#include <ento-mcu/cache_util.h>
#include <ento-mcu/flash_util.h>
#include <ento-mcu/clk_util.h>
#include <ento-mcu/systick_config.h>
#endif // GEM5

extern "C" void initialise_monitor_handles(void);

using namespace EntoBench;
using namespace EntoUtil;

#define ARRAY_SIZE 32
#define STRIDE 4
#define NUM_WARMUP 0

// Macro to repeat a statement n times (up to 4096)
// This makes sure that the statements are repeated in place without loops to
// avoid additional branches.
#define REPEAT_1(x) x
#define REPEAT_2(x) REPEAT_1(x) REPEAT_1(x)
#define REPEAT_4(x) REPEAT_2(x) REPEAT_2(x)
#define REPEAT_8(x) REPEAT_4(x) REPEAT_4(x)
#define REPEAT_16(x) REPEAT_8(x) REPEAT_8(x)
#define REPEAT_32(x) REPEAT_16(x) REPEAT_16(x)
#define REPEAT_64(x) REPEAT_32(x) REPEAT_32(x)
#define REPEAT_128(x) REPEAT_64(x) REPEAT_64(x)
#define REPEAT_256(x) REPEAT_128(x) REPEAT_128(x)
#define REPEAT_512(x) REPEAT_256(x) REPEAT_256(x)
#define REPEAT_1024(x) REPEAT_512(x) REPEAT_512(x)
#define REPEAT_2048(x) REPEAT_1024(x) REPEAT_1024(x)
#define REPEAT_4096(x) REPEAT_2048(x) REPEAT_2048(x)

// Indirect macro expansion helper
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define CONCAT_IMPL(a, b) a##b

// Dynamic repeat macro
#define REPEAT_N(n, x) CONCAT(REPEAT_, n)(x)

#define PAYLOAD_NOPS_16_BITS(n)   ".rept " #n "\n\tnop\n\t.endr\n"

#define STR2(x) #x
#define STR(x) STR2(x)

#endif // COMMON_H