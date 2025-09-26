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

// Assembly version of ubench_c
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

__attribute__ ((noinline))
void ubench_asm(uint32_t *array, const size_t size) {
    // Assembly version of ubench_c based on -O0 output of the ubench_c 
    // function
    __asm__ __volatile__ (
        "push {r7}                \n"
        "sub sp, #20              \n"
        "add r7, sp, #0           \n"
        "str r0, [r7, #4]         \n"
        "str r1, [r7, #0]         \n"
        "movs r3, #0              \n"
        "str r3, [r7, #12]        \n"
        "b.n 1f                   \n"
        "2:                       \n"
        "ldr r3, [r7, #12]        \n"
        "lsls r3, r3, #2          \n"
        "ldr r2, [r7, #4]         \n"
        "add r3, r2               \n"
        "ldr r3, [r3, #0]         \n"
        "cmp r3, #0               \n"
        "beq.n 3f                 \n"
        "ldr r3, [r7, #12]        \n"
        "lsls r3, r3, #2          \n"
        "ldr r2, [r7, #4]         \n"
        "add r3, r2               \n"
        "movs r2, #0              \n"
        "str r2, [r3, #0]         \n"
        "3:                       \n"
        "ldr r3, [r7, #12]        \n"
        "adds r3, #1              \n"
        "str r3, [r7, #12]        \n"
        "1:                       \n"
        "ldr r2, [r7, #12]        \n"
        "ldr r3, [r7, #0]         \n"
        "cmp r2, r3               \n"
        "bcc.n 2b                 \n"
        "nop                      \n"
        "nop                      \n"
        "adds r7, #20             \n"
        "mov sp, r7               \n"
        "ldr.w r7, [sp], #4       \n"
        "bx lr                    \n"
    );
}
