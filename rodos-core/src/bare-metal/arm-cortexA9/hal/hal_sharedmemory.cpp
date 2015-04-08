/*
 * Copyright (c) 2013 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Johannes Freitag
 */

#include "rodos.h"
#include "scheduler.h"
#include "hal/hal_sharedmemory.h"
#include <new>

#include "workspace.h"

#define MAX_NUM_SHM 6 // 6 is for debug msg from secondary cores
#define NUM_CORES 4
#define MAGIC_WORD 1235

//shared DDR section
extern "C"{
extern char _DDR_FREE_START;
extern void*  _DDR_FREE_LENGTH;

}
#define DDR_FREE (&_DDR_FREE_START)
#define DDR_FREE_LENGTH ((int)&_DDR_FREE_LENGTH)




#ifndef NO_RODOS_NAMESPACE

namespace RODOS {
#endif

#define MEMORY_BARRIER 	asm volatile ("dmb \n\t"	:::"memory"	)




class ARMInterCoreMutex {
	volatile int owner; // =0 means unlocked
public:
	ARMInterCoreMutex();
	void lock();
	bool tryLock();
	void unlock();
	int getOwner(){
		return owner;
	}
};

ARMInterCoreMutex::ARMInterCoreMutex(){
	owner=0;
}
void ARMInterCoreMutex::lock(){
	while(!tryLock()){
		if(Scheduler::isSchedulerRunning()){
			Thread::yield();
		}

	}
}
bool ARMInterCoreMutex::tryLock(){
	volatile int* ownerAddr = &owner;
	int c= cpu_get_current()+1;
	int result=2; // 0 we locked it, 1 load-store-exclusive failed, 2 someone else locked it


		do{
			result=2;
			//xprintf("###=%d %d ",result,owner);
			hwDisableInterrupts();
			asm volatile (
				"ldrex ip,[%[own]] \n\t"
				"cmp ip,#0 \n\t"
				"strexeq %[res],%[cpu],[%[own]] \n\t"

				:[res]"+r"(result):[own]"r"(ownerAddr),[cpu]"r"(c):"memory","cc","r12"
				);
			hwEnableInterrupts();
			//xprintf("ldstrex=%d %d ",result,owner);
		}while (result==1);

		MEMORY_BARRIER;
	return (result == 0);
}
void ARMInterCoreMutex::unlock(){
	if(owner==cpu_get_current()+1){
		MEMORY_BARRIER;
		owner=0;
	}
}



struct shared_static_vars_t{
	ARMInterCoreMutex shared_static_vars_mutex;
	char* shared_ddr_AllocPointer;
	HW_HAL_Sharedmemory* shms[MAX_NUM_SHM];
};

shared_static_vars_t * shared_static_vars = 0;

int shared_static_vars_init(){
	if(shared_static_vars!= 0){
		return 1;
	}

	shared_static_vars_t* sharedVars;

	if(cpu_get_current()==0){
		//xprintf("S1 ");
		sharedVars = new (DDR_FREE) shared_static_vars_t();
		//xprintf("S2 ");
		sharedVars->shared_ddr_AllocPointer = DDR_FREE + sizeof(shared_static_vars_t);
		for(int i=0;i<MAX_NUM_SHM;i++){
				sharedVars->shms[i]=0;
		}

	}else{
		sharedVars = (shared_static_vars_t*)DDR_FREE;
	}
	shared_static_vars = sharedVars;
	return 0;
}

int shared_static_vars_initializer_dummy = shared_static_vars_init();

//The shared_static_vars_mutex must be locked when using this function
char* shared_DDR_malloc(int32_t size){
	size += 8; //For alignemnt

	//shared_static_vars->shared_static_vars_mutex.lock();
	if(shared_static_vars->shared_ddr_AllocPointer+size >= DDR_FREE+DDR_FREE_LENGTH){
		ERROR("Out of shared core DDR Memory");
		//shared_static_vars->shared_static_vars_mutex.unlock();
		return 0;
	}

	char* newMem = shared_static_vars->shared_ddr_AllocPointer;
	shared_static_vars->shared_ddr_AllocPointer+= size;

	//shared_static_vars->shared_static_vars_mutex.unlock();

	//Zero memory
	for(int i=0;i<size;i++){
		newMem[i]=0;
	}

	newMem = (char*)(((intptr_t)newMem +8) & (~7)); // Allign on 8 bytes
	return newMem;
}



class HW_HAL_Sharedmemory {
public:
	int idx;
	HAL_Sharedmemory* hal_shm[NUM_CORES];

	char* shmStart;
	uint32_t shmLength;
	bool opened[NUM_CORES];
	volatile bool changed[NUM_CORES];
	volatile bool changeNoticed[NUM_CORES];
	ARMInterCoreMutex mutex;
};


void ShmIrqHandler(){
	HW_HAL_Sharedmemory* shm;
	const int cpu=cpu_get_current();
	MEMORY_BARRIER;

	for(int i=0;i<MAX_NUM_SHM;i++){
		shm=shared_static_vars->shms[i];
		if(shm){
			if(shm->changed[cpu] && !shm->changeNoticed[cpu] && shm->opened[cpu]){
				shm->hal_shm[cpu]->upCallDataReady();
				shm->changeNoticed[cpu]=true;
			}
		}
	}
}

void InitShmIrq(){
	static bool init = false;
	if(!init){
		register_interrupt_routine(SW_INTERRUPT_4,&ShmIrqHandler);
		enable_interrupt(SW_INTERRUPT_4,cpu_get_current(),128);
		gic_set_irq_security(SW_INTERRUPT_4,false);
		init=true;
	}
}



HAL_Sharedmemory::HAL_Sharedmemory(Sharedmemory_IDX shmIdx) {

	shared_static_vars_init();

	shared_static_vars->shared_static_vars_mutex.lock();
	if(shared_static_vars->shms[shmIdx]==0){
		char* mem = shared_DDR_malloc(sizeof(HW_HAL_Sharedmemory));
		context = new (mem) HW_HAL_Sharedmemory();
		shared_static_vars->shms[shmIdx]=context;

		context->idx = shmIdx;
		context->shmStart = 0;
		context->shmLength = 0;
		for(int i=0;i<NUM_CORES;i++){
			context->hal_shm[i]=0;
			context->opened[i]=false;
			context->changed[i]=true;
			context->changeNoticed[i]=false;
		}
	}else{
		context = shared_static_vars->shms[shmIdx];
	}
	shared_static_vars->shared_static_vars_mutex.unlock();

	context->hal_shm[cpu_get_current()] = this;
}

void* HAL_Sharedmemory::init(int32_t * size, int32_t * maxMembers) {
	*maxMembers=NUM_CORES;

	shared_static_vars->shared_static_vars_mutex.lock();
	if(context->shmStart==0){
		context->shmStart=shared_DDR_malloc(*size);
		context->shmLength=*size;
	}else{
		*size=context->shmLength;
	}
	shared_static_vars->shared_static_vars_mutex.unlock();

	context->opened[cpu_get_current()]=true;
	InitShmIrq();
	return context->shmStart;
}


int32_t HAL_Sharedmemory::lock() {
	return context->mutex.tryLock();
}

int32_t HAL_Sharedmemory::islocked() {
	return !(context->mutex.getOwner()==0);
}

void HAL_Sharedmemory::unlock() {
	context->mutex.unlock();
}

void HAL_Sharedmemory::raiseSharedMemoryChanged() {
	uint32_t targets=0;

	for(int i=0;i<NUM_CORES;i++){
		if(context->opened[i]){
			context->changed[i]=true;
			context->changeNoticed[i]=false;
			targets |= (1<<i);
		}
	}
	MEMORY_BARRIER;
	gic_send_sgi(SW_INTERRUPT_4,targets,kGicSgiFilter_UseTargetList);
}

int32_t HAL_Sharedmemory::getMemberId() {
	return cpu_get_current();
}

bool HAL_Sharedmemory::isWriteFinished() {
	return true; //Method does not make sense for HAL_Sharedmemory
}

bool HAL_Sharedmemory::isDataReady() {
	return context->changed[cpu_get_current()];
}

void HAL_Sharedmemory::resetDataReady() {
	context->changed[cpu_get_current()] = false;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
