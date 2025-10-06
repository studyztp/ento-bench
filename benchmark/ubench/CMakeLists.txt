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
    set(multiValueArgs SOURCE_FILE DEFINITIONS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(target_name ${ARG_TARGET_NAME})
    set(source_file ${ARG_SOURCE_FILE})
    set(definitions ${ARG_DEFINITIONS})

    list(APPEND source_file ${CMAKE_CURRENT_SOURCE_DIR}/../main.cc)

    if (NOT target_name)
        message(FATAL_ERROR "TARGET_NAME is required")
    endif()
    if (NOT source_file)
        message(FATAL_ERROR "SOURCE_FILE is required")
    endif()

    if (STM32_BUILD)
        foreach(sys_setup "normal" "disabled_icache" "disabled_prefetch" "disabled_both_icache_prefetch")
            if (sys_setup STREQUAL "normal")
                set(final_target_name "${target_name}-normal")
            elseif (sys_setup STREQUAL "disabled_icache")
                set(final_target_name "${target_name}-no-icache")
                set(final_definitions ${definitions} "DISABLE_I_CACHE")
            elseif (sys_setup STREQUAL "disabled_prefetch")
                set(final_target_name "${target_name}-no-prefetch")
                set(final_definitions ${definitions} "DISABLE_PREFETCH")
            elseif (sys_setup STREQUAL "disabled_both_icache_prefetch")
                set(final_target_name "${target_name}-no-icache-no-prefetch")
                set(final_definitions ${definitions} "DISABLE_I_CACHE" "DISABLE_PREFETCH")
            endif()
            add_benchmark(${final_target_name} 
                SOURCES ${source_file}
                LIBRARIES ${MODULES}
            )
            add_stm32_target(${final_target_name})
            target_include_directories(${final_target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
            target_compile_definitions(${final_target_name} PRIVATE ${final_definitions})

            message(WARNING "Added benchmark target: ${final_target_name} with definitions: ${final_definitions}")

            list(APPEND BENCH_TARGETS ${final_target_name})
            set(BENCH_TARGETS ${BENCH_TARGETS} CACHE INTERNAL "List of benchmark targets")
        endforeach()
    elseif (GEM5_BUILD)
        add_benchmark(${target_name} 
            SOURCES ${source_file}
            LIBRARIES ${MODULES}
        )
        target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
        target_compile_definitions(${target_name} PRIVATE ${definitions})
        list(APPEND BENCH_TARGETS ${target_name})
        set(BENCH_TARGETS ${BENCH_TARGETS} CACHE INTERNAL "List of benchmark targets")
    endif()
endfunction()

add_subdirectory(control)
add_subdirectory(execution)

add_configured_benchmark_group_with_target_configs("ubench"
  TARGETS ${BENCH_TARGETS}
  CONFIG_FILE "${CMAKE_SOURCE_DIR}/benchmark/configs/ubench_benchmarks.json"
)
