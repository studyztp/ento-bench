include(${CMAKE_SOURCE_DIR}/entobench_helpers.cmake)
if(GEM5_BUILD)
    set(MODULES
        bench
        Eigen
        ento_util
    )
else()
    set(MODULES
        bench
        ento-mcu-semihosted
        Eigen
        ento_util
        CMSIS::STM32::${STM_PRODUCT}
    )
endif()

set(BENCH_TARGETS "" CACHE INTERNAL "List of benchmark targets") # At top level

function (add_ubench)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCE_FILE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(target_name ${ARG_TARGET_NAME})
    set(source_file ${ARG_SOURCE_FILE})

    list(APPEND source_file ${CMAKE_CURRENT_SOURCE_DIR}/../main.cc)

    if (NOT target_name)
        message(FATAL_ERROR "TARGET_NAME is required")
    endif()
    if (NOT source_file)
        message(FATAL_ERROR "SOURCE_FILE is required")
    endif()
    add_benchmark(${target_name} 
        SOURCES ${source_file}
        LIBRARIES ${MODULES}
    )
    if (STM32_BUILD)
        add_stm32_target(${target_name})
    endif()
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
    
    list(APPEND BENCH_TARGETS ${target_name})
    set(BENCH_TARGETS ${BENCH_TARGETS} CACHE INTERNAL "List of benchmark targets")
endfunction()

add_subdirectory(control)
add_subdirectory(execution)

message(WARNING "Microbenchmarks targets: ${BENCH_TARGETS}")

add_configured_benchmark_group_with_target_configs("ubench"
  TARGETS ${BENCH_TARGETS}
  CONFIG_FILE "${CMAKE_SOURCE_DIR}/benchmark/configs/ubench_benchmarks.json"
)
