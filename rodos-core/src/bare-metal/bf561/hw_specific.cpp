/***********************************************************************************************//**
 * \ingroup rodos
 * \file hw_specific.cpp
 * \version 0.1
 * \date 19.02.2014
 * \author Georg Raith
 *
 * \brief 
 * \b History: \n
 *
 * 19.02.2014 rg: initial version
 */

#include "hw_config.h"
#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

volatile long taskRunning = 0;

void sp_partition_yield() {
  // empty
}

void startIdleThread() {}; // only for some architectures, most implementations == nop()

long* hwInitContext(long* stack, void* object) {
	memset(stack-((BFIN_CONTEXT_SIZE - 1)/4 ), 0, BFIN_CONTEXT_SIZE); //intialise stack with 0
	*(stack - 0) = 0xffa00000;		//reset vector corea, should never be used
	*(stack - 2) = (long) &threadStartupWrapper;  //RETI, Einsprungaddresee des context
	*(stack - 12) = (long)  object; 	//r0 aktuelle thread variable

	return (stack - BFIN_CONTEXT_SIZE);
}


void FFLUSH() {
#warning not implemented
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
