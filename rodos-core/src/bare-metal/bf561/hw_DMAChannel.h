/*
 * hw_DMAChannel.h
 *
 *  Created on: 19.03.2014
 *      Author: wittkampm
 */

#ifndef HW_DMACHANNEL_H_
#define HW_DMACHANNEL_H_

#include "hw_dma.h"
#include "hw_DMAManager.h"
#include "hw_DMAJob.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class DMAManager;
class DMAJob;


class DMAChannel {

	DMAJob* activeHead;
	DMAJob* activeTail;
	bool isActive;
	volatile u16* regSrcDMAConfig;
	volatile u16* regSrcDMAIrqStatus;
	volatile u32* regSrcNextDescPtr;
	volatile u16* regDstDMAConfig;
	volatile u16* regDstDMAIrqStatus;
	volatile u32* regDstNextDescPtr;

	DMAManager* dmaManager;
	DMAJob* getAndRemoveProcessedJob();


public:
	void channelReset();
	void channelWakeup();

	DMAChannel(
			volatile u16* regSrcDMAConfig,
			volatile u16* regSrcDMAIrqStatus,
			volatile void* regSrcNextDescPtr,
			volatile u16* regDstDMAConfig,
			volatile u16* regDstDMAIrqStatus,
			volatile void* regDstNextDescPtr,
			DMAManager* dmaManager
	);
	void jobDone();
	void jobError();

	void addJob(DMAJob* j);

	bool isInQueue(DMAJob* j);

	void processJobs();
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* HW_DMACHANNEL_H_ */
