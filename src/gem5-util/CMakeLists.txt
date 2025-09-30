cmake_minimum_required(VERSION 3.16)

set(SRC_FILES
syscall_wrapper.c
)

add_library(gem5_syscall_wrapper STATIC
${SRC_FILES}
)

