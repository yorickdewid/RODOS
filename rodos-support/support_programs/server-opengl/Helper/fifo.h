

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
* @file fifo.h
* @date 2008/05/11 16:13
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief FIFO (first in first out) communication (header)
*
*/

#ifndef __FIFO_H__
#define __FIFO_H__

/**
*  @class Fifo
*  @brief simple FIFO (realized as ring buffer)
*
*  A fifo where both sides (reader & writer) can work asynchronously.
*  If one side has more than one owner, the fifo has to be
*  protected using priority_ceiling, or semaphores.
*
*  @param Type    data type of fifo entries
*  @param len     maximal number of entries must be at least 2
*
*/
template < typename Type, int len > class Fifo  {

private:
  /** internal buffer */
  Type buffer[len];

  /** write index */
  volatile int writeX;

  /** read index */
  volatile int readX;

  /** advance index to next position
  * with overflow to 0 to implement a ring
  */
  int advanceIndex(int index) {
    return ((index < (len-1)) ? (index+1) : 0);
  }


public:
  /** constructor */
  Fifo() {
    readX = 0;
    writeX = 0;
  }

  /** implements the generic interface of putter */
  bool putGeneric(const unsigned int msgLen, const void* data) {
    if(msgLen > sizeof(Type)) {
      printf("fifo wrong len");
      return false;
    }
    return put(*(Type*)data);
  }

  /**  returns true == ok, false == fifo full */
  bool put(const Type& val) {
    int index =  advanceIndex(writeX);
    if(index == readX) {
       return false; /* full! */
    }

    buffer[writeX] = val;
    writeX = index;
    return true;
  }

  /** return true == ok, false = fifo empty, val not modified */
  bool get(Type& val) {
    if(readX == writeX) {
      return false;
    }
    val = buffer[readX];
    readX = advanceIndex(readX);
    return true;
  }

  ///
  int getLen() { return len; }
};


#endif /* __FIFO_H__ */
