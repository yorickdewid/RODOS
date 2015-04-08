

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
* @file hw_specific.h
* @date 2008/04/22 16:54
*
* Copyright 2008 DLR
*
* @brief all hardware specific stuff
*
* Each new hw platform shall provide this interface
*
*/

#ifndef __HW_SPECIFIC_H__
#define __HW_SPECIFIC_H__


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/***********************************/

void hwInit();	///< initilisation of all hw interfaces, fpgas, devices, etc.

/** get time, time unit is nanoseconds, time 0 = startup */
unsigned long long hwGetNanoseconds();
unsigned long long hwGetAbsoluteNanoseconds();	///< from host, eg for random generators
void hwInitTime();	         ///< Initialize the time (eg. time 0)

void hwResetAndReboot();	///<  End of Programm,

/** Allows another Space/Time partition to work, eg
 ** another linux process in linux, or another partition
 ** in an ARIC OS. If standalone then just nop()
 **/
void sp_partition_yield();


/***********************************/


/* 
 * Wrapper function to enable the call to the Tread::run() method 
 * (in C++) from the task handler (in C).  
 */
class Thread;
extern void threadStartupWrapper(Thread*);


/** create context on stack and return a pointer to it */
long* hwInitContext(long* stack, void* object);

/** Node number if a network system **/
extern long getNodeNumber();


/**
* @class Timer
* @brief Class for interval timer management.
*
*/
class Timer {

private:
  /**
   * Time period in which the timer is activated.
   */
  static long long microsecondsInterval;

  /** 
   * Constructor, no specific action to peform.
   */
  Timer();

public:
  /**
   * Setup signal handlers for timer signal management.
   */
  static void init();

  /**
   * Destructor, left empty.
   */
  ~Timer();

  /**
   * Start timer signal generation, setting time period set in setInterval().
   */
  static void start();

  /**
  * Stop timer signal generation.
  */
  static void stop();

  /**
  *  Set timer interval to the given value.
  */
  static void setInterval(const long long microsecondsInterval);
};

extern long volatile taskRunning;


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* __HW_SPECIFIC_H__ */
