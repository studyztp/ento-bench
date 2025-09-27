#include "common.h"

void init(uint32_t *array, const size_t size, const size_t stride) {
    for (size_t i = 0; i < size; i++) {
        if (i % stride == 0) {
            array[i] = 1;
        } else {
            array[i] = 0;
        }
    }
}

__attribute__ ((noinline))
void ubench_c(uint32_t *array, const size_t size) {
    for (size_t i = 0; i < size; i += 1) {
        if (array[i]) {
            array[i] = 0;
        }
    }
}

// Assembly version of ubench_c with -O0
// __attribute__ ((noinline))
// void ubench_c(uint32_t *array, const size_t size) {
//     81a0:       b480            push    {r7}
//     81a2:       b085            sub     sp, #20
//     81a4:       af00            add     r7, sp, #0
//     81a6:       6078            str     r0, [r7, #4]
//     81a8:       6039            str     r1, [r7, #0]
//     // what it should looks like in C
//     for (size_t i = 0; i < size; i += 1) {
//     81aa:       2300            movs    r3, #0
//     81ac:       60fb            str     r3, [r7, #12]
//     81ae:       e00f            b.n     81d0 <ubench_c+0x30>
//         if (array[i]) {
//     81b0:       68fb            ldr     r3, [r7, #12]
//     81b2:       009b            lsls    r3, r3, #2
//     81b4:       687a            ldr     r2, [r7, #4]
//     81b6:       4413            add     r3, r2
//     81b8:       681b            ldr     r3, [r3, #0]
//     81ba:       2b00            cmp     r3, #0
//     81bc:       d005            beq.n   81ca <ubench_c+0x2a>
//             array[i] = 0;
//     81be:       68fb            ldr     r3, [r7, #12]
//     81c0:       009b            lsls    r3, r3, #2
//     81c2:       687a            ldr     r2, [r7, #4]
//     81c4:       4413            add     r3, r2
//     81c6:       2200            movs    r2, #0
//     81c8:       601a            str     r2, [r3, #0]
//     for (size_t i = 0; i < size; i += 1) {
//     81ca:       68fb            ldr     r3, [r7, #12]
//     81cc:       3301            adds    r3, #1
//     81ce:       60fb            str     r3, [r7, #12]
//     81d0:       68fa            ldr     r2, [r7, #12]
//     81d2:       683b            ldr     r3, [r7, #0]
//     81d4:       429a            cmp     r2, r3
//     81d6:       d3eb            bcc.n   81b0 <ubench_c+0x10>
//         }
//     }
// }
//     81d8:       bf00            nop
//     81da:       bf00            nop
//     81dc:       3714            adds    r7, #20
//     81de:       46bd            mov     sp, r7
//     81e0:       f85d 7b04       ldr.w   r7, [sp], #4
//     81e4:       4770            bx      lr

// Assembly version of ubench_c with -O2
// __attribute__ ((noinline))
// void ubench_c(uint32_t *array, const size_t size) {
//     for (size_t i = 0; i < size; i += 1) {
//  80000b0:       b149            cbz     r1, 80000c6 <ubench_c+0x16>
//  80000b2:       3804            subs    r0, #4
//  80000b4:       eb00 0181       add.w   r1, r0, r1, lsl #2
//         if (array[i]) {
//             array[i] = 0;
//  80000b8:       2200            movs    r2, #0
//         if (array[i]) {
//  80000ba:       f850 3f04       ldr.w   r3, [r0, #4]!
//  80000be:       b103            cbz     r3, 80000c2 <ubench_c+0x12>
//             array[i] = 0;
//  80000c0:       6002            str     r2, [r0, #0]
//     for (size_t i = 0; i < size; i += 1) {
//  80000c2:       4288            cmp     r0, r1
//  80000c4:       d1f9            bne.n   80000ba <ubench_c+0xa>
//         }
//     }
// }
//  80000c6:       4770            bx      lr

__attribute__ ((noinline))
void ubench_asm(uint32_t *array, const size_t size) {
    // After using gem5 to verify the performance of using which version of 
    // ubench_c, we found that the -O2 version stress the branch predictor
    // more, so we use the -O2 version here because it is more realistic and 
    // allows us to observe more mispredictions to match the real behavior.
    __asm__ __volatile__ (
        "    .syntax unified\n"
        "    mov r0, %0\n"           // r0 = array
        "    mov r1, %1\n"           // r1 = size
        "    cbz     r1, 1f\n"          // if (size == 0) exit
        "    subs    r0, #4\n"          // Pre-adjust array pointer
        "    add.w   r2, r0, r1, lsl #2\n" // r2 = end_address
        "    movs    r3, #0\n"          // r3 = 0 (constant)
        "2:  ldr.w   r1, [r0, #4]!\n"   // Load with pre-increment
        "    cbz     r1, 3f\n"          // if (value == 0) skip store
        "    str     r3, [r0, #0]\n"    // Store 0
        "3:  cmp     r0, r2\n"          // Compare current vs end
        "    bne.n   2b\n"              // Loop if not equal
        "1:  \n"
        : "+r"(array)                   // array is input/output (modified)
        : "r"(size)                     // size is input only
        : "r1", "r2", "r3", "cc", "memory"
    );
}
