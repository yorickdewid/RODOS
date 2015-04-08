

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
* @file thread.cc
* @date 2010/09/22 16:50
* @author Sergio Montenegro
*
* Copyright 2008 DLR
*
* A Thread is a schedulable object with own context and stack
*
* @brief %Thread handling
*/

#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
// #include <stdlib.h>
namespace RODOS {

pthread_mutex_t threadDataProtector = PTHREAD_MUTEX_INITIALIZER;

struct ThreadOnPosixContext{
	pthread_t pt;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
};

/*********** dummy signal händler für all threads ***/
void threadSigtermHandler(int sig) { } 

//List Thread::threadList = 0;
//Thread* Thread::currentThread = 0;

/** constructor */
Thread::Thread(const char* name,
               const long priority,
               const long _stackSize) :
    ListElement(threadList, name) {

    this->stackSize = _stackSize; // not used in posix

    lastActivation = 0;		// not used in posix

    // posix priorities range from 1 to 99
    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000, therfore convert best guess
    
    this->priority = priority / 10;
    if(this->priority <  1)this-> priority = 1;
    if(this->priority > 99) this->priority = 99;

    suspendedUntil = 0;		  // Ready to run
    waitingFor     = 0;		  // not waiting for any one

    ThreadOnPosixContext* ctx =  new ThreadOnPosixContext;
    pthread_mutex_init(&ctx->mutex,0);
    pthread_cond_init(&ctx->condition,0);
    context = (long*) ctx;

    
    nextBeat = END_OF_TIME ;        // no period defined
    period   = 0 ;


}

Thread::~Thread() {
    PRINTF("%s:",getName());
    ERROR("Thread deleted");
}


/** it was better do make a wrapper for the wrapper becouse
 *  the threadStartupWrapper has some implementations in assembler
 *  and I did not dare to change them to be void* (*)(void*)
 */

static bool threadsGO = false; // to wait until all threads are ready
void* posixThreadEntryPoint(void *param) {
	while(!threadsGO) sched_yield();
	threadStartupWrapper((Thread*)param);
	return 0;
}

/* called in main() after all constuctors, to create/init thread */
void Thread::create() {

	/** Posix code **/
	static pthread_attr_t pthreadCreationAttr;
	static pthread_t pt;

	pthread_attr_init(&pthreadCreationAttr);
	pthread_attr_setstacksize(&pthreadCreationAttr, stackSize);
	// pthread_attr_setpriority(&pthreadCreationAttr, priority);

	pthread_create(&pt, &pthreadCreationAttr, posixThreadEntryPoint, this);
	((ThreadOnPosixContext*)context)->pt = pt;
	// xprintf("Thread %lx context %ld\n", (long)this, (long)context);

	setPriority(priority*10); // *10 becouse of posix limitations
	taskRunning++;

	// TODO Set ptread_attr_t
	//
}


void checkSuspend(volatile int64_t* reactivationTime,pthread_cond_t* cond,pthread_mutex_t* mutex){
	int64_t now = NOW();
	int64_t hostabsoluteReactivationTime =  hwGetAbsoluteNanoseconds() + (*reactivationTime - now);

	struct timespec tp;
	tp.tv_sec = hostabsoluteReactivationTime / SECONDS;
	tp.tv_nsec = hostabsoluteReactivationTime % SECONDS;

	while(*reactivationTime > now) {
		if(*reactivationTime == END_OF_TIME){
			pthread_cond_wait(cond,mutex);
		}else{
			pthread_cond_timedwait(cond,mutex,&tp);
		}
		now = NOW();
	}
}


/** pause execution of this thread and call scheduler */
void Thread::yield() {
	//Make suspendUntil.. in genericIO work
    Thread* caller =  getCurrentThread();
    ThreadOnPosixContext* context = (ThreadOnPosixContext*)(caller->context);

    pthread_mutex_lock(&context->mutex);
    if(caller->suspendedUntil > NOW()){
    	caller->waitingFor = 0;
    	checkSuspend(&(caller->suspendedUntil),&context->condition,&context->mutex);
    }
    pthread_mutex_unlock(&context->mutex);


	sched_yield();
}

/* restore context of this thread and continue execution of this thread */
/** not used in posix?? */
// void Thread::activate() {
// currentThread = this;
// taskRunning = 1;  /* a bit to early, but no later place possible */
// }


/*******************************************************************/

/* get priority of the thread */
long Thread::getPriority() const {
    return priority*10; // rodos priorites have a larger range
}

/* set priority of the thread */
void Thread::setPriority(const long prio) {
	//
    // posix priorities range from 1 to 99
    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000, therfore convert best guess
    
    priority = prio / 10;
    if(priority <  1) priority = 1;
    if(priority > 99) priority = 99;

    pthread_t pt = ((ThreadOnPosixContext*)context)->pt;

    if(context == 0) return; // not initialized

    /** Posix code **/
    // xprintf("Setting Prio %ld for %d\n", priority, (int)pt);
    struct sched_param    param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = priority;
    pthread_setschedparam(pt, SCHED_FIFO, &param);


    /*** Only debug ***/
    // int policy;
    // xprintf("Get scheduling parameters\n");
    // pthread_getschedparam(pt, &policy, &param);
    // xprintf("priority = %d ploicy = %d\n", param.sched_priority, policy);
    /************************/

}

Thread* Thread::getCurrentThread() {
    pthread_t posixCaller = pthread_self();

    Thread* me = 0;
    while(me == 0) {
	    ITERATE_LIST(Thread, threadList) {
	    	pthread_t pthread = ((ThreadOnPosixContext*)iter->context)->pt;
	    	if( pthread == posixCaller) {
	    		me = iter;
	    		break;
	    	}
	    }
	    if(me == 0) {
		    xprintf("Posix theads %ld not ready? !\n", (long)posixCaller);
		    //yield();
		    // sleep(1);
	    }
    }
    return me;
}


/* resume the thread */
void Thread::resume() {
	ThreadOnPosixContext* c = (ThreadOnPosixContext*)context;
	pthread_mutex_lock(&c->mutex);
    suspendedUntil = 0;
    waitingFor = 0;
    pthread_cond_signal(&c->condition);
    pthread_mutex_unlock(&c->mutex);
}



/* suspend the thread */
bool Thread::suspendCallerUntil(const TTime reactivationTime, void* signaler) {

    Thread* caller =  getCurrentThread();
    ThreadOnPosixContext* context = (ThreadOnPosixContext*)(caller->context);

    pthread_mutex_lock(&context->mutex);

    caller->waitingFor = signaler;
    caller->suspendedUntil = reactivationTime;

    checkSuspend(&(caller->suspendedUntil),&context->condition,&context->mutex);

    pthread_mutex_unlock(&context->mutex);

    /** after yield: It was resumed (suspendedUntil set to 0) or time was reached ?*/
    if(caller->suspendedUntil == 0) return true; // it was resumed!
    caller->suspendedUntil = 0;
    return false; // time was reached

}




/******************************/

void Thread::initializeThreads() {
    xprintf("Threads in System:");
    ITERATE_LIST(Thread, threadList) {
        xprintf("\n   Prio = %7ld Stack = %6ld %s: ", iter->priority, iter->stackSize, iter->getName());
        iter->init();
        iter->suspendedUntil = 0;
    }
    xprintf("\n");
}

void Thread::startAllThreads() {



    ITERATE_LIST(Thread, threadList) {
        iter->create();
    }



    threadsGO = true;
}

/** non-static C++ member functions cannot be used like normal
   C function pointers. www.function-pointer.org suggests using a
   wrapper function instead. */

void threadStartupWrapper(Thread* thread) {
    Thread::currentThread = thread;
    thread->suspendedUntil = 0;

    thread->run();
    /*
      loop forever
      if run() returns this thread is to be considered terminated
    */

    while(1) {
        thread->suspendedUntil = END_OF_TIME;
        thread->yield();
    }
}


unsigned long long Thread::getScheduleCounter() {
    return Scheduler::getScheduleCounter();
}

/********************************************************************************/


/*********************************************************************************/

// not used in posix: posix has its own scheluder
//Thread* Thread::findNextToRun(TTime timeNow) { }

Thread* Thread::findNextWaitingFor(void* signaler) {
    Thread* nextWaiter = 0;

    ITERATE_LIST(Thread, threadList) {
        if (iter->waitingFor == signaler) {
	    if(nextWaiter == 0 ||
              (iter->getPriority() > nextWaiter->getPriority()) ) {
                nextWaiter = iter;
            }
        }
    }
    return nextWaiter;
}

int32_t Thread::getMaxStackUsage(){ return 0; }

}
