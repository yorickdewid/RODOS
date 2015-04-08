/*
 * init_argv.cpp
 *
 *  Created on: 13.03.2012
 *      Author: Michael Ruffer
 */

#define T2H_SYSCALL_INIT_ARGV           14

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>


//extern "C" {
#include "FreeRTOS.h"
#include "nlao_usart.h"
//#include "FreeRTOSConfig.h"
//#include "usart.h"
#undef putchar

int putchar (int c) {
    usart_putc(c&0xff);
    //usart_write_char(configDBG_USART,c&0xff);
    return 0;
}
 
/*
 * - original _init_argv from newlib (newlib/libc/sys/avr32/syscalls.c) stops the program
 *   during debug due to breakpoint in T2H_SYSCALL(syscall_code)
 * - to avoid this behavior we have to redefine _init_argv
 *   -> this is possible because it is defined as "weak" in newlib
 *

unchanged sourec code from  newlib/libc/sys/avr32/syscalls.c:

 * The breakpoint/mov r12,-1/mov r11,... pattern causes
 * the debugger to be able to recognize the system call.
 * If the debug system is not enabled then the breakpoint
 * is executed as a nop and r12 set to -1 and errno to
 * ENOSYS. The debugger is responsible for setting r12
 * and r11 and skipping the instructions after the breakpoint
 * which would otherwise set these registers.

#define T2H_SYSCALL(syscall_code)                 \
  asm ("mov   r8, %0\n"                           \
       "breakpoint\n"                             \
       "mov   r12, -1\n"                          \
       "mov   r11, %1\n"                          \
       :: "i" (syscall_code), "i" (ENOSYS) );

#define T2H_SYSCALL_RETURN()                                       \
  asm ("ret  r12\n");

void __attribute__((weak,naked))
     _init_argv (void *arg_mem_top)
{
  T2H_SYSCALL(T2H_SYSCALL_INIT_ARGV);
  T2H_SYSCALL_RETURN();
}
*/
#define T2H_SYSCALL(syscall_code)                 \
  asm ("mov   r8, %0\n"                           \
       "nop\n"                             \
       "mov   r12, -1\n"                          \
       "mov   r11, %1\n"                          \
       :: "i" (syscall_code), "i" (ENOSYS) );

#define T2H_SYSCALL_RETURN()                                       \
  asm ("ret  r12\n");

void __attribute__((naked))
     _init_argv (void *arg_mem_top)
{
  T2H_SYSCALL(T2H_SYSCALL_INIT_ARGV);
  T2H_SYSCALL_RETURN();
}

//} // extern "C"