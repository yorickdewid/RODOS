/*
 * board_config.cpp
 *
 *  Created on: 13.03.2014
 *      Author: wittkampm
 */

#include "board_config.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

void board_init() __attribute__((weak));

void board_init(){
	//
}
u32 board_interrupt_hook_secondary_service(
		u32 peripheral_interrupt_channel,
		bool isNoArg,
		isr_callback* callback,
		void* arg) __attribute__((weak));

u32 board_interrupt_hook_secondary_service(
		u32 peripheral_interrupt_channel,
		bool isNoArg,
		isr_callback* callback,
		void* arg) {
	return 1;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
