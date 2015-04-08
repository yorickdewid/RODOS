/*
 * hw_interrupt.cpp
 *
 *  Created on: 12.03.2014
 *      Author: wittkampm
 */



#include "hw_interrupt.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


typedef struct system_interrupt_node_s {
	u8 peripheral_interrupt_channel;
	u8 interrupt_vector_group;
	u32 isr_mask0;
	u32 isr_mask1;
	bool isNoArg;
	isr_callback* callback;
	void* arg;
} system_interrupt_node;

// delete this...
//typedef struct system_secondary_interrupt_node_s {
//	u8 channel;
//	isr_callback* callback;
//	void* arg;
//} system_secondary_interrupt_node;

#define SYSTEM_INTERRUPT_MAX_NODES 64
#define SYSTEM_INTERRUPT_MAX_NODES_PER_IVG 5

typedef struct system_interrupt_context_s {
	system_interrupt_node nodes[SYSTEM_INTERRUPT_MAX_NODES];
	system_interrupt_node* ivg[9][SYSTEM_INTERRUPT_MAX_NODES_PER_IVG];
	u8 current_node_index;
	u8 current_ivg_index[9];
	u32 unhandledInterrupts;
} system_interrupt_context;

system_interrupt_context context;

void system_interrupt_process(const u8 ivg_index) {
	int i;
	u32 ret = 1;
	system_interrupt_node* node;
#ifdef __BFIN_COREB
	u32 isr0 = *pSICB_ISR0;
	u32 isr1 = *pSICB_ISR1;
#else
	u32 isr0 = *pSICA_ISR0;
	u32 isr1 = *pSICA_ISR1;
#endif
	ssync();

	for (i = 0; i < context.current_ivg_index[ivg_index]; i++) {
		if (((context.ivg[ivg_index][i])->isr_mask0 & isr0)||((context.ivg[ivg_index][i])->isr_mask1 & isr1)) {
			node = context.ivg[ivg_index][i];
			if (*node->callback != 0) {
				if (node->isNoArg) {
					ret = 0;
					((isr_callback_no_arg*)(*node->callback))();
				} else {
					ret = (*node->callback)(ISR_CALLBACK_SERVICE_REQUEST, node->arg);
				}
			}
		}
	}
	if (ret != 0) {
		context.unhandledInterrupts++;
	}
	return;
}


__attribute__((interrupt_handler)) void system_interrupt_ivg7() {
	system_interrupt_process(0);
	return;
}

__attribute__((interrupt_handler)) void system_interrupt_ivg8() {
	system_interrupt_process(1);
	return;
}

__attribute__((interrupt_handler)) void system_interrupt_ivg9() {
	system_interrupt_process(2);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg10() {
	system_interrupt_process(3);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg11() {
	system_interrupt_process(4);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg12() {
	system_interrupt_process(5);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg13() {
	system_interrupt_process(6);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg14() {
	system_interrupt_process(7);
	return;
}
__attribute__((interrupt_handler)) void system_interrupt_ivg15() {
	system_interrupt_process(8);
	return;
}


void HW_InterruptInit() {
	// init the system interrupt context
	memset(&context,0,sizeof(context));

	// disable all periphery related core interrupts until they get populated by calling the enable function
	*pIMASK &= ~(0xff80);

	// set all peripherals to a default value of 13 which is the lowest priority for General Interrupts.
	// they will get the real values during initialization
	// also clear all mask and status bits
#ifdef __BFIN_COREB
	*pSICB_IAR0 = 0x66666666;
	*pSICB_IAR1 = 0x66666666;
	*pSICB_IAR2 = 0x66666666;
	*pSICB_IAR3 = 0x66666666;
	*pSICB_IAR4 = 0x66666666;
	*pSICB_IAR5 = 0x66666666;
	*pSICB_IAR6 = 0x66666666;
	*pSICB_IAR7 = 0x66666666;
	*pSICB_IMASK0 = 0;
	*pSICB_IMASK1 = 0;
	*pSICB_IWR0 = 0;
	*pSICB_IWR1 = 0;

#else
	*pSICA_IAR0 = 0x66666666;
	*pSICA_IAR1 = 0x66666666;
	*pSICA_IAR2 = 0x66666666;
	*pSICA_IAR3 = 0x66666666;
	*pSICA_IAR4 = 0x66666666;
	*pSICA_IAR5 = 0x66666666;
	*pSICA_IAR6 = 0x66666666;
	*pSICA_IAR7 = 0x66666666;
	*pSICA_IMASK0 = 0;
	*pSICA_IMASK1 = 0;
	*pSICA_IWR0 = 0;
	*pSICA_IWR1 = 0;
#endif

	*pEVT7 = (void (*)) (& system_interrupt_ivg7);
	*pEVT8 = (void (*)) (& system_interrupt_ivg8);
	*pEVT9 = (void (*)) (& system_interrupt_ivg9);
	*pEVT10 = (void (*)) (& system_interrupt_ivg10);
	*pEVT11 = (void (*)) (& system_interrupt_ivg11);
	*pEVT12 = (void (*)) (& system_interrupt_ivg12);
	*pEVT13 = (void (*)) (& system_interrupt_ivg13);
	*pEVT14 = (void (*)) (& __asmSaveContextAndCallScheduler_isr); //reserved for Scheduler software interrupt
	*pEVT15 = (void (*)) (& system_interrupt_ivg15);	// 'user level'


	//enable interrupt for Schedule-switching
	*pIMASK |= (1 << 14);

	// setup flags subsystem
	return;

}
/**
 * global disable of Interrupts
 */
void HW_disableInterrupts() {

}
/**
 * global enable of Interrupts
 */
void HW_enableInterrupts() {

}

/**
 * Mask (disable) the Interrupt of given ID.
 * @param id Interrupt ID
 */
void HW_maskInterrupts(unsigned int id) {

}
/**
 * Unmask (enable) the Interrupt of given ID.
 * @param id Interrupt ID
 */
void HW_unmaskInterrupts (unsigned int id) {


}

/**
 * @param peripheral_interrupt_channel
 * @param interrupt_vector_group defines the interrupt vector for the processor hierarchy. The value should be between 7 (highest) and 13 (lowest) priority.
 * @param wakeup if set to true the interrupt will wakeup the processor from idle or sleep states
 * @param isNoArg sets whether the callback is using arguments or not
 * @param callback the application service routine
 * @param a optional argument to the service routine
 * @return 0 in case of success
 */
u32 HW_setInterruptHandlerWithArgs(u32 peripheral_interrupt_channel, u8 interrupt_vector_group, bool wakeup, bool isNoArg, isr_callback* callback, void* arg) {
	u32 ret = 0;
	system_interrupt_node* node;
	int i;
	u32 mask;
	// test space
#ifdef DEBUG
	if (context.current_node_index >= SYSTEM_INTERRUPT_MAX_NODES) return 1;
#endif
	// get and populate node data
	node = &(context.nodes[context.current_node_index++]);
	node->isNoArg = isNoArg;
	node->callback = callback;
	node->arg = arg;
	node->interrupt_vector_group = interrupt_vector_group;
	node->peripheral_interrupt_channel = peripheral_interrupt_channel & 0x3f;

	// set mask prototype
	if (node->peripheral_interrupt_channel < 32) {
		node->isr_mask0 = 1<<node->peripheral_interrupt_channel;
		node->isr_mask1 = 0;
	} else {
		node->isr_mask0 = 0;
		node->isr_mask1 = 1<<(node->peripheral_interrupt_channel - 32);
	}
	//
	if (peripheral_interrupt_channel & 0x80000000) {
		// service belongs to secondary channel, call the function if exists
		ret = board_interrupt_hook_secondary_service(peripheral_interrupt_channel, isNoArg, callback, arg);
		context.current_node_index--;
	} else {
		// hook directly, catch errors
#ifdef DEBUG
		if (context.current_ivg_index[interrupt_vector_group-7] >= SYSTEM_INTERRUPT_MAX_NODES_PER_IVG) {
			context.current_node_index--;
			ret = 1;
		}
#endif
		// put a pointer to the node into the list of the right ivg level
		context.ivg[interrupt_vector_group-7][context.current_ivg_index[interrupt_vector_group-7]++] = node;
		// activate the ivg in the core IMASK register
		*pIMASK |= 1<<interrupt_vector_group;
		// assign the priority of this channel in the system priority register file
		i = node->peripheral_interrupt_channel >> 3;
		mask = 0xf << ((node->peripheral_interrupt_channel & 0x7) * 4);
		switch (i) {
	#ifdef  __BFIN_COREB
		case 0: { *pSICB_IAR0 &= ~mask; *pSICB_IAR0 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 1: { *pSICB_IAR1 &= ~mask; *pSICB_IAR1 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 2: { *pSICB_IAR2 &= ~mask; *pSICB_IAR2 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 3: { *pSICB_IAR3 &= ~mask; *pSICB_IAR3 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 4: { *pSICB_IAR4 &= ~mask; *pSICB_IAR4 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 5: { *pSICB_IAR5 &= ~mask; *pSICB_IAR5 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 6: { *pSICB_IAR6 &= ~mask; *pSICB_IAR6 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 7: { *pSICB_IAR7 &= ~mask; *pSICB_IAR7 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
	#else
		case 0: { *pSICA_IAR0 &= ~mask; *pSICA_IAR0 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 1: { *pSICA_IAR1 &= ~mask; *pSICA_IAR1 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 2: { *pSICA_IAR2 &= ~mask; *pSICA_IAR2 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 3: { *pSICA_IAR3 &= ~mask; *pSICA_IAR3 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 4: { *pSICA_IAR4 &= ~mask; *pSICA_IAR4 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 5: { *pSICA_IAR5 &= ~mask;	*pSICA_IAR5 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 6: { *pSICA_IAR6 &= ~mask;	*pSICA_IAR6 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
		case 7: { *pSICA_IAR7 &= ~mask; *pSICA_IAR7 |= ((interrupt_vector_group-7) << ((node->peripheral_interrupt_channel & 0x7) * 4)); } break;
	#endif
		}
		// setup wakeup registers
		if (wakeup) {
	#ifdef __BFIN_COREB
			*pSICB_IWR0 |= node->isr_mask0;
			*pSICB_IWR1 |= node->isr_mask1;
	#else
			*pSICA_IWR0 |= node->isr_mask0;
			*pSICA_IWR1 |= node->isr_mask1;
	#endif
		}
		// setup mask of the system interrupt controller
	#ifdef __BFIN_COREB
		*pSICB_IMASK0 |= node->isr_mask0;
		*pSICB_IMASK1 |= node->isr_mask1;
	#else
		*pSICA_IMASK0 |= node->isr_mask0;
		*pSICA_IMASK1 |= node->isr_mask1;
	#endif
	}
	if (!ret) {
		if (node->isNoArg) {
			// skip calling isr without argument
			//((isr_callback_no_arg*)(*node->callback))();
		} else
		{
			// call application callback with setup event
			(*node->callback)(ISR_CALLBACK_ENABLED, node->arg);
		}
	}
	return ret;
}

u32 system_interrupt_unhook(u32 channel) {
	// @todo implement the interrupt unhook function
#warning unhook interrupt callbacks not implemented!
	return 1;
}



/**
 * Hook a service routine to a given Interrupt ID.
 * @param index Interrupt ID
 * @param interruptHandler Service Routine for this Interrupt ID
 */
void HW_setInterruptHandler(unsigned int index, void (*interruptHandler)()) {
	HW_setInterruptHandlerWithArgs(index, HW_STD_IVG, false, true, (isr_callback*) interruptHandler, NULL);
}

/**
 * Get number of unhandled interrupts.
 */
unsigned long HW_getUnhandledInterruptsCounter() {
	return 0;
}

volatile u8 cr_counter = 0;
volatile u32 cr_buffer_interrupt_mask;

void system_enter_critical_region(void *pArg) {
	volatile u32 interrupt_mask;
	csync();
	if (cr_counter == 0) {
		//global disable interrupts
		__asm__("CLI %0;" : "=r" (interrupt_mask));
		csync();
		cr_buffer_interrupt_mask = interrupt_mask;
	}
	cr_counter++;
}


void system_exit_critical_region(void *pArg) {
	volatile u32 interrupt_mask;

	csync();
	cr_counter--;
	if (cr_counter == 0){//activate interrupts
		interrupt_mask = cr_buffer_interrupt_mask;
		__asm__("STI %0;" : "=r" (interrupt_mask));
		csync();
	}

}


bool isInterrupt() {
	if ((*pIPEND & (~((0x7) | (1 << 15)))) != 0) {
		return true;
	}

	return false;
}

bool isCritRegion() {
	if (cr_counter > 0) {
		return true;
	}

	return false;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
