/*
 * hw_DMAManager.cpp
 *
 *  Created on: 19.03.2014
 *      Author: wittkampm
 */

#include <hw_DMAManager.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


DMAManager::DMAManager() : Thread("DMAManager", DMA_MGR_PRIO, DMA_MGR_STACKSIZE){
	knownChannelIndex = 0;
}

DMAManager::~DMAManager() {
	//
}

// ISR related and called from ISR!
void DMAManager::addToFifo(DMAChannel* channel) {
	// protected-add the channel to the fifo
	this->readyChannels.putProtected(&(*channel));
	// resume the thread
	this->resume();
}

bool DMAManager::addToKnownChannels(DMAChannel* channel) {
	if (this->knownChannelIndex < DMA_MGR_KNOWN_CHANNELS) {
		this->knownChannels[this->knownChannelIndex++] = channel;
		return true;
	} else return false;

}
void DMAManager::init() {
	// start the thread once per second
#warning checkme: nur nötig wenn ein periodischer Check wirklich gebraucht wird.
	this->setPeriodicBeat(NOW(), 1*SECONDS);
}

void DMAManager::run() {
	int i;
	DMAChannel* currentChannel;
	while (1) {
		// iterate through the array of known channels to get processed jobs
		for (i = 0; i < this->knownChannelIndex; i++) {
#warning checkme: nur nötig wenn periodische checks wirklich gebraucht werden
			this->knownChannels[i]->processJobs();
		}
		// sleep until interrupt occurred
		this->suspendUntilNextBeat();
		// process the list of the readyChannel Fifo
		while (this->readyChannels.getProtected(currentChannel)) {
			currentChannel->processJobs();
		}
	}
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

