#include "common.h"
extern "C" {
static volatile int always_true = 1;
#define ALWAYS_TRUE_BENCH if(always_true) { __asm__ __volatile__ ("" ::: "memory"); }

void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
}

__attribute__ ((noinline))
void ubench_asm(uint32_t *array, const size_t size) {
}
}
