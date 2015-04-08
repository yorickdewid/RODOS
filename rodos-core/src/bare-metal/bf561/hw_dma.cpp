/*
 * hw_dma.cpp
 *
 *  Created on: 17.03.2014
 *      Author: wittkampm
 */



#include "hw_dma.h"
#include "hw_DMAJob.h"
#include "hw_DMAChannel.h"
#include "hw_DMAManager.h"
#include "rodos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*********************************************************//*
	define channels and manager
*/

static DMAManager manager;
static DMAChannel dma1_stream0 (
		pMDMA1_S0_CONFIG,			//u32* regSrcDMAConfig,
		pMDMA1_S0_IRQ_STATUS, 		//u32* regSrcDMAIrqStatus,
		pMDMA1_S0_NEXT_DESC_PTR,  	//u32* regSrcNextDescPtr,
		pMDMA1_D0_CONFIG,			//u32* regDstDMAConfig,
		pMDMA1_D0_IRQ_STATUS, 		//u32* regDstDMAIrqStatus,
		pMDMA1_D0_NEXT_DESC_PTR,	//u32* regDstNextDescPtr,
		&manager);					//DMAManager*

static DMAChannel dma1_stream1 (
		pMDMA1_S1_CONFIG,			//u32* regSrcDMAConfig,
		pMDMA1_S1_IRQ_STATUS, 		//u32* regSrcDMAIrqStatus,
		pMDMA1_S1_NEXT_DESC_PTR,  	//u32* regSrcNextDescPtr,
		pMDMA1_D1_CONFIG,			//u32* regDstDMAConfig,
		pMDMA1_D1_IRQ_STATUS, 		//u32* regDstDMAIrqStatus,
		pMDMA1_D1_NEXT_DESC_PTR,	//u32* regDstNextDescPtr,
		&manager);					//DMAManager*

/*********************************************************//*
	DMA callbacks
	The argument is always a pointer to a DMA Queue
*/

u32 system_dma_callback(isr_callback_event event, void* arg) {
	DMAChannel* q = (DMAChannel*) arg;
	if (event == ISR_CALLBACK_SERVICE_REQUEST) {
		if (q != NULL) q->jobDone();
	}
	return 0;
}

u32 system_dma_error(isr_callback_event event, void* arg) {
	if (event == ISR_CALLBACK_SERVICE_REQUEST) {
		if ((*pMDMA1_D0_IRQ_STATUS & 0x2) || (*pMDMA1_S0_IRQ_STATUS & 0x2)) {
			// Memory Stream 0
			dma1_stream0.jobError();
			*pMDMA1_D0_IRQ_STATUS = 3;
			ssync();
			*pMDMA1_S0_IRQ_STATUS = 3;
			ssync();
		}
		if ((*pMDMA1_D1_IRQ_STATUS & 0x2) || (*pMDMA1_S1_IRQ_STATUS & 0x2)) {
			// Memory Stream 1
			dma1_stream1.jobError();
			*pMDMA1_D1_IRQ_STATUS = 3;
			ssync();
			*pMDMA1_S1_IRQ_STATUS = 3;
			ssync();
		}
		// \todo add other channels here
	}
	return 0;
}

/*********************************************************//*
	DMA1 service
*/
/**
 * Init of the DMA1 Hardware:
 */
void hw_dma1_init() {
	/// -# Hook Callback for Memory Stream 0
	HW_setInterruptHandlerWithArgs(DMA1_STREAM0_PIC, DMA1_STREAM0_IVG, false, false, &system_dma_callback, &dma1_stream0);

	/// -# Hook Callback for Memory Stream 1
	HW_setInterruptHandlerWithArgs(DMA1_STREAM1_PIC, DMA1_STREAM1_IVG, false, false, &system_dma_callback, &dma1_stream1);

	// \todo add other channels here

	/// -# Finally Hook Error Callback for all streams
	HW_setInterruptHandlerWithArgs(DMA1_ERROR_PIC, DMA1_ERROR_IVG, false, false, &system_dma_error, NULL);
}
/*********************************************************//*
	DMA2 service
*/
void hw_dma2_init() {

}
/*********************************************************//*
	IMDMA service
*/

void hw_imdma_init() {

}
/*********************************************************//*
	user visible functions of the DMA service
*/
/**
 * Global Init for all DMA controllers.
 * Call in hw_init.
 */
void hw_dma_init() {
	hw_dma1_init();
	hw_dma2_init();
	hw_imdma_init();

}
/**
 * Access to pointers to the DMA resources installed in the system.
 * @param ch channel number
 * @return a pointer to the DMAChannel or NULL if the requested channel is not installed.
 */
DMAChannel* hw_dma_get_channel(system_dma_channel_handle ch) {
	DMAChannel* c = NULL;
	switch (ch) {
	case system_dma1_s0: c = &dma1_stream0; break;
	case system_dma1_s1: c = &dma1_stream1; break;
//	case system_dma2_uartTx:
//	case system_dma2_uartRx:
	// \todo add others channels here
	default: break;
	}
	return c;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
