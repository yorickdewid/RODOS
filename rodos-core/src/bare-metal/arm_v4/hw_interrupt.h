
#ifndef ARMv4_INTERRUPT_H
#define ARMv4_INTERRUPT_H

#include <stdint.h>
namespace RODOS {		
//applies an Handler to a specific Interrupt
//there can be only one Handler for each Interrupt
void
installInterrupt(unsigned long interruptSource,
		unsigned long priority, void (*isrFunction)());

void
disableInterrupt(unsigned long interruptSource);
}
#endif	// ARMv4_INTERRUPT_H
