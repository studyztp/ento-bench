// Example implementation using the ARM syscall wrapper macros
#include "syscall_wrapper.h"
#include <sys/types.h>
#include <errno.h>  

ssize_t _read(int fd, void *buf, size_t count) {
    ssize_t result;
    int error_flag;
    
    // Use the ARM_SYSCALL_ARG3 macro for read(fd, buf, count)
    ARM_SYSCALL_ARG3(ARM_SYSCALL_READ, result, error_flag, fd, buf, count);
    
    if (error_flag != 0) {
        // Handle error - you might want to set errno here
        return -error_flag;
    }
    
    return result;
}
ssize_t _write(int fd, const void *buf, size_t count) {
    ssize_t result;
    int error_flag;
    
    // Use the ARM_SYSCALL_ARG3 macro for write(fd, buf, count)
    ARM_SYSCALL_ARG3(ARM_SYSCALL_WRITE, result, error_flag, fd, buf, count);
    
    if (error_flag != 0) {
        // Handle error - you might want to set errno here
        return -error_flag;
    }
    
    return result;
}

int _open( const char* name, int flags, int mode )
{
  int result, error_flag;
  ARM_SYSCALL_ARG3(ARM_SYSCALL_OPEN, result, error_flag, name, flags, mode);

  if ( error_flag == 1 ) {
    errno = result;
    return -1;
  }
  
  return result;
}

int _lseek( int file, int ptr, int dir )
{
  int result, error_flag;
  ARM_SYSCALL_ARG3(ARM_SYSCALL_LSEEK, result, error_flag, file, ptr, dir );

  if ( error_flag == 1 ) {
    errno = result;
    return -1;
  }
  
    return result;
    }

int _fstat( int file, struct stat* st )
{
  int result, error_flag;
  ARM_SYSCALL_ARG2(ARM_SYSCALL_FSTAT, result, error_flag, file, st );

  if ( error_flag == 1 ) {
    errno = result;
    return -1;
  }

  return 0;
}

int _close( int file )
{
  int result, error_flag;
  ARM_SYSCALL_ARG1(ARM_SYSCALL_CLOSE, result, error_flag, file );

  if ( error_flag != 0 ) {
    errno = error_flag;
    return -1;
  }
  
  return 0;
}

int _stat( const char* file, struct stat* st )
{
  int result, error_flag;
  ARM_SYSCALL_ARG2(ARM_SYSCALL_STAT, result, error_flag, file, st );

  if ( error_flag == 1 ) {
    errno = result;
    return -1;
  }
  
  return 0;
}

int _link( char* old_name, char* new_name )
{
  int result, error_flag;
  ARM_SYSCALL_ARG2(ARM_SYSCALL_LINK, result, error_flag, old_name, new_name );

  if ( error_flag == 1 ) {
    errno = result;
    return -1;
  }
  
  return 0;
}

int _unlink( char* name )
{
  int result, error_flag;
  ARM_SYSCALL_ARG1(ARM_SYSCALL_UNLINK, result, error_flag, name );

  if ( error_flag != 0 ) {
    errno = error_flag;
    return -1;
  }
  
  return 0;
}

int _getpid() 
{
  return 1;
}

int _kill( int pid, int sig )
{
  errno = EINVAL;
  return -1;
}

caddr_t _sbrk( int incr )
{
  extern unsigned char _end; // Defined by linker
  int zero_arg;

  int result_initial;
  int error_flag_initial;
  unsigned char* end_initial;

  int result;
  int error_flag;
  unsigned char* end;

  // The heap end variables are unsigned char pointers but are stored as
  // integers. We use integers for convenience because the pkernel returns an
  // integer type result; using int types still retains correct functionality.

  int prev_heap_end;
  int heap_end;

  // We need to pass &_end to the pkernel so it knows where _end is. The
  // pkernel has its own separate _end symbol. Note that a real brk syscall
  // only takes a single argument -- the new break address -- so adding a
  // second argument isn't really 'correct'. But when running simulation that
  // does not use the pkernel, the extra argument will just be ignored. So
  // this remains functionally correct.

  end_initial = &_end;
  end         = &_end;

  zero_arg = 0;

  // An empty brk call returns the current program break value.

  ARM_SYSCALL_ARG2(ARM_SYSCALL_BRK, result_initial, error_flag_initial, zero_arg, end_initial);

  if ( error_flag_initial == 1 ) {
    errno = result_initial;
    return (caddr_t)-1;
  }

  // Save the previous program break value so we can return it later.

  prev_heap_end = result_initial;

  // Compute new program break value.

  heap_end = prev_heap_end + incr;

  // Do another brk syscall to set the new break value.

  ARM_SYSCALL_ARG2(ARM_SYSCALL_BRK, result, error_flag, heap_end, end );

  if ( error_flag == 1 ) {
    errno = result;
    return (caddr_t)-1;
  }

  // Return previous break value.

  return (caddr_t) prev_heap_end;
}

void _exit( int exit_status )
{
  int result, error_flag;
  ARM_SYSCALL_ARG1(ARM_SYSCALL_EXIT, result, error_flag, exit_status);

  // Convince gcc this function never returns to avoid warnings
  for (;;)
    ;
}
