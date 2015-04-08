

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
 * @file commbuffer.h
 * @date 2008/05/25 16:20
 * @author Sergio Montenegro, Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * @brief simple buffer for asynchronous communication (header)
 */

#pragma once

#include "putter.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @class CommBuffer
 * @brief A simple buffer for asynchronous communication
 *
 * CommBuffer can be used to interchange data asynchronously.
 * both sides (reader/writer) can work simultaneously and the data
 * will be allways consistent.
 * There can be only one reader and only one writer.
 * Eachtime the reader reads, it gets the latest data written.
 * Due to fast switching between the buffers, the delay in simultaneous access
 * is only short.
 * @param Type Type of the data to synchonize by CommBuffer
 */

template <typename Type> class CommBuffer : public Putter {

private:
    /// Data area consisting of three independent parts to avoid access conflicts.
    Type buffer[3];
    /// Pointer to buffer area that is currently written or will be written next.
    Type* writer;
    /// Pointer to buffer where last data is stored.
    Type* reader;
    /// Additional buffer, to avaoid conflicts between simultaneous read-write access.
    Type* intermediate;
    /** Flag to protect buffer. It indicates access to the current read buffer 
     * in order to prevent data conflicts.
     */
    volatile bool readingNow;

public:
  uint64_t writeCnt;
  uint64_t readCnt;
  /** Constructor */
  CommBuffer() {
    writer = &buffer[0];
    reader = &buffer[1];
    intermediate = &buffer[2];
    readingNow = false;
    writeCnt = readCnt = 0;
  }

  /**
   * Implements the generic interface of class Putter.
   * Put methods should only used by one thread.
   * @param msgLen Length of the message.
   * @param data Pointer to the message data to put.
   * @return true if the put operation is successful, else false. If the
   * msgLen doesn't match the size of the template type the method is never
   * successful.
   */
  bool putGeneric(const long topicId, const unsigned int len, const void* msg, const NetMsgInfo& netMsgInfo) {
    if(len > sizeof(Type)) {
      ERROR("combuffer wrong len");
      return false;
    }
    put(*(Type*)msg);
    return true;
  }

  /**
   * Implements the the put operation of data to the CommBuffer. The data is
   * stored in the buffer, and swap read and write buffer for next write.
   * Put methods should only used by one thread.
   * @param data Reference of the message data to put.
   */
  void put(Type& data) {
    *writer = data;

    /* Swap reader <-> writer in order to make them read in the next call to get. */
    Type* swap = reader;
    reader = writer;
    writer = swap;

    /* Buffer being read now? Then take the intermediate buffer for the next write. */
    if(readingNow) {
      swap = writer;
      writer = intermediate;
      intermediate = swap;
    }
    writeCnt++;
  }

  /**
   * Get the next consistent message from the CommBuffer.
   * The method get should only used by one thread.
   * @param data Reference of the message data to get.
   */
  void get(Type &data) {
    readingNow = true;
    Type* readerTmp = reader; // This is assumed to be atomar
    data = *readerTmp;		  // this is not atomar
    readingNow = false;
    readCnt++;
  }
 
 /** like get, but modifiy out parameter only if there is new data **/

  bool getOnlyIfNewData(Type &data) {
    if(writeCnt > readCnt) {
       get(data);
       readCnt = writeCnt;
       return true;
    }
    return false;
  }
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

