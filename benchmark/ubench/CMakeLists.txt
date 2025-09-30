set(LIST_OF_UBENCH "")
include(${CMAKE_SOURCE_DIR}/entobench_helpers.cmake)
message(WARNING "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
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
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
    list(APPEND LIST_OF_UBENCH ${target_name})
    set(LIST_OF_UBENCH ${LIST_OF_UBENCH} PARENT_SCOPE)
endfunction()

add_subdirectory(control)
