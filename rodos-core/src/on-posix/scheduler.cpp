

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


/**
* @file scheduler.cc
* @date 2010/09/24 18:10
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* @brief priority based scheduler
*
*/
#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"

#include <unistd.h>
#include <pthread.h>
#include <signal.h>

namespace RODOS {
/** count all calls to the scheduler */
unsigned long long Scheduler::scheduleCounter=0;


pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t interruptDisableCounter_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int interruptDisableCounter=0;

void sigio_handler(int a);
void timerSignalHandler(int ignore);
void sharedmemorySignalHandler(int signum);

void signalprocessLoop(){
	int signalnum;
	sigset_t signalsToProcess;
	sigemptyset(&signalsToProcess);
	sigaddset(&signalsToProcess,SIGIO);
	sigaddset(&signalsToProcess,SIGALRM);
	sigaddset(&signalsToProcess,SIGUSR1);


	while(1){
		signalnum = sigwaitinfo(&signalsToProcess,0);

		pthread_mutex_lock(&signal_mutex);
		switch(signalnum){
		case SIGIO:
			sigio_handler(signalnum);
			break;
		case SIGUSR1:
			sharedmemorySignalHandler(signalnum);
			break;
		case SIGALRM:
			timerSignalHandler(signalnum);
			break;
		default:
			xprintf("Warning: Got unknown signal: %d\n",signalnum);
		}
		pthread_mutex_unlock(&signal_mutex);
	}
}



void hwDisableInterrupts(){
	pthread_mutex_lock(&interruptDisableCounter_mutex);
		interruptDisableCounter++;
		if(interruptDisableCounter==1){
			pthread_mutex_lock(&signal_mutex);
		}
	pthread_mutex_unlock(&interruptDisableCounter_mutex);
}

/**
  *  global interrupt enable
  */
void hwEnableInterrupts(){
	pthread_mutex_lock(&interruptDisableCounter_mutex);
		interruptDisableCounter--;
		if(interruptDisableCounter <0){
			ERROR("called hwEnabeldInterrupts without corresponding hwDisableInterrupt\n");
		}
		if(interruptDisableCounter==0){
			pthread_mutex_unlock(&signal_mutex);
		}
	pthread_mutex_unlock(&interruptDisableCounter_mutex);
}


void schedulerWrapper(long* ctx) {
}

/** activate idle thread */
void Scheduler::idle() {

	// Disable signals in all Threads
	// We handle signals synchronously in this(idle) thread
	sigset_t newset;
	sigfillset(&newset);
	sigdelset(&newset,SIGINT);// keep CTRL-C working
	pthread_sigmask(SIG_SETMASK,&newset,0);

	Thread::startAllThreads();
	Timer::start();

	signalprocessLoop();

	while(1) sleep(0xffffffff); // while(1) becouse timerinterups terminaes the sleep
}

// Not used in posix, becouse posix hast its own schedulder
// void Scheduler::schedule() { }

unsigned long long Scheduler::getScheduleCounter() {
  return scheduleCounter;
}


/** returns true if scheduler is considered to be running */
bool Scheduler::isSchedulerRunning() {
 return (taskRunning >  0);
}
}
