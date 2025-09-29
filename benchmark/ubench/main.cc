#include "common.h"

extern "C" {
void ubench(uint32_t *array, const size_t size);
}

int main()
{

#ifdef NATIVE
    initialise_monitor_handles();

    // Configure max clock rate and set flash latency
    sys_clk_cfg();
    SysTick_Setup();
    __enable_irq();


    // NEW IDIOM: Generic cache setup using configuration
    ENTO_BENCH_SETUP();
#endif // NATIVE

    ENTO_BENCH_PRINT_CONFIG();

    ENTO_INFO("Starting ubench benchmarks...");

    // Create a wrapper lambda that matches the expected signature
    auto ubench_wrapper = []() {
        uint32_t array[ARRAY_SIZE];
        ubench(array, ARRAY_SIZE);
    };
    
    auto ubench_problem = make_basic_problem(ubench_wrapper);
    ENTO_BENCH_HARNESS_TYPE(decltype(ubench_problem));
    BenchHarness ubench_harness(ubench_problem, "Microbenchmark");

    ubench_harness.run();

   ENTO_INFO("All benchmarks completed.");

    exit(1);
    return 0;
}