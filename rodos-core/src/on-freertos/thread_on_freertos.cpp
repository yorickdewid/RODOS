

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
* A Thread is a schedulable object with own context and stack
* In FreeRTOS a Thread corresponds to a task.
*
* @brief %Thread handling
*/

#include "rodos.h"
#include "scheduler.h"
#include "hw_specific.h"

// FreeRTOS includes
#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
#include "task.h"      // Tasks and Scheduler


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/*********** dummy signal handler for all threads ***/
void threadSigtermHandler(int sig) { } 

/********************************************************************************/

/** constructor */
Thread::Thread(const char* name,
               const long priority,
               const long _stackSize) :
    ListElement(threadList, name) {

    this->stackSize = _stackSize / sizeof(int); // TODO Size in FreeRTOS to be expressed in words 

    lastActivation = 0;		  // not used in posix

    // FreeRTOS priorities range from 1 to configMAX_PRIORITIES (nominally 7) 
    // rodos priorities range from 0 to 2G
    // normaly user uses from 0 to 1000, therfore convert best guess
    this->priority = priority / configMAX_PRIORITIES;
    if (this->priority <  1) {
      // Priority 0 is for idle threads
      this-> priority = 1;
    }
    if(this->priority > (long) configMAX_PRIORITIES) {
      this->priority = (long) configMAX_PRIORITIES;
    }

    suspendedUntil = 0;		  // Ready to run
    waitingFor     = 0;		  // not waiting for any one
    context = 0;                  // used for handler
    
    nextBeat = END_OF_TIME ;      // no specific time to wake up
    period   = 0 ;                // no period defined yet

}
/********************************************************************************/


Thread::~Thread() {
    PRINTF("%s:",getName());
    ERROR("Thread deleted");
}
/********************************************************************************/


/** Non-static C++ member functions cannot be used like normal
 * C function pointers. www.function-pointer.org suggests using a
 * wrapper function instead. 
 */
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
/********************************************************************************/



/** It was better do make a wrapper for the wrapper because
 *  the threadStartupWrapper has some implementations in assembler
 *  and I did not dare to change them to be void* (*)(void*)
 */
static bool threadsGO = false; // to wait until all threads are created

void freeRtosEntryPoint(void *param) {
  /** At this point we have an entry point, but we don't start the task 
   * before not all tasks are properly created.
   */
  while(!threadsGO) {
    // FreeRTOS - give room to another task, the scheduler will handle
    taskYIELD();
  }
  threadStartupWrapper((Thread*) param);
  //return 0;
}
/********************************************************************************/


/** Called in main() after all constuctors, to create/init thread */
void Thread::create() {
  portBASE_TYPE taskCreated;     // flag to indicate whether creation was successful
  xTaskHandle createdTaskHandle; // handle to the current task created 

  /** FreeRtos code **/
  taskCreated = 
    xTaskCreate(      
		freeRtosEntryPoint, // this->run() Pointer to function executed during run
		(signed char *) this->name, // name of the thread
		(unsigned short) this->stackSize, // Size to be expressed in words 
		this, 		// pointer to Thread class as startup function parameter 
		this->priority, // Max_PRIORITY is smaller here 
		&createdTaskHandle); // handle to task, needed to access internal fields

  // TODO
  //xprintf("Priority for thread %s set to %ld\n", this->name, this->priority);

  // Was task successfully created?
  if (taskCreated != pdPASS) {
    ERROR("FreeRTOS task could not be created!");
  }
  
  // Use the context pointer in order to store the handle TODO Verify that there is no conflict
  this->context = (long*) createdTaskHandle;

  taskRunning++;   // Task is still alive

}
/********************************************************************************/


/** pause execution of this thread and call scheduler */
void Thread::yield() {
  // FreeRTOS
  // TODO cannot be called before scheduler has been started
  taskYIELD();
}

/* restore context of this thread and continue execution of this thread */
/** not used in posix?? */
// void Thread::activate() {
// currentThread = this;
// taskRunning = 1;  /* a bit to early, but no later place possible */
// }

/********************************************************************************/


/* Get priority of the thread.
 * TODO Is there a need for scaling?
 */
long Thread::getPriority() const {

  // FreeRTOS unsigned portBASE_TYPE uxTaskPriorityGet( xTaskHandle pxTask )
  return (long) uxTaskPriorityGet( (xTaskHandle) this->context); 
}
/********************************************************************************/


/** Set priority of the thread.  
 * Since FreeRTOS uses smaller values for
 * priorities, we need to check for maximumm priorities.  However scaling in
 * thiis place has to be avoided. It is up to the user to handle and monitor
 * his priorities relative to the priorities used in the system.
 */
void Thread::setPriority(const long prio) {

  long newPriority = prio; // Scaling with configMAX_PRIORITIES will not work
  if (newPriority > (long) configMAX_PRIORITIES) {
      newPriority = (long) configMAX_PRIORITIES;
  }

  // FreeRTOS void vTaskPrioritySet( xTaskHandle pxTask, unsigned portBASE_TYPE uxNewPriority )
  vTaskPrioritySet((xTaskHandle) this->context, (unsigned portBASE_TYPE) newPriority);
}
/********************************************************************************/


/**
 * Return the handle of the calling task.
 */
Thread* Thread::getCurrentThread() {
  Thread* currentThread = 0;
  // FreeRTOS xTaskHandle xTaskGetCurrentTaskHandle( void );
  xTaskHandle currentTask = xTaskGetCurrentTaskHandle();
  
  // Look up threads and look whether the taskHandle, stored in context matches.
  while (currentThread == 0) {
    ITERATE_LIST(Thread, threadList) {
      if(iter->context == (void*) currentTask) {
	currentThread = iter;
	break;
      }
    } 
    if (currentThread == 0) {
      // The thread list is still empty.
      xprintf("FreeRTOS task %ld not assigned yet not ready? !\n", (long) currentTask);
      taskYIELD();
    }
  }
  return currentThread;
}
/********************************************************************************/


/* Resume thread */
void Thread::resume() {
  // FreeRTOS void 
  vTaskResume( (xTaskHandle) context);
}
/********************************************************************************/


/* Suspend thread to a specific time */
bool Thread::suspendCallerUntil(const TTime reactivationTime, void* signaler) {

  Thread* caller =  getCurrentThread(); // pointer to the current thread, to store parameter
  // xTaskHandle currentTask = xTaskGetCurrentTaskHandle // TODO a pointer to the current task
  caller->waitingFor = signaler;
  // Use absolute time from begin in order to avoid offsets occuring in case relative time is used.
  caller->suspendedUntil = reactivationTime;

  long long now = NOW(); // time in nano seconds is provided
  long long timeDiff;
  // Suspend only if the time of waking up is not allready passed by.
  if (caller->suspendedUntil > now) {
    /** Call to FreeRTOS.
     * First convert times between current time and ticks.
     * Use the time difference to reschedule the task.
     * TODO At this point I assume the caller is identical with the current running task.
     * Actually we are in a somewhat messy state.
     */
    timeDiff = reactivationTime - now;
    // Scale time from nano seconds in Rodos to ticks in FreeRTOS 
    portTickType timeToWait = (long)(timeDiff / (portTICK_RATE_MS * MILLISECONDS ) );
    
    /*xprintf("Delayed thread until %lld by %lld (%ld ticks), time now is %lld \n", 
	    caller->suspendedUntil, timeDiff, timeToWait, now); */
    vTaskDelay( (portTickType) timeToWait); //xTicksToDelay 
  }

  /** after yield: It was resumed (suspendedUntil set to 0) or time was reached ?*/
  if(caller->suspendedUntil == 0) {
    return true; // it was resumed!
  }
  return false; // time was reached

}
/********************************************************************************/


void Thread::initializeThreads() {
    xprintf("Threads in System:");
    ITERATE_LIST(Thread, threadList) {
        xprintf("\n   %s  Prio = %7ld Stack = %6ld : ", 
		iter->getName(), iter->priority, iter->stackSize);
        iter->init();
        iter->suspendedUntil = 0;
    }
    xprintf("\n");
    // RTOS void vTaskList( signed char *pcWriteBuffer )

}
/********************************************************************************/


void Thread::startAllThreads() {
    // Called from scheduler 'in order to prepare threads before start
    ITERATE_LIST(Thread, threadList) {
        iter->create();
    }
    /// All threads are created, now the scheduler can be started.
    threadsGO = true;
    // Call FreeRTOS Scheduler to start threads.
    vTaskStartScheduler();

}
/********************************************************************************/


unsigned long long Thread::getScheduleCounter() {
    return Scheduler::getScheduleCounter();
}
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

#ifndef NO_RODOS_NAMESPACE
}
#endif

