add_ubench(TARGET_NAME nops SOURCE_FILE nops.cc)


# set(ARITHMETIC_OPS add sub mul mla)
# foreach(OP ${ARITHMETIC_OPS})
#     add_ubench(TARGET_NAME arithmetic_${OP} SOURCE_FILE arithmetic.cc DEFINITIONS OP=${OP})
# endforeach()
