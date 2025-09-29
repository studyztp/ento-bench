set(LIST_OF_UBENCH "")
include(${CMAKE_SOURCE_DIR}/entobench_helpers.cmake)
message(WARNING "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
if(GEM5_BUILD)
    
    set(MODULES
        bench
        Eigen
        ento_util
    )

    function (add_ubench)
        set(options)
        set(oneValueArgs TARGET_NAME)
        set(multiValueArgs SOURCE_FILE)
        cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        set(target_name ${ARG_TARGET_NAME})
        set(source_file ${ARG_SOURCE_FILE})

        if (NOT target_name)
            message(FATAL_ERROR "TARGET_NAME is required")
        endif()
        if (NOT source_file)
            message(FATAL_ERROR "SOURCE_FILE is required")
        endif()

        list(APPEND source_file ${CMAKE_CURRENT_SOURCE_DIR}/../main.cc)

        add_benchmark(${target_name} 
            SOURCES ${source_file}
            LIBRARIES ${MODULES}
        )

        target_link_libraries(${target_name} PRIVATE ${MODULES})
        target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
        stm32_generate_linker_script(
            TARGET ${target_name}
            FLASH_ORIGIN 0x08000000
            FLASH_SIZE 0x00080000  # 512KB
            RAM_ORIGIN 0x20000000
            RAM_SIZE 0x00018000    # 96KB
            LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.ld
            HEAP_SIZE 0x200
            STACK_SIZE 0x400
        )
        list(APPEND LIST_OF_UBENCH ${target_name})
        set(LIST_OF_UBENCH ${LIST_OF_UBENCH} PARENT_SCOPE)
    endfunction()

    function(stm32_generate_linker_script)
        set(options)
        set(oneValueArgs 
            TARGET
            FLASH_ORIGIN
            FLASH_SIZE
            RAM_ORIGIN
            RAM_SIZE
            LINKER_SCRIPT
            HEAP_SIZE
            STACK_SIZE
        )
        set(multiValueArgs)
        cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        set(TARGET ${ARG_TARGET})
        if (NOT TARGET)
            message(FATAL_ERROR "TARGET is required")
        endif()
        set(FLASH_ORIGIN ${ARG_FLASH_ORIGIN})
        if (NOT FLASH_ORIGIN)
            message(FATAL_ERROR "FLASH_ORIGIN is required")
        endif()
        set(FLASH_SIZE ${ARG_FLASH_SIZE})
        if (NOT FLASH_SIZE)
            message(FATAL_ERROR "FLASH_SIZE is required")
        endif()
        set(RAM_ORIGIN ${ARG_RAM_ORIGIN})
        if (NOT RAM_ORIGIN)
            message(FATAL_ERROR "RAM_ORIGIN is required")
        endif()
        set(RAM_SIZE ${ARG_RAM_SIZE})
        if (NOT RAM_SIZE)
            message(FATAL_ERROR "RAM_SIZE is required")
        endif()
        set(LINKER_SCRIPT ${ARG_LINKER_SCRIPT})
        if (NOT LINKER_SCRIPT)
            message(FATAL_ERROR "LINKER_SCRIPT is required")
        endif()
        set(HEAP_SIZE ${ARG_HEAP_SIZE})
        if (NOT HEAP_SIZE)
            message(FATAL_ERROR "HEAP_SIZE is required")
        endif()
        set(STACK_SIZE ${ARG_STACK_SIZE})
        if (NOT STACK_SIZE)
            message(FATAL_ERROR "STACK_SIZE is required")
        endif()
        
        # Execute the linker script generation
        include(${CMAKE_CURRENT_LIST_DIR}/../linker_ld.cmake)
        
        # Add to target
        target_link_options(${TARGET} PRIVATE -T${LINKER_SCRIPT})
    endfunction()
else()
    set(MODULES
        bench
        ento-mcu-semihosted
        Eigen
        ento_util
        CMSIS::STM32::${STM_PRODUCT}
    )
    function (add_ubench)
        set(options)
        set(oneValueArgs TARGET_NAME)
        set(multiValueArgs SOURCE_FILE)
        cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        set(target_name ${ARG_TARGET_NAME})
        set(source_file ${ARG_SOURCE_FILE})

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
        list(APPEND LIST_OF_UBENCH ${target_name})
        set(LIST_OF_UBENCH ${LIST_OF_UBENCH} PARENT_SCOPE)
    endfunction()

endif()

add_subdirectory(control)
