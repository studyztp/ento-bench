#include "common.h"

static volatile int always_false = 0;
#define ALWAYS_FALSE_BENCH if(always_false) { __asm__ __volatile__ ("" ::: "memory"); }

void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench_c(uint32_t *array, const size_t size) {
    REPEAT_N(ARRAY_SIZE, ALWAYS_FALSE_BENCH);
}

__attribute__ ((noinline))
void ubench_asm(uint32_t *array, const size_t size) {
    ubench_c(array, size);
}
