/*
 * syscalls.cpp
 *
 *  Created on: 13.03.2012
 *      Author: Michael Ruffer, modified Johannes Freitag
 */


#undef errno
extern int errno;

#include <hal/hal_uart.h>
#include "params.h"
#include "bcm2835.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern HAL_UART uart_stdout;

#define T2H_SYSCALL_INIT_ARGV           14

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
int _close(int file) {
	return -1;
}

char *__env[1] = { 0 };
char **environ = __env;

int _execve(char *name, char **argv, char **env) {
	//errno = ENOMEM;
	return -1;
}

int _fork(void) {
	//errno = EAGAIN;
	return -1;
}

int _fstat(int file, struct stat *st) {
	//st->st_mode = S_IFCHR;
	return 0;
}

int _getpid(void) {
	return 1;
}

int _isatty(int file) {
	return 1;
}

int _kill(int pid, int sig) {
	//errno = EINVAL;
	return -1;
}

int _link(char *old, char *pNew) {
	//errno = EMLINK;
	return -1;
}

int _lseek(int file, int ptr, int dir) {
	return 0;
}

int _open(const char *name, int flags, int mode) {
	return -1;
}

int _read(int file, char *ptr, int len) {
	return 0;
}

register char * stack_ptr asm ("sp");
extern char _estack;
char* _sbrk(int incr) {
	extern char __heap_start__ asm ("__heap_start__"); /* Defined by the linker.  */
	static char * heap_end;
	char * prev_heap_end;

	if (heap_end == 0)
		heap_end = &__heap_start__;

	prev_heap_end = heap_end;

	// Test, ob stack und heap kollidieren funktioniert in urspruenglicher
	// implementierung nicht, weil
	// RODOS den Stack verwaltet und der Stackpointer adressmaessig
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
		asm("bkpt;");
		//errno = ENOMEM;
		return  (char*) -1;
#endif
	}

	heap_end += incr;

	return  prev_heap_end;
}

/*
 int stat(char *file, struct stat *st) {
 st->st_mode = S_IFCHR;
 return 0;
 }
 */
int _times(struct tms *buf) {
	return -1;
}

int _unlink(char *name) {
	//errno = ENOENT;
	return -1;
}

int _wait(int *status) {
	//errno = ECHILD;
	return -1;
}

int _write(int file, char *ptr, int len) {
	return 1;
}

/*
 * reentrant syscalls
 */
long _write_r(void *reent, int fd, const void *buf, int cnt) {
	return _write(fd, (char*) buf, cnt);
}

char* _sbrk_r(void *reent, int incr) {
	return _sbrk(incr);
}
#endif

/* end newlib stubs */



int putchar(char c) {
	uart_stdout.putcharNoWait(c);
	return c;
}

int puts(const char * str) {
	const char *c;
	c = str;
	while (*c) {
		putchar(*c++);
	}
	return 0;
}

void abort(void) { while(1); }

} // end extern "C"

void sp_partition_yield() {}

void FFLUSH() { }



#ifndef NO_RODOS_NAMESPACE
}
#endif

