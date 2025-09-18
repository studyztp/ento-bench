if (GEM5_BUILD)
    message(WARNING "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

    # Inputs
    # Expect: SRC_FILE (basename, no ext), SRC_FILE_PATH (path to .S)
    set(SRC_FILE ${GEM5_SYSCALL_WRAPPER_FILENAME})
    set(SRC_FILE_PATH ${GEM5_UTIL}/${SRC_FILE}.S)
    set(PP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE}.pp.S)
    set(OBJ_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE}.o)

    # 1) Preprocess assembly -> .pp.S
    add_custom_command(
        OUTPUT  ${PP_FILE}
        COMMAND arm-none-eabi-cpp
                -mfloat-abi=${FLOAT_ABI} -mfpu=${FPU} -mcpu=${CORE}
                ${SRC_FILE_PATH} -o ${PP_FILE}
        DEPENDS ${SRC_FILE_PATH}
        COMMENT "Preprocessing ${SRC_FILE}"
        VERBATIM
    )

    # 2) Assemble -> .o
    add_custom_command(
        OUTPUT  ${OBJ_FILE}
        COMMAND arm-none-eabi-as
                -mcpu=${CORE} -mthumb -mfpu=${FPU} -mfloat-abi=${FLOAT_ABI}
                -o ${OBJ_FILE} ${PP_FILE}
        DEPENDS ${PP_FILE}
        COMMENT "Assembling ${SRC_FILE} -> ${OBJ_FILE}"
        VERBATIM
    )

    # Tell CMake this is an external, generated object file
    set_source_files_properties(${OBJ_FILE} PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE
    )

    # 3) Archive into libgem5_syscall_wrapper.a
    add_library(gem5_syscall_wrapper STATIC ${OBJ_FILE})

    add_custom_target(gem5_syscall_wrapper_preprocessed DEPENDS ${PP_FILE})
    add_custom_target(gem5_syscall_wrapper_object DEPENDS ${OBJ_FILE})
    set_target_properties(gem5_syscall_wrapper PROPERTIES LINKER_LANGUAGE C)
endif()
