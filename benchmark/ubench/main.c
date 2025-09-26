#include "common.h"

void init(uint32_t *array, const size_t size, const size_t stride);
void ubench_c(uint32_t *array, const size_t size);
void ubench_asm(uint32_t *array, const size_t size);

int main() {
    uint32_t array[ARRAY_SIZE];
    size_t size = ARRAY_SIZE;
    size_t stride = STRIDE;
    init(&array[0], size, stride);
    m5_work_begin();
    ubench_c(&array[0], size);
    m5_work_end();
    m5_work_begin();
    ubench_asm(&array[0], size);
    m5_work_end();
    m5_exit();
}

void Reset_Handler(void) {
    // System initialization
    main();
    while(1);  // Prevent return
}
