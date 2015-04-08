/*
 * syscalls.cpp
 *
 *  Created on: 13.03.2012
 *      Author: Michael Ruffer
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#undef errno
extern int errno;

#include "hal/hal_uart.h"
#include "params.h"
#include "hw_specific.h"
namespace RODOS {
#define T2H_SYSCALL_INIT_ARGV           14

extern HAL_UART UART_DEBUG;

extern "C" {

/* newlib stubs
 * The following function are necessary for newlib. It is very important
 * that there are valid implementations for _sbrk_r and sbrk. These
 * functions are used for memory allocation.
 * For more informations about syscalls visit:
 * http://sourceware.org/newlib/libc.html#Syscalls
 * http://sourceware.org/newlib/libc.html#Reentrant-Syscalls
 *
 */
#if 1
int close(int file) {
	return -1;
}

char *__env[1] = { 0 };
char **environ = __env;

int execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

int fork(void) {
	errno = EAGAIN;
	return -1;
}

int fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int getpid(void) {
	return 1;
}

int isatty(int file) {
	return 1;
}

int kill(int pid, int sig) {
	errno = EINVAL;
	return -1;
}

int link(char *old, char *pNew) {
	errno = EMLINK;
	return -1;
}

int lseek(int file, int ptr, int dir) {
	return 0;
}

int open(const char *name, int flags, int mode) {
	return -1;
}

int read(int file, char *ptr, int len) {
	return 0;
}

register char * stack_ptr asm ("sp");
extern char _estack;
caddr_t sbrk(int incr) {
	extern char __heap_start__ asm ("__heap_start__"); /* Defined by the linker.  */
	static char * heap_end;
	char * prev_heap_end;

	if (heap_end == NULL)
		heap_end = &__heap_start__;

	prev_heap_end = heap_end;

	// Test, ob stack und heap kollidieren funktioniert in urspruenglicher
	// implementierung nicht, weil
	// RODOS den Stack verwaltet und der Stackpointer adressmae�ig
	// irgendwo im XMALLOC-Puffer liegt. Der Puffer liegt z.Z. an einer
	// Adresse die immer kleiner als heap_end ist -> vgl. Linkerscript!!!
	// Da der eigentliche Systemstack nachdem RODOS die Kontrolle des Stack
	// uebernommen hat nicht mehr benoetigt wird, darf der Heap bis ans Ende
	// des RAM wachsen (theoretisch :) ...).
	//if (heap_end + incr > stack_ptr) {
	if (heap_end + incr > &_estack) {
		/* Some of the libstdc++-v3 tests rely upon detecting
		 out of memory errors, so do not abort here.  */
#if 0
		extern void abort (void);

		_write (1, "_sbrk: Heap and stack collision\n", 32);

		abort ();
#else
		asm("breakpoint;");
		errno = ENOMEM;
		return (caddr_t) -1;
#endif
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

/*
 int stat(char *file, struct stat *st) {
 st->st_mode = S_IFCHR;
 return 0;
 }
 */
int times(struct tms *buf) {
	return -1;
}

int unlink(char *name) {
	errno = ENOENT;
	return -1;
}

int wait(int *status) {
	errno = ECHILD;
	return -1;
}

int write(int file, char *ptr, int len) {
	int todo;

	file = file;
	ptr = ptr;

	for (todo = 0; todo < len; todo++) {
		if (*ptr == '\n') {
			while (UART_DEBUG.isWriteFinished() == false) {
			}
			UART_DEBUG.putcharNoWait('\r');
		}
		while (UART_DEBUG.isWriteFinished() == false) {
		}
		UART_DEBUG.putcharNoWait(*ptr);
		ptr++;
	}
	return len;
}

/*
 * reentrant syscalls
 */
long _write_r(void *reent, int fd, const void *buf, size_t cnt) {
	return write(fd, (char*) buf, cnt);
}

caddr_t _sbrk_r(void *reent, size_t incr) {
	return sbrk(incr);
}
#endif
/* end newlib stubs */

//puts chars
int putchar(int ic) {
	char c = (char) (ic & 0xff);

	if (c == '\n') {
		while (UART_DEBUG.isWriteFinished() == false) {
		}
		UART_DEBUG.putcharNoWait('\r');
	}
	while (UART_DEBUG.isWriteFinished() == false) {
	}
	UART_DEBUG.putcharNoWait(c);
	return c;
}

//puts strings
int puts(const char * str) {
	const char *c;
	c = str;
	while (*c) {
		putchar(*c++);
	}
	return 0;
}

/*
 * - original _init_argv from newlib (newlib/libc/sys/avr32/syscalls.c) stops the program
 *   during debug (with TRACE32) due to a breakpoint in T2H_SYSCALL(syscall_code)
 * - to avoid this behavior we have to redefine _init_argv
 *   -> this is possible because it is defined as "weak" in newlib
 *

 unchanged source code from  newlib/libc/sys/avr32/syscalls.c:

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

//void _init_argv (void *arg_mem_top)
// - it's necessary to define init_argv without parameter "arg_mem_top" , because
//   the compiler generates (when called with -O0) the command "st.w r7[-4],r12" to store
//   the parameter on stack
//   -> in non-naked functions there is the command "mov r7,sp" before to copy the stack pointer
//      to r7, but in naked functions this command is missing and r7 is uninitialized and leads to a
//      memory access error
// - the compiler should not generate the command "st.w r7[-4],r12", because the function is declared naked,
//   but it does -> maybe a bug
void __attribute__((naked))
_init_argv() {
	T2H_SYSCALL(T2H_SYSCALL_INIT_ARGV);
	T2H_SYSCALL_RETURN();
}
} // extern "C"
}
