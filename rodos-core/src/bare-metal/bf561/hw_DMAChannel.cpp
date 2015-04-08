/*
 * hw_DMAChannel.cpp
 *
 *  Created on: 19.03.2014
 *      Author: wittkampm
 */

#include <hw_DMAChannel.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

DMAChannel::DMAChannel(
		volatile u16* regSrcDMAConfig,
		volatile u16* regSrcDMAIrqStatus,
		volatile void* regSrcNextDescPtr,
		volatile u16* regDstDMAConfig,
		volatile u16* regDstDMAIrqStatus,
		volatile void* regDstNextDescPtr,
		DMAManager* dmaManager
) {
	this->regSrcDMAConfig = regSrcDMAConfig;
	this->regSrcDMAIrqStatus = regSrcDMAIrqStatus;
	this->regDstDMAConfig = regDstDMAConfig;
	this->regDstDMAIrqStatus = regDstDMAIrqStatus;
	this->regSrcNextDescPtr = (volatile u32*) regSrcNextDescPtr;
	this->regDstNextDescPtr = (volatile u32*) regDstNextDescPtr;
	this->isActive = false;
	this->activeHead = NULL;
	this->activeTail = NULL;
	this->channelReset();
	this->dmaManager = dmaManager;
	this->dmaManager->addToKnownChannels(this);
}


void DMAChannel::channelReset(){
	void* criticalArg;
	system_enter_critical_region(criticalArg);
	*(this->regSrcDMAConfig) = 0;
	*(this->regSrcDMAIrqStatus) = 3;
	*(this->regDstDMAConfig) = 0;
	*(this->regDstDMAIrqStatus) = 3;
	// SSyncs after writes to DMA MMR Registers may not be handled correctly, Silicon Anomaly 05000302
	// insert dummy read before ssync
	volatile u32 t = *(this->regDstDMAIrqStatus);
	ssync();
	system_exit_critical_region(criticalArg);
}
void DMAChannel::channelWakeup() {
	void* criticalArg;
	system_enter_critical_region(criticalArg);
	if (((*(this->regSrcDMAConfig) & 0x80) == 0) && (!this->isActive) &&  (this->activeHead != NULL) && (!this->activeHead->isProcessed()) ) {
		this->isActive = true;
		//		// start DMA
		*(this->regSrcNextDescPtr) = (u32) this->activeHead->getSourceDescriptorAddr();
		*(this->regDstNextDescPtr) = (u32) this->activeHead->getDestinationDescriptorAddr();
		// SSyncs after writes to DMA MMR Registers may not be handled correctly, Silicon Anomaly 05000302
		// insert dummy read before ssync
		volatile u32 t = *(this->regDstNextDescPtr);
		ssync();
		*(this->regSrcDMAConfig) = 7<<12 | 9 << 8 | this->activeHead->getSourceDescriptor()->dma_config;	// activate FLOW and size of descriptor for the first job
		*(this->regDstDMAConfig) = 7<<12 | 9 << 8 | this->activeHead->getDestinationDescriptor()->dma_config; // activate FLOW and size of descriptor for the first job
		// SSyncs after writes to DMA MMR Registers may not be handled correctly, Silicon Anomaly 05000302
		// insert dummy read before ssync
		t = *(this->regDstDMAConfig);
		ssync();
	}
	system_exit_critical_region(criticalArg);
}

void DMAChannel::jobDone() {
	DMAJob* p = activeHead;
	this->channelReset();
	if (p!=NULL) {
		p->setProcessed();
	}
	this->isActive = false;
	this->dmaManager->addToFifo(this);
}
void DMAChannel::jobError() {
	DMAJob* p = activeHead;
	this->channelReset();
	if (p!=NULL){
		p->setProcessed();
		p->setError();
	}
	this->isActive = false;
	this->dmaManager->addToFifo(this);
}


bool DMAChannel::isInQueue(DMAJob *j) {
	DMAJob *cl;
	DMAJob *cq;
	cq = this->activeHead;
	while (cq != NULL) {
		cl = cq;
		while(cl != NULL) {
			if (cl == j) return true;
			else cl = cl->getNextInLine();
		}
		cq = cq->getNextInQueue();
	}
	return false;
}



void DMAChannel::addJob(DMAJob* j) {
	if (j == NULL) return;
	system_enter_critical_region(0);
	j->setNextInQueue(NULL);
	j->resetProcessed();
	j->resetError();
	if (this->activeHead == NULL) {
		this->activeHead = j;
	} else {
		this->activeTail->setNextInQueue(j);
	}
	this->activeTail = j;
	system_exit_critical_region(0);
	this->channelWakeup();
}

DMAJob* DMAChannel::getAndRemoveProcessedJob(){
	system_enter_critical_region(0);
	DMAJob* j = NULL;
	if ((this->activeHead != NULL) && this->activeHead->isProcessed()) {
		j = this->activeHead;
		this->activeHead = this->activeHead->getNextInQueue();
		j->setNextInQueue(NULL);
	}
	system_exit_critical_region(0);
	return j;
}

void DMAChannel::processJobs(){
	DMAJob* jobHead;
	DMAJob* jobHeadt;
	DMAJob* c;
	DMAJob* ct;
	// process the queue in the outer loop
	jobHead = this->getAndRemoveProcessedJob();
	while (jobHead != NULL) {
		// process the line in the inner loop
		c = jobHead;
		// prepare for the case the DMAJob callback is deleting itself
		jobHeadt = this->getAndRemoveProcessedJob();
		while (c != NULL) {
			// the job must be processed because only the last job in line raises the interrupt!
			c->setProcessed();
			// copy the error state of the first job to the whole line
			if (c->isError()) c->setError();
			ct = c->getNextInLine();
			c->executeCallback();
			c = ct;
		}
		// check the queue if another job has arrived during the processing
		if (jobHeadt != NULL) {
			jobHead = jobHeadt;
		} else {
			jobHead = this->getAndRemoveProcessedJob();
		}
	}
	// wakeup the channel for the next jobs
	this->channelWakeup();
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
