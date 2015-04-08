/*
 * board_config.h
 *
 *  Created on: 13.03.2014
 *      Author: wittkampm
 */

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "hw_config.h"





#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * Board specific configuration.
 * Specific boards might override this function in external code.
 */
void board_init() ;
u32 board_interrupt_hook_secondary_service(
		u32 peripheral_interrupt_channel,
		bool isNoArg,
		isr_callback* callback,
		void* arg);

#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* BOARD_CONFIG_H_ */
