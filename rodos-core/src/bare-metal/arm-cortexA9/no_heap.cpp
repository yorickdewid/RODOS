/*
 * Source: Building Bare-Metal ARM Systems with GNU
 * "Finally, if you don’t use the heap, which you shouldn’t in robust,
 * deterministic applications, you can reduce the C++ overhead even further."
 *
 */

#include <stdlib.h>


extern "C" void *malloc(size_t) {
	return (void *)0;
}


extern "C" void free(void *) {

}
