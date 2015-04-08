/**
 * @brief Idle function: switches off CPU (parts of it, anyway) until next interrupt.
 */

#include "hw_core.h"
#include "hw_idle.h"

void hwIdle() {
	// power-down
	__asm volatile("wr %g0, %asr19\n");
}
