
#include <stdint.h>

#include "hw_specific.h"
#include "lpc24xx_registers.h"
namespace RODOS {
//applies an Handler to a specific Interrupt
//there can be only one Handler for each Interrupt
void
installInterrupt(unsigned long interruptSource,
		unsigned long priority, void (*isrFunction)())
{
	*(&VICVectAddr0 + interruptSource) = (uint32_t) isrFunction;
	*(&VICVectPriority0 + interruptSource) = priority & 0x0000000f;
	
	// Interrupt belongs to IRQ and not FIQ
	VICIntSelect &= ~(1 << interruptSource);
	VICIntEnable = (1 << interruptSource);
}

void
disableInterrupt(unsigned long interruptSource) 
{
	VICIntEnClear = (1 << interruptSource);
}
}
