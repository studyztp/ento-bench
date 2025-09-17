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

.global _sbrk_r
.type   _sbrk_r,%function
.thumb_func
_sbrk_r:
    push {r4,lr}
    mov  r4, r0
    movs r0, #12        /* ENOMEM */
    str  r0, [r4, #0]
    mvn  r0, # 0         /* (void*)-1 */
    pop  {r4,pc}
.size _sbrk_r, .-_sbrk_r

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
