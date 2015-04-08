

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



#pragma once


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
     RingBuffer administrates a pool of objects in a ring manner (Template class).

     with getWriteBuffer/endWrite you can rewrite the oldest entry in the buffer.
     with getfromRing you get cyckick one for one, all written entries
     in the buffer.

     Thread-safe for one writer and one reader.

     @param      t Class to be adrministrated (Template)
     @param      poolSize how many elements are to be allocated

     @author     Sergio Montenegro
     @version    Sept. 1999, March 2000, Feb 2009


*/

template <class t, uint32_t poolSize>
class RingBuffer {
public:
    uint32_t   writeIndex;
    uint32_t   readIndex;
    uint32_t   currentWrite;
    t          vals[poolSize];

public:

    /// How often it was written
    uint64_t writeCnt;
    /// How often it was readed
    uint64_t readCnt;
    /// How many objects are in FIFO ( == writeCnt - readCnt)
    uint32_t occupiedCnt;

    ///
    RingBuffer() {
        readCnt = 0;
        writeCnt = 0;
        writeIndex = 0;
        readIndex  = 0;
        occupiedCnt = 0;
        currentWrite = -1;
    }


    void put(t& newdata) {
        currentWrite = writeIndex;
        vals[writeIndex] = newdata;
        writeIndex++;
        if (writeIndex  >= poolSize) writeIndex = 0;
        if (occupiedCnt <  poolSize) occupiedCnt++;
        writeCnt++;
        currentWrite = -1;
    }

    /// get the next
    void get(t& fromRing) {

        /** Jump the current being written record **/
        if (readIndex == currentWrite) readIndex++;
        if (readIndex >= poolSize) readIndex = 0;

        /** readIndex should not pass write index if there is no data after it */
        if (writeCnt < poolSize && readIndex >= writeIndex) readIndex = 0;

        fromRing = vals[readIndex];
        readIndex++;
        if (readIndex >= poolSize) readIndex = 0;

        if (occupiedCnt > 0) occupiedCnt--;
        readCnt++;
    }

    ///
    int getLen() { return poolSize; }

};



#ifndef NO_RODOS_NAMESPACE
}
#endif

