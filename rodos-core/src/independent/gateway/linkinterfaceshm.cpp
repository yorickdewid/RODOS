/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

#include "rodos.h"
#include "hal/hal_sharedmemory.h"
#include <new>
#include "gateway/linkinterfaceshm.h"

#define FIFO_MAGIC_WORD 1234

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

HAL_Sharedmemory hal_sharedmemory(Sharedmemory_IDX0);

LinkinterfaceSHM::LinkinterfaceSHM(Sharedmemory_IDX shmIdx) :
		Linkinterface(-1) {
	this->shmIdx = shmIdx;
}

void LinkinterfaceSHM::init() {

	Linkinterface::init();
	void * sharedMemoryUserSpace;
	fifo=0;

	new (&hal_sharedmemory) HAL_Sharedmemory(shmIdx); // placement new to avoid dynamic memory allocation
	//get sharedMemory pointer
	int32_t size = 20000; //20kB
	int32_t maxMembers = MAXMEMBERS;
	sharedMemoryUserSpace = hal_sharedmemory.init(&size, &maxMembers);
	this->readerId = hal_sharedmemory.getMemberId();

	if(sharedMemoryUserSpace==0){
		ERROR("Error initializing shared memory\n");
		return;
	}
	hal_sharedmemory.setIoEventReceiver(this);

	//lock shared memory
	while (hal_sharedmemory.lock() == false)
		;
	//create fifo
	if (*((int32_t*) sharedMemoryUserSpace) == FIFO_MAGIC_WORD) {
		fifo = (MultipleReaderFifo<NetworkMessage, FIFOSIZE, MAXMEMBERS> *) ((int32_t*) sharedMemoryUserSpace + 1);
	} else {
		*((int32_t*) sharedMemoryUserSpace) = FIFO_MAGIC_WORD;
		fifo = new ((int32_t*) sharedMemoryUserSpace + 1) MultipleReaderFifo<NetworkMessage, FIFOSIZE, MAXMEMBERS>();
	}

	//unlock shared memory
	hal_sharedmemory.unlock();

}

bool LinkinterfaceSHM::sendNetworkMsg(NetworkMessage &outMsg) {
	if(fifo==0)
		return false;

	//lock shared memory
	while (hal_sharedmemory.lock() == false)
		;

	//write
	fifo->put(outMsg);

	//unlock
	hal_sharedmemory.unlock();

	//inform other processes
	hal_sharedmemory.raiseSharedMemoryChanged();

	return true;
}

bool LinkinterfaceSHM::getNetworkMsg(NetworkMessage &inMsg, int32_t &numberOfReceivedBytes) {
	if(fifo==0)
		return false;
	numberOfReceivedBytes = -1;
	hal_sharedmemory.resetDataReady();//ack shared memory
	return fifo->get(inMsg, readerId);
}

void LinkinterfaceSHM::onWriteFinished() {
    //Method does not make sense for HAL_Sharedmemory because there is no write function
}

void LinkinterfaceSHM::onDataReady() {
    if(threadToResume) threadToResume->resume();
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
