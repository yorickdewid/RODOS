/*
 * hw_dma.h
 *
 *  Created on: 17.03.2014
 *      Author: wittkampm
 */

#ifndef HW_DMA_H_
#define HW_DMA_H_

#include "hw_config.h"
#include "hw_DMAChannel.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#warning find better place...
#define DMA_JOB_POOL_SIZE	10
#define DMA1_STREAM0_PIC	53
#define DMA1_STREAM0_IVG	8
#define DMA1_STREAM0_SRC_CH	13
#define DMA1_STREAM0_DST_CH	12

#define DMA1_STREAM1_PIC	54
#define DMA1_STREAM1_IVG	8
#define DMA1_STREAM1_SRC_CH	15
#define DMA1_STREAM1_DST_CH	14

#define DMA1_ERROR_PIC	1
#define DMA1_ERROR_IVG	7
#define DMA2_ERROR_PIC	2
#define DMA2_ERROR_IVG	7
#define DMA1_MAX_PENDING_JOBS 10
#define DMA2_MAX_PENDING_JOBS 10



typedef enum {
	none,			// Use PIO I/O
	system_dma1_s0,	// Memory Stream 0 on DMA1
	system_dma1_s1,	// Memory Stream 1 on DMA1
	system_dma2_uartTx,	// UART Transmit on DMA2
	system_dma2_uartRx,	// UART Receive on DMA2

	// \todo add others channels here
} system_dma_channel_handle;





///**
// * transfer of word-size 32 bit between all memory sections
// * @return
// */
//u32 system_dma_init_dma1();
///**
// *
// * @return
// */
//u32 system_dma_init_dma2();
///**
// *
// * @return
// */
//u32 system_dma_init_imdma();
//
class DMAChannel;

DMAChannel* hw_dma_get_channel(system_dma_channel_handle ch);

void hw_dma_init();


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* HW_DMA_H_ */
