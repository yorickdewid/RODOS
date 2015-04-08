

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

#include <unistd.h>
//#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <xmk.h>
#include <sys/init.h>
#include <sys/process.h>
#include <semaphore.h>

#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

typedef struct thread_env {
    pthread_t pt;
    sem_t* virtex_sem;
} thread_env_t;

pthread_mutex_t threadDataProtector = PTHREAD_MUTEX_INITIALIZER;

/*********** dummy signal händler für all threads ***/
void threadSigtermHandler(int sig) { }

/** constructor */
Thread::Thread(const char* name,
               const long priority,
               const long _stackSize) :
    ListElement(threadList, name) {

    this->stackSize = _stackSize; 

    lastActivation = 0;		// not used in posix

    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000

    // As reconfiguring of Xilkernel doesn't work,
    // we have to restrict to 32 priority levels.
    // Limit range and convert the numbers as
    // 0 is highest level in Xilkernel:

    this->priority=priority;

    suspendedUntil = 0;		  // Ready to run
    waitingFor     = 0;		  // not waiting for any one
    nextBeat = END_OF_TIME ;        // no period defined
    period   = 0 ;

    // initialize the semaphore for timed wait
    context = (long*)malloc(sizeof(thread_env_t));
    ((thread_env_t*)context)->virtex_sem=(sem_t*)malloc(sizeof(sem_t));

    // allocate and initialize the stack
    void* stack = xmalloc((stackSize+7)&~7);
    stackBegin = (char*)stack;
    for (int i=0; i<stackSize; i++) {
        stackBegin[i]=0;
    }

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
    //fprintf (stderr,"ThreadEntryPoint, threadsGO is %s\n",threadsGO?"true":"false");
    while(!threadsGO) ::yield();
	threadStartupWrapper((Thread*)param);
	return 0;
}

// see http://compgroups.net/comp.arch.fpga/Microblaze-Xilkernel-and-g++
extern "C" {
void xilkernel_init ();
void xilkernel_start ();
int pthread_attr_setstack(const pthread_attr_t *attr, void
*stackaddr, size_t stacksize);
}

/* called in main() after all constuctors, to create/init thread */
void Thread::create() {

	/** Posix code **/
	static pthread_attr_t pthreadCreationAttr;
	static pthread_t pt;

    pthread_attr_init(&pthreadCreationAttr);

    // pthread_attr_setstack doesn't work
    pthreadCreationAttr.stackaddr=stackBegin;
    pthreadCreationAttr.stacksize=stackSize;

    int posix_prio;

    // xilkernel priorities range from 31 to 0
    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000, therfore convert best guess

    posix_prio = priority / 32;
    // reserve posix_prio 0 and 1 for priority ceiling and networkreader
    if(posix_prio >= 32) posix_prio = 31;
    if(posix_prio < 2) posix_prio = 2;
    posix_prio = 32-posix_prio;

    /** Posix code **/
    //    PRINTF("Creating thread with Posix Prio %d\n", posix_prio);
    struct sched_param    param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = posix_prio;
    pthread_attr_setschedparam(&pthreadCreationAttr, &param);

    pthread_create(&pt, &pthreadCreationAttr, posixThreadEntryPoint, this);

	((thread_env_t*)context)->pt = pt;

    // This is not necessery and it wouldn't work anyway
    setPriority(priority);

	taskRunning++;

}

/** pause execution of this thread and call scheduler */
void Thread::yield() {
    ::yield();
}


/*******************************************************************/

/* get priority of the thread */
long Thread::getPriority() const {
    // PRINTF("Getting Thread Priority\n");
    return priority; // rodos priorites have a larger range
                     // lower value means lower priority
}

/* set priority of the thread */
void Thread::setPriority(const long prio) {
	
    int posix_prio = 0;

    // xilkernel priorities range from 31 to 0
    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000, therfore convert best guess

    priority = prio;

    // reserve posix_prio 0 and 1 for priority ceiling and networkreader

    if ((prio == CEILING_PRIORITY) || (prio == NETWORKREADER_PRIORITY)) {
        switch (prio) {
            case CEILING_PRIORITY:
                posix_prio = CEILING_PRIORITY < NETWORKREADER_PRIORITY ? 1 : 0;
                break;
            case NETWORKREADER_PRIORITY:
                posix_prio = CEILING_PRIORITY < NETWORKREADER_PRIORITY ? 0 : 1;
                break;
            default:
                break;
        }
    }
    else {
        posix_prio = prio / 32;
        if(posix_prio >= 32) posix_prio = 31;
        if(posix_prio < 2) posix_prio = 2;
        posix_prio = 32-posix_prio;
    }

    pthread_t pt = ((thread_env_t*)context)->pt;

    /* Warning: This doesn't work during runtime using the context */

    /** Posix code **/
    // PRINTF("Changing Posix Prio to %d for %d\n", posix_prio, (int)pt);
    struct sched_param    param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = posix_prio;
    pthread_setschedparam(pt, 0, &param);

#if 0
    /*** Only debug ***/
    int policy;
    PRINTF("Get scheduling parameters\n");
    memset(&param, 0, sizeof(param));
    pthread_getschedparam(pt, &policy, &param);
    PRINTF("priority = %d ploicy = %d\n", param.sched_priority, policy);
    /************************/
#endif

}

Thread* Thread::getCurrentThread() {
    pthread_t posixCaller = pthread_self();

    Thread* me = 0;
    while(me == 0) {
	    ITERATE_LIST(Thread, threadList) {
            if(((thread_env_t*)iter->context)->pt == posixCaller) {
		 me = iter;
		 break;
	      }
	    }
	    if(me == 0) {
		    // PRINTF("Posix theads %ld not ready? !\n", (long)posixCaller);
		    yield();
	    }
    }
    return me;
}


/* resume the thread */
void Thread::resume() {

    suspendedUntil = 0;
    waitingFor = 0;

    int value;

    //PRINTF ("Resume this thread.\n");

    if ((sem_getvalue(((thread_env_t*)context)->virtex_sem,&value) == 0) &&
        (value < 0)) {
        sem_post(((thread_env_t*)context)->virtex_sem);
    }
    else {
    }
}

/* suspend the thread */
bool Thread::suspendCallerUntil(const TTime reactivationTime, void* signaler) {

    Thread* caller =  getCurrentThread();
    caller->waitingFor = signaler;
    caller->suspendedUntil = reactivationTime;

    long long now = NOW();
    long long timeDiff = reactivationTime - now;
    while(timeDiff > 0) {

        sem_timedwait(((thread_env_t*)caller->context)->virtex_sem, timeDiff / MILLISECONDS);
	    now = NOW();
	    timeDiff = caller->suspendedUntil - now;
    }

    /** after yield: It was resumed (suspendedUntil set to 0) or time was reached ?*/
    if(caller->suspendedUntil == 0) return true; // it was resumed!
    return false; // time was reached

}

void Thread::initializeThreads() {

    PRINTF ("Threads in System:");
    ITERATE_LIST(Thread, threadList) {
        PRINTF ("\n   %16s  Prio = %7ld Stack = %6ld\n", iter->getName(), iter->priority, iter->stackSize);
        iter->init();
        iter->suspendedUntil = 0;
        sem_init(((thread_env_t*)iter->context)->virtex_sem,0,0);
    }
}

void Thread::startAllThreads() {

    // PRINTF ("start all threads\n");

    ITERATE_LIST(Thread, threadList) {
        iter->create();
    }

    threadsGO = true;

    // PRINTF ("resume all threads\n");

    // wake up
    ITERATE_LIST(Thread, threadList) {
        iter->resume();
    }

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
    return;
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


/**********************************/

int32_t Thread::getMaxStackUsage(){ return 0; }

#ifndef NO_RODOS_NAMESPACE
}
#endif
