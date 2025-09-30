#define ARM_SYSCALL_ARG0( sys_, res_, eflag_ )                          \
  register __typeof__ (res_)   res_   ## _ asm ("r0");                  \
  __asm__ volatile                                                      \
  ( "mov r7, %1 \n"                                                     \
    "svc #0"                                                            \
    : "=r"(res_ ## _)                                                   \
    : "i"(sys_)                                                          \
    : "r7", "memory" );                                                 \
  res_   = res_ ## _;                                                   \
  eflag_ = (res_ ## _ < 0) ? -res_ ## _ : 0;                            

#define ARM_SYSCALL_ARG1( sys_, res_, eflag_, a0_ )                     \
  register __typeof__ (res_)   res_   ## _ asm ("r0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("r0") = a0_;            \
  __asm__ volatile                                                      \
  ( "mov r7, %2 \n"                                                     \
    "svc #0"                                                            \
    : "=r"(res_ ## _)                                                   \
    : "r"(a0_ ## _), "i"(sys_)                                         \
    : "r7", "memory" );                                                 \
  res_   = res_ ## _;                                                   \
  eflag_ = (res_ ## _ < 0) ? -res_ ## _ : 0;                            

#define ARM_SYSCALL_ARG2( sys_, res_, eflag_, a0_, a1_ )                \
  register __typeof__ (res_)   res_   ## _ asm ("r0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("r0") = a0_;            \
  register __typeof__ (a1_)    a1_    ## _ asm ("r1") = a1_;            \
  __asm__ volatile                                                      \
  ( "mov r7, %3 \n"                                                     \
    "svc #0"                                                            \
    : "=r"(res_ ## _)                                                   \
    : "r"(a0_ ## _), "r"(a1_ ## _), "i"(sys_)                           \
    : "r7", "memory" );                                                 \
  res_   = res_ ## _;                                                   \
  eflag_ = (res_ ## _ < 0) ? -res_ ## _ : 0;                            

#define ARM_SYSCALL_ARG3( sys_, res_, eflag_, a0_, a1_, a2_ )           \
  register __typeof__ (res_)   res_   ## _ asm ("r0");                  \
  register __typeof__ (a0_)    a0_    ## _ asm ("r0") = a0_;            \
  register __typeof__ (a1_)    a1_    ## _ asm ("r1") = a1_;            \
  register __typeof__ (a2_)    a2_    ## _ asm ("r2") = a2_;            \
  __asm__ volatile                                                      \
  ( "mov r7, %4 \n"                                                     \
    "svc #0"                                                            \
    : "=r"(res_ ## _)                                                   \
    : "r"(a0_ ## _), "r"(a1_ ## _), "r"(a2_ ## _), "i"(sys_)           \
    : "r7", "memory" );                                                 \
  res_   = res_ ## _;                                                   \
  eflag_ = (res_ ## _ < 0) ? -res_ ## _ : 0;                            

// ARM Linux syscall numbers
#define ARM_SYSCALL_EXIT    1
#define ARM_SYSCALL_FORK    2
#define ARM_SYSCALL_READ    3
#define ARM_SYSCALL_WRITE   4
#define ARM_SYSCALL_OPEN    5
#define ARM_SYSCALL_CLOSE   6
#define ARM_SYSCALL_LINK   9
#define ARM_SYSCALL_UNLINK 10
#define ARM_SYSCALL_EXECVE 11
#define ARM_SYSCALL_CHDIR 12
#define ARM_SYSCALL_LSEEK 19
#define ARM_SYSCALL_GETPID 20
#define ARM_SYSCALL_KILL 37
#define ARM_SYSCALL_BRK 45
#define ARM_SYSCALL_STAT 106
#define ARM_SYSCALL_FSTAT 108

extern int main(void);

__attribute__((naked, noreturn)) void Reset_Handler(void) {
    // Zero BSS section
    extern char _sbss[], _ebss[];
    for (char* p = _sbss; p < _ebss; p++) {
        *p = 0;
    }
    
    // Copy initialized data from flash to RAM
    extern char _sdata[], _edata[], _sidata[];
    char* src = _sidata;
    for (char* dst = _sdata; dst < _edata; dst++, src++) {
        *dst = *src;
    }
    
    // Call main
    main();
    
    // Exit cleanly
    while(1) {
        asm volatile("wfi");  // Wait for interrupt
    }
}