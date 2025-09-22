/* wrapper for the syscalls to trigger gem5 SE */
.syntax unified
.thumb
.text

/* tiny macro for Linux syscalls on AArch32 because gem5 SE uses this to 
*  trigger syscall emulation 
*/
.macro SYSCALL num
    mov  r7, #\num
    svc  #0
.endm

.global _write_r
.type   _write_r,%function
.thumb_func
_write_r:
    push {r4,lr}
    mov  r4, r0
    mov  r0, r1
    mov  r1, r2
    mov  r2, r3
    SYSCALL 4           /* __NR_write_r */
    cmp  r0, #0
    bge  1f
    rsbs r0, r0, #0     /* r0 = errno */
    str  r0, [r4, #0]   /* r->_errno = errno (newlib: offset 0) */
    mvn  r0, #0         /* return -1 */
1:  pop  {r4,pc}
.size _write_r, .-_write_r

.global _read_r
.type   _read_r,%function
.thumb_func
_read_r:
    push {r4,lr}
    mov  r4, r0
    mov  r0, r1
    mov  r1, r2
    mov  r2, r3
    SYSCALL 3           /* __NR_read_r */
    cmp  r0, #0
    bge  1f
    rsbs r0, r0, #0
    str  r0, [r4, #0]
    mvn  r0, #0
1:  pop  {r4,pc}
.size   _read_r, .-_read_r

.global _close_r
.type   _close_r,%function
.thumb_func
_close_r:
    push {lr}
    mov  r0, r1
    SYSCALL 6           /* __NR_close_r */
    pop  {pc}
.size   _close_r, .-_close_r

.global _lseek_r
.type   _lseek_r,%function
.thumb_func
_lseek_r:
    push {lr}
    mov  r0, r1
    mov  r1, r2
    mov  r2, r3
    SYSCALL 19          /* __NR_lseek_r */
    pop  {pc}
.size   _lseek_r, .-_lseek_r

.global _isatty_r
.type   _isatty_r,%function
.thumb_func
_isatty_r:
    movs r0, #1
    bx   lr
.size   _isatty_r, .-_isatty_r

.global _fstat_r
.type   _fstat_r,%function
.thumb_func
_fstat_r:
    movs r0, #0
    bx   lr
.size _fstat_r, .-_fstat_r

.extern __sram_start__
.extern __sram_end__

.global _sbrk_r
.type   _sbrk_r,%function
.thumb_func
_sbrk_r:
    push {r4,r5,r6,lr}
    mov  r4, r0         /* save _reent pointer */
    mov  r5, r1         /* save increment */
    
    ldr  r6, =heap_ptr
    ldr  r0, [r6]       /* r0 = current heap_ptr */
    
    /* Initialize on first call to proper RAM address */
    cmp  r0, #0
    bne  1f
    ldr  r0, =__sram_start__    /* Force heap to start in main RAM */
    str  r0, [r6]
    
1:  /* Calculate new heap pointer */
    add  r1, r0, r5     /* new_ptr = current + increment */
    
    /* Check against RAM end (assuming 96KB RAM) */
    ldr  r2, =__sram_end__    /* End of 96KB RAM */
    cmp  r1, r2
    bhi  heap_full      /* if new_ptr > RAM_end, fail */
    
    /* Success: update heap pointer */
    str  r1, [r6]       /* heap_ptr = new_ptr */
    pop  {r4,r5,r6,pc}  /* return old heap_ptr in r0 */
    
heap_full:
    /* Failure: set errno and return -1 */
    movs r1, #12        /* ENOMEM */
    str  r1, [r4, #0]   /* errno = ENOMEM */
    mvn  r0, #0         /* return -1 */
    pop  {r4,r5,r6,pc}

.data
.align 4
heap_ptr:
    .word 0


.global _exit_r
.type   _exit_r,%function
.thumb_func
_exit_r:
    SYSCALL 1           /* __NR_exit_r */
1:  b 1b
.size _exit_r, .-_exit_r

.global _exit
.type   _exit,%function
.thumb_func
_exit:
    SYSCALL 1           /* __NR_exit */
1:  b 1b
.size _exit, .-_exit

.global _getpid_r
.type   _getpid_r,%function
.thumb_func
_getpid_r:
    mov  r7, #20         @ __NR_getpid_r (AArch32)
    svc  #0
    bx   lr
.size _getpid_r, .-_getpid_r

/* int _kill_r(int pid, int sig) */
.global _kill_r
.type   _kill_r,%function
.thumb_func
_kill_r:
    mov  r7, #37         @ __NR_kill_r (AArch32)
    svc  #0              @ returns 0 on success, -errno on failure
    bx   lr
.size _kill_r, .-_kill_r

.global _open_r
.type   _open_r,%function
.thumb_func
_open_r:
    push  {r4,lr}
    mov   r4, r0         /* save struct _reent* */
    mov   r0, r1         /* path */
    mov   r1, r2         /* flags */
    mov   r2, r3         /* mode */
    SYSCALL 5
    cmp   r0, #0
    bge   1f             /* success -> fd in r0 */
    rsbs  r0, r0, #0     /* r0 = -ret = errno */
    str   r0, [r4, #0]   /* r->_errno = errno (drop if you don't care) */
    mvn   r0, #0         /* return -1 */
1:  pop   {r4,pc}
.size _open_r, .-_open_r

.global raise
.type   raise,%function
.thumb_func
raise:
    push {r4,lr}
    mov  r4, r0          @ save sig
    mov  r7, #20         @ __NR_getpid
    svc  #0              @ r0 = pid
    mov  r1, r4          @ r1 = sig
    mov  r7, #37         @ __NR_kill
    svc  #0              @ r0 = 0 or -errno
    pop  {r4,pc}
.size raise, .-raise

.extern __sram_start__
.extern __sram_end__

.global _stack_init
.type _stack_init, %function
.thumb_func
_stack_init:
    ldr r3, =__sram_end__
    mov sp, r3
    ldr r10, =__sram_start__        // Your SRAM start address
    bx lr                       // Return
.size _stack_init, . - _stack_init
