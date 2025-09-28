#include "common.h"

// forward declarations
void init(uint32_t *array, const size_t size, const size_t stride);
void ubench(uint32_t *array, const size_t size);

int main() {
    uint32_t array[ARRAY_SIZE];
    size_t size = ARRAY_SIZE;
    size_t stride = STRIDE;
    
    // Warmup runs
    for (int i = 0; i < NUM_WARMUP; i++) {
        init(array, size, stride);
        ubench(array, size);
    }
    
    init(array, size, stride);
    m5_work_begin();
    ubench(array, size);
    m5_work_end();

    m5_exit();
}

void Reset_Handler(void) {
    // System initialization
    main();
    while(1);  // Prevent return
}
