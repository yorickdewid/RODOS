/*
 * Created by Tiago Duarte
 * 09-04-2013 16:30
 * Reference: http://doc.ironwoodlabs.com/arm-arm-none-eabi/html/libc/Stubs.html#Stubs
 * Reference good: http://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html - 19-04-2013 @ 11:42
 */

#include <errno.h>
#include <sys/stat.h>
#include "rodos.h"
#include "workspace.h"

#undef errno
extern int errno;

//#ifndef __IEEE_LITTLE_ENDIAN
#define __IEEE_LITTLE_ENDIAN

void *__dso_handle;

extern "C" {

	int _write(int file, char *ptr, int len) {
		int todo;

		for (todo = 0; todo < len; todo++) {
			putchar(*ptr++);
		}
		return len;
	}

	void _abort(void) {
		/*
		 * Is recommended to implement something that indicates that
		 * this function was entered. Like flashing led, buzz, ...
		 */
		char str[25] = "ABORT ABORT\n";
		_write(1, str, sizeof(str));
		while(1);
	}

//	// Just appeared when I changed the compiler to gnuarm
//	void abort(void){
//		_abort();
//		while(1);
//	}

	/*
	 * In the minimal implementation, this function always fails,
	 * since there is only standard output, which is not a valid
	 * file to close.
	 */
	int _close(int file) {
		errno = EBADF;
		return -1;
	}

	/*
	 * Assume that all files are character special devices and
	 * populate the status data structure accordingly.
	 */
	int _fstat(int file, struct stat *st) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	/*
	 * This specifically checks whether a stream is a terminal.
	 */
	int _isatty(int file) {
		return 1;
	}

	int _getpid(void) {
		return 1;
	}

	/*
	 * Rather than failing, this function returns 0, indicating end-of-file.
	 */
	int _read(int file, char *ptr, int len) {
		return 0;
	}

	caddr_t _sbrk(int incr) {
		/*
		 * The memory map for this processor is:
		 * - the stack decreases from top_of_stacks with a range of 48K
		 * - the heap start after the top_of_stacks, represented by
		 * free_memory_start until the end of the RAM (_end or free_memory_end)
		 *
		 * In this scenario we cannot have heap and stack collisions!
		 * This function must verify if the end of memory was reached.
		 */
		static int *heap_ptr;
		int *actual_heap_ptr;
		extern int __heap_start;
		extern int __heap_end;

		// first time: initialize the heap_end pointer
		if (heap_ptr == 0) {
			heap_ptr = &__heap_start;
		}

		actual_heap_ptr = heap_ptr;
		if((heap_ptr + incr) < &__heap_end){
			heap_ptr += incr;
			return (caddr_t)actual_heap_ptr;
		}
		else{
			char str[25] = "End of heap space!\n";
			_write(1, str, sizeof(str));
			errno = ENOMEM;
			return (caddr_t)-1;
		}
		errno = ENOMEM;
		return NULL;
	}

	/*
	 * Exit a program without any cleanup.
	 */
	void _exit(void) {
		char str[8] = "_exit\n";
		_write(1, str, sizeof(str));
		while(1) {
			// Loop until reset
		}
	}

	/*
	 * A minimal implementation has no concept of either signals,
	 * nor of processes to receive those signals. So this function
	 * should always fail with an appropriate value in errno.
	 */
	int _kill(int pid, int sig) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * A minimal implementation has no file system, so this function
	 * can return 0, indicating that the only stream (standard output)
	 * is positioned at the start of file.
	 */
	int _lseek(int file, int ptr, int dir) {
		return 0;
	}
}/* END EXTERN C */
