/*
 * Source: Building Bare-Metal ARM Systems with GNU
 * "... standard new and delete operators throw exceptions and therefore require
 * the library support for exception handling. (The new and delete operators
 * are used in the static constructor/destructor invocation code, so are
 * linked in even if you don’t use the heap anywhere in your application.)"
 * This change saves 50KB of code together with the no_heap.cpp
 */


#include <stdlib.h>


void *operator new(size_t size) throw() {
	return malloc(size);
}

extern "C" int __aeabi_atexit(void *object, void (*destructor)(void *), void *dso_handle) {
	return 0;
}


/*
 * http://elegantinvention.com/blog/information/smaller-binary-size-with-c-on-baremetal-g/
 * remove -lstdc++ from linker flags and then adding -nostdlib.
 * Then we need to define this function.
 * This reduced the size of the .bin file.
 */
extern "C" void __cxa_pure_virtual(){
	while(1);
}
