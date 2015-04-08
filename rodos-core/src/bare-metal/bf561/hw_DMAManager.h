/*
 * hw_DMAManager.h
 *
 *  Created on: 19.03.2014
 *      Author: wittkampm
 */

#ifndef HW_DMAMANAGER_H_
#define HW_DMAMANAGER_H_

#include "rodos.h"
#include "hw_DMAChannel.h"
#include "hw_DMAJob.h"
#include "protectedFifo.h"

#warning find better place
#define DMA_MGR_KNOWN_CHANNELS	2
#define DMA_MGR_PRIO	701
#define DMA_MGR_STACKSIZE 4096

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class DMAChannel;


class DMAManager : public Thread {
	ProtectedFifo<DMAChannel*, DMA_MGR_KNOWN_CHANNELS> readyChannels;
	DMAChannel* knownChannels[DMA_MGR_KNOWN_CHANNELS];
	int knownChannelIndex;
	DMAChannel* getNextInQueue();
public:
	DMAManager();
	virtual ~DMAManager();
	void run();
	void init();
	void addToFifo(DMAChannel* channel);

	/** return true == ok, false == known channels array full */
	bool addToKnownChannels(DMAChannel* channel);
};


#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* HW_DMAMANAGER_H_ */
