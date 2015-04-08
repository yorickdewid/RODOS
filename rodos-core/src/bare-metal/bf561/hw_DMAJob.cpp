/**
 *
 * hw_DMAJob.cpp
 *
 *  Created on: 17.03.2014
 *      Author: wittkampm
 */

#include <hw_DMAJob.h>
#include <string_pico.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


DMAJob::DMAJob() {
	callbackArg = NULL;
	memset(&descSource, 0, sizeof(dma_descriptor));
	memset(&descDest, 0, sizeof(dma_descriptor));
	nextInLine = NULL;
	nextInQueue = NULL;
	processed = false;
	error = false;
	appCallback = NULL;
	owner = NULL;
}


void DMAJob::init(
		Thread* owner,
		DMAJob* next,
		isr_callback* appCallback,
		void* arg,
		void* start_addr_source,
		void* start_addr_destination,
		system_dma_transferwidth width,
		u16 xcount,
		u16 ycount
)
{
	this->init(
			owner,
			next,
			appCallback,
			arg,
			start_addr_source,
			start_addr_destination,
			width,
			xcount,
			ycount,
			1 << width,
			1 << width,
			xcount * (1 << width),
			xcount * (1 << width)
	);
}

void DMAJob::init(
		Thread* owner,
		DMAJob* next,
		isr_callback* appCallback,
		void* arg,
		void* start_addr_source,
		void* start_addr_destination,
		system_dma_transferwidth width,
		u16 xcount,
		u16 ycount,
		s16 xsourcemodify,
		s16 xdestmodify,
		s16 ysourcemodify,
		s16 ydestmodify) {
	// set the callback
	this->appCallback = appCallback;
	this->callbackArg = arg;
	this->owner = owner;
	// init the pointer, call append later with next as argument
	this->nextInLine = NULL;
	this->nextInQueue = NULL;
	this->processed = false;
	this->error = false;

	// populate the descriptor fields,
	// the "modify"-fields counts in bytes, regardless of the transfer size whereas the "count" fields are in elements!
	// see "Naming Conventions for Memory DMA Registers"
	this->descDest.start_address = start_addr_destination;
	this->descDest.xcount = xcount;
	this->descDest.xmodify = xdestmodify;
	this->descDest.ycount = ycount;
	this->descDest.ymodify = ydestmodify;
	this->descSource.start_address = start_addr_source;
	this->descSource.xcount = xcount;
	this->descSource.xmodify = xsourcemodify;
	this->descSource.ycount = ycount;
	this->descSource.ymodify = ysourcemodify;

	// generate the DMA config word
	if (ycount > 1) {
		// 32bit transfer, 2d mode, WNR, DMAEN
		this->descDest.dma_config = ( 1 << 4 | (width << 2) | 1 << 1| 1 );
		// 32bit transfer, 2d mode, DMAEN
		this->descSource.dma_config = ( 1 << 4 | (width << 2) | 1);
	}
	else
	{
		// enable 1d transfer mode
		// width-bit transfer, WNR, DMAEN
		this->descDest.dma_config = ( (width << 2) | 1 << 1| 1 );
		// 32bit transfer, DMAEN
		this->descSource.dma_config = ( (width << 2) | 1);
	}
	// link the descriptor chains
	this->append(next);


}

/**
 * Get the internal source DMA descriptor.
 * @return the internal descriptor
 */
dma_descriptor* DMAJob::getSourceDescriptor() const {
	return (dma_descriptor*) &(this->descSource);
}
volatile void* DMAJob::getSourceDescriptorAddr() const {
	return (volatile void*) &(this->descSource);
}
volatile void* DMAJob::getDestinationDescriptorAddr() const {
	return (volatile void*) &(this->descDest);
}

/**
 * Get the internal destination DMA descriptor.
 * @return the internal descriptor
 */
dma_descriptor* DMAJob::getDestinationDescriptor() const {
	return (dma_descriptor*) &(this->descDest);
}

void DMAJob::append(DMAJob* next) {
	if (next != NULL) {
		// middle of the queue, enable flow in large descriptor mode with 9 descriptor entries
		this->descSource.next_desriptor = next->getSourceDescriptor();
		this->descDest.next_desriptor = next->getDestinationDescriptor();
		this->descSource.dma_config |= (7 << 12 | 9 << 8 );
		this->descDest.dma_config |= (7 << 12 | 9 << 8 );
		// disable interrupts for the destination channel
		this->descDest.dma_config &= ~(1<<7);
	} else {
		// the last element in queue, enable interrupt for destination by setting the interrupt enable bit
		// prepare the descriptors to stop the current DMA process
		this->descDest.next_desriptor = NULL;
		this->descSource.next_desriptor = NULL;
		this->descDest.dma_config &= ~(0xff00);
		this->descDest.dma_config |= (1<<7);
		this->descSource.dma_config &= ~(0xff00);
	}
	this->nextInLine = next;
	this->flushDescriptor();
}

Thread* DMAJob::getOwner() const { return this->owner; }

/**
 * get the state of the object - processed or not
 * @return the current processed state
 */
bool DMAJob::isProcessed() const {return this->processed; }
/**
 * Set the object to processed state.
 */
void DMAJob::setProcessed() { this->processed = true; }
/**
 * Reset the processed state of the object.
 */
void DMAJob::resetProcessed() { this->processed = false; }

/**
 * get the next Object in queue
 * @return the next Object in queue
 */
DMAJob* DMAJob::getNextInQueue() const { return this->nextInQueue; }
/**
 * get the next Object in Line
 * @return the next Object in Line
 */
DMAJob* DMAJob::getNextInLine() const {return this->nextInLine; }

/**
 * set the arguement object to be the next object in queue
 * @param queue
 */
void DMAJob::setNextInQueue(DMAJob* queue) {
	this->nextInQueue = queue;
}

/**
 * get the error state of the object.
 * \note the error state is if an error occurred during DMA processing, usually due to wrong descriptor settings.
 * @return the error state
 */
bool DMAJob::isError() const {
	return this->error;
}
/**
 * set to error state
 */
void DMAJob::setError() {
	this->error = true;
}
/**
 * reset the error state
 */
void DMAJob::resetError() {
	this->error = false;
}

void DMAJob::executeCallback() {
	if (this->appCallback != NULL) {
		if (this->processed) {
			if (this->error) this->appCallback(ISR_CALLBACK_DMA_ERROR, this->callbackArg);
			else this->appCallback(ISR_CALLBACK_DMA_DONE, this->callbackArg);
		} else {
			this->appCallback(ISR_CALLBACK_DRIVER_HW_ERROR, this->callbackArg);
		}
	}
}

int DMAJob::getXCount( ) const { return this->descDest.xcount; }
int DMAJob::getYCount() const { return this->descDest.ycount; }
int DMAJob::getSize() const {
	int y = (this->descDest.ycount < 1) ? 1 : this->descDest.ycount;
	y *= this->descDest.xcount;
	return y;
}

DMAJob::~DMAJob() {
	// nothing to destruct
}

void DMAJob::flushDescriptor() {
	mfc_FlushCacheArea(&(this->descDest), &(this->descDest) + 1);
	mfc_FlushCacheArea(&(this->descSource), &(this->descSource) + 1);
}
void DMAJob::flushDescriptorLine() {
	DMAJob* j = this;
	while(j != NULL) {
		j->flushDescriptor();
		j = j->getNextInLine();
	}
}

void DMAJob::flushSource() {
	if (this->descSource.dma_config) {
		mfc_FlushCacheArea((u32*)this->descSource.start_address, (u32*)((u32*)this->descSource.start_address + this->getSize()));
	}
}
void DMAJob::flushDestination() {
#warning untested code
	if (this->descDest.dma_config) {
		mfc_FlushCacheArea((u32*)this->descDest.start_address, (u32*)((u32*)this->descDest.start_address + this->getSize()));
	}
}

void DMAJob::flushSourceLine() {
	DMAJob* j = this;
	while(j != NULL) {
		j->flushSource();
		j = j->getNextInLine();
	}
}
void DMAJob::flushDestinationLine() {
#warning untested code
	DMAJob* j = this;
	while(j != NULL) {
		j->flushDestination();
		j = j->getNextInLine();
	}
}

void DMAJob::flushInvalidateSource() {
#warning untested code
	if (this->descSource.dma_config) {
		mfc_FlushInvalidateCacheArea((u32*)this->descSource.start_address, (u32*)((u32*)this->descSource.start_address + this->getSize()));
	}
}
void DMAJob::flushInvalidateDestination() {
#warning untested code
	if (this->descDest.dma_config) {
		mfc_FlushInvalidateCacheArea((u32*)this->descDest.start_address, (u32*)((u32*)this->descDest.start_address + this->getSize()));
	}
}

void DMAJob::flushInvalidateSourceLine() {
#warning untested code
	DMAJob* j = this;
	while(j != NULL) {
		j->flushInvalidateSource();
		j = j->getNextInLine();
	}
}
void DMAJob::flushInvalidateDestinationLine() {
#warning untested code
	DMAJob* j = this;
	while(j != NULL) {
		j->flushInvalidateDestination();
		j = j->getNextInLine();
	}
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
