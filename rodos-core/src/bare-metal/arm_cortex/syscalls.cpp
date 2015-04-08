#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include "hw/hw_types.h"
#include "hw/hw_memmap.h"
#include "hw/hw_uart.h"
#include "src/uart.h"
#include "src/debug.h"
#include "src/sysctl.h"
#include "src/gpio.h"
#include "utils/uartstdio.h"

#undef errno
extern int errno;
extern "C" {
	
extern unsigned long _heap;
extern unsigned long _eheap;

static caddr_t heap = NULL;

caddr_t _sbrk(int incr)
{
	   caddr_t prevHeap;
	   caddr_t nextHeap;

	   if(heap == NULL) heap = (caddr_t) &_heap;

	   prevHeap = heap;
	   nextHeap = prevHeap + incr;


	if(nextHeap >= (caddr_t) &_eheap) return (caddr_t) -1; // error - no more memory

	   else {
	       heap = nextHeap;
	       return prevHeap;
	   }
}

int _isatty(int fd)
{
  return 1;
}

int _lseek(int fd, off_t pos, int whence)
{
  return 0;
}

int _close(int file) {
return -1;
}

int _read(int fd, void *buffer, unsigned int count)
{
return(0);
}

int _fstat(int file, struct stat *st) {
st->st_mode = S_IFCHR;
return 0;
}

int open(const char *name, int flags, int mode) {
return -1;
}

int
_write(int file, char *ptr, int len) {
   int todo;
   for (todo = 0; todo < len; todo++) {
	  if(*ptr=='\n')
	  {
		  UARTCharPut(UART0_BASE, '\r');
	  }
      UARTCharPut(UART0_BASE, (*ptr++));
   }
   return len;
}

void _exit (int  rc) {
	while (1) { }
}   /* _exit () */

} // Extern "C"

// void operator delete(void* p) { }
extern "C" void abort(void) { while(1); }
//extern "C" int _write(int file, char *ptr, int len);
//extern "C" caddr_t _sbrk(int incr);

extern "C" {
//  void* __dso_handle = (void*) &__dso_handle;

  int __cxa_atexit(void (*func) (void*), void* arg, void* dso_handle) {
    return 0;
  }

  void __cxa_pure_virtual(void) {
  }

//  int __aeabi_unwind_cpp_pr0(int, void* , void*) { while(1) {} return 0;}


  int __aeabi_atexit(void (*func) (void*), void* arg, void* dso_handle) {
    return 0;
  }
/*	//copies memory
	void* memcpy(void* dest, const void* src, int n) {
	    char* d = (char*) dest;
	    char* s = (char*) src;
	    for ( int i=0; i<n; ++i) {
	      *d++ = *s++;
	    }

  return dest;
	}
*/
	//copies strings
	int strcmpXXXX( const char *str1, const char *str2 ) {
	    const char* s1 = str1;
	    const char* s2 = str2;
		while((*s1!=0)&&(*s2!=0)){
			if (*s1++!=*s2++){
				return -1;
			}
		}
    return 0;
  }
}
//puts chars
extern "C" int putchar ( int ic )
{
	char c = (char)(ic & 0xff);
	
	if(c == '\n')
	{
		UARTCharPut(UART0_BASE, '\r');
	}
    UARTCharPut(UART0_BASE, c);
	return c;	
}

//puts strings
extern "C" int puts ( const char * str )
{
	const char *c;
	c=str;
	while (*c){
		putchar(*c++);
	}
	return 0;	
}
extern "C" int sched_yield();
void sp_partition_yield() {}

void FFLUSH() { }
