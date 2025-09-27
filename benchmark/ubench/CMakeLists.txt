cmake_minimum_required(VERSION 3.16)

project(testing CXX C ASM)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(ARM_GCC arm-none-eabi-gcc)
find_program(ARM_GXX arm-none-eabi-g++)

set(CMAKE_CXX_COMPILER ${ARM_GXX})
set(CMAKE_C_COMPILER ${ARM_GCC})

# Platform specific configurations can also be set via cache variables or environment
set(CORE "cortex-m4" CACHE STRING "Set the processor core type")
set(FPU "fpv4-sp-d16" CACHE STRING "Set the FPU type")
set(FLOAT_ABI "hard" CACHE STRING "Set the float ABI type")
set(MARCH "armv7e-m" CACHE STRING "Set the architecture")

set(CMAKE_C_FLAGS "-O2 -g -mcpu=${CORE} -mfpu=${FPU} -mfloat-abi=${FLOAT_ABI} -mthumb -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "-O2 -g -mcpu=${CORE} -mfpu=${FPU} -mfloat-abi=${FLOAT_ABI} -mthumb -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "-static -mcpu=${CORE} -mfpu=${FPU} -mfloat-abi=${FLOAT_ABI} -mthumb -Wl,--gc-sections --specs=nosys.specs ")
set(LIST_OF_UBENCH "")

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

    add_executable(${target_name} ${source_file} ${CMAKE_CURRENT_SOURCE_DIR}/../main.c)
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

add_subdirectory(control)

if(USE_ASM)
    message(WARNING "Using assembly implementations")
    foreach(ubench ${LIST_OF_UBENCH})
        target_compile_definitions(${ubench} PRIVATE USE_ASM)
    endforeach()
endif()
