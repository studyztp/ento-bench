set(NUM_OF_REPATING_NOPS "1" "2" "4" "8" "16")

foreach(NOPS ${NUM_OF_REPATING_NOPS})
    add_ubench(TARGET_NAME nops_${NOPS} SOURCE_FILE nops.cc DEFINITIONS ARRAY_SIZE=${NOPS})
endforeach()

set(ARITHMETIC_OPS "add" "sub")
foreach(OP ${ARITHMETIC_OPS})
    add_ubench(TARGET_NAME arithmetic_${OP} SOURCE_FILE arithmetic.cc DEFINITIONS OP=${OP})
endforeach()
