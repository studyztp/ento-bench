#include "common.h"

extern "C" {

#ifndef OP
#define OP add
#endif // OP

__attribute__ ((noinline))
void JUST_AN_OP () {
    asm volatile(                     
        ".thumb                 \n" 
        ".p2align 4             \n"  
        ".rept 8                \n"   
        STR(OP) "s r0, #1       \n"   
        STR(OP) "s r1, #1       \n"   
        STR(OP) "s r2, #1       \n"   
        STR(OP) "s r3, #1       \n"   
        ".endr                  \n"   
        :                             
        :                             
        : "r0", "memory");
}

__attribute__ ((noinline))
void init(uint32_t *array, const size_t size, const size_t stride) {
    // do nothing
}

__attribute__ ((noinline))
void ubench(uint32_t *array, const size_t size) {
    asm volatile(".p2align 4");
    REPEAT_1024(JUST_AN_OP();)
}
}
