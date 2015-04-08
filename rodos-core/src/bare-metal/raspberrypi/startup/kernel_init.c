#include "stdint.h"

typedef void (*constructor_t)(void);
extern constructor_t _init_array_start[];
extern constructor_t _init_array_end[];
extern uint32_t bss_start;
extern uint32_t bss_end;

extern void PUT32(uint32_t, uint32_t);

void kernel_init(void) {

	//initialize .bss section with zeros
	uint32_t addr;
	for (addr = bss_start; addr < bss_end; addr += 4)
		PUT32(addr, 0);

	//call all static constructors
	constructor_t *fn = _init_array_start;
	while (fn < _init_array_end) {
		(*fn++)();
	}

}

void *__dso_handle = 0;


int __cxa_atexit(void(*f)(void *), void *p, void *d) {
	return 0;
}

void __cxa_pure_virtual(void) {
	while(1){}
}

int __cxa_guard_acquire(unsigned char *g) {
	if( *g == 0 ) {
		*g = 1;
		return 1;
	} else {
		return 0;
	}
}

void __cxa_guard_release(unsigned char *g) {
	*g = 2;
}
