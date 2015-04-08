

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

#pragma once

#include "putter.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

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
template < typename Type, int len > class Fifo : public Putter {

protected:

    Type buffer[len];

    volatile int writeX;
    volatile int readX;

    /** advance index to next position
    * with overflow to 0 to implement a ring
    */
    int advanceIndex(int index) {
        return ((index < (len-1)) ? (index+1) : 0);
    }


public:

    Fifo() {
        readX = 0;
        writeX = 0;
    }

    /** implements the generic interface of putter */
    bool putGeneric(const long topicId, const unsigned int msgLen, const void* msg, const NetMsgInfo& netMsgInfo) {
        if(msgLen > sizeof(Type)) {
            ERROR("fifo wrong len");
            return false;
        }
        return put(*(Type*)msg);
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

    int getLen() { return len; }

    int getElementCount() { ///< warning: not absolutelly threadsafe
        int r = readX;
        int w = writeX;
        return (r <= w) ? (w-r) : (len-r+w);
    }
    int getFreeSpaceCount() { ///< warning: not absolutelly threadsafe
        return len - getElementCount() - 1;
    }
    bool isFull()  { return advanceIndex(writeX)==readX; } ///< warning: not absolutelly threadsafe
    bool isEmpty() { return readX == writeX;}              ///< warning: not absolutelly threadsafe
    void clear()   { readX = writeX = 0; }                 ///< erases all content
};

/**********************************************************************/

/**
* @class SyncFifo
* @brief A synchronized fifo.
*
* Like Fifo, but it provides syncPut which block if the fifo is
* full and syncRead which block if the fifo is empty.
* Each function may have a timeout. If no timeout is given, or timeout == 0
* it will wait util the fifo is not empty/full.
* Warning:
* Only for one writer and one reader. If more than that is required it has
* to be protected using semaphores.
*
*/
template <class Type, int len> class SyncFifo : public Fifo<Type, len> {

protected:
    Thread* suspendedReader;
    Thread* suspendedWriter;

public:

    SyncFifo() {
        suspendedReader = 0;
        suspendedWriter = 0;
    }


    /**
    * Implements the generic interface of putter:
    * Warning does not suspends if fifo full
    */
    bool putGeneric(const long topicId, const unsigned int msgLen, const void* msg, const NetMsgInfo& netMsgInfo) {
        if (msgLen > sizeof(Type)) {
            ERROR("syncFifo wrong len");
            return false;
        }

        bool ok = this->put(*(Type*)msg);
        PRIORITY_CEILING {
            if (suspendedReader!=0) {
                suspendedReader->resume();
            }
        }
        return ok;
    }

    /**  suspends while fifo is full. returns true == ok, false == timeout, */
    bool syncPut(Type &val, const int64_t timeout = 0) {
        bool ok = false;
        PRIORITY_CEILING {
            ok = this->put(val);
            if (!ok) {
                suspendedWriter = 	Thread::getCurrentThread();
                if (timeout == 0) {
                    Thread::suspendCallerUntil(END_OF_TIME);
                } else  {
                    Thread::suspendCallerUntil(NOW() + timeout);
                }
                suspendedWriter = 0; // after suspend, after resume
            }
        }

        if (!ok) { ok = this->put(val); }

        if (ok) {
            PRIORITY_CEILING {
                if(suspendedReader) {
                    suspendedReader->resume();
                }
            }
        }
        return ok;
    }

    /** return true == ok, false = fifo empty, val not modified */
    int syncGet(Type &val, const int64_t timeout = 0) {
        bool ok = false;
        PRIORITY_CEILING {
            ok= this->get(val);
            if (!ok) {
                suspendedReader = Thread::getCurrentThread();
                if(timeout == 0) {
                    Thread::suspendCallerUntil(END_OF_TIME);
                } else {
                    Thread::suspendCallerUntil(NOW() + timeout);
                }
                suspendedReader = 0;
            }
        }

        if (!ok) { ok = this->get(val); }

        if(ok) {
            PRIORITY_CEILING {
                if (suspendedWriter) {
                    suspendedWriter->resume();
                }
            }
        }
        return ok;
    }

};


template <typename Type, int len, int numOfreaders >
class MultipleReaderFifo : public Putter {

protected:

    Type buffer[len];

    volatile int writeX;
    volatile int readX[numOfreaders];
    unsigned int readerCnt; ///< used only to genereate readerId, if user wisches!

    /** advance index to next position
    * with overflow to 0 to implement a ring
    */
    int advanceIndex(int index) {
        return ((index < (len-1)) ? (index+1) : 0);
    }


public:

    MultipleReaderFifo() {
        readerCnt = 0;
        writeX = 0;
        for(int i = 0; i < numOfreaders; i++) readX[i] = 0;
    }

    int getReaderId() { //< Warning: not threade safe!!!
        int myId = readerCnt++;
        if(myId > numOfreaders) return -1; // no more readers!
        return myId;
    }

    /** implements the generic interface of putter */
    bool putGeneric(const long topicId, const unsigned int msgLen, const void* msg, const NetMsgInfo& netMsgInfo) {
        if(msgLen > sizeof(Type)) {
            ERROR("fifo wrong len");
            return false;
        }
        return put(*(Type*)msg);
    }

    /**  returns true == ok, false == fifo full */
    bool put(const Type& val) {
        int index =  advanceIndex(writeX);
        for(int i = 0; i < numOfreaders; i++) {
            if(index == readX[i]) { // one reading is to slow -> shift him!
                readX[i] = advanceIndex(readX[i]); // Warnign: thread safe?!?!
            }
        }

        buffer[writeX] = val;
        writeX = index;
        return true;
    }

    /** return true == ok, false = fifo empty, val not modified */
    bool get(Type& val, int readerId) {
        if(readerId >= numOfreaders)  return false;
        if(readX[readerId] == writeX) return false;

        val = buffer[readX[readerId]];
        readX[readerId] = advanceIndex(readX[readerId]);
        return true;
    }

    ///
    int getLen() { return len; }
};


/**
  * A Fifo that lets you write or read continius Blocks of the stored type.
  * Gives out pointers to the internal space, so you can copy data directly from/to there.
  * Useful for DMA trasnfers.
  */

template <class Type, int len>  class BlockFifo : public Fifo<Type,len> {
protected:
    int advanceIndexEx(int index,int advanceSize) {
        int newIndex = index + advanceSize;
        while(newIndex >= len) {
            newIndex -= len;
        }
        return newIndex;
    }


public:
    BlockFifo() { }


    /// returns pointer to write and maximal number of entries you may write
    Type* getBufferToWrite(int& maxLen) {
        int r = this->readX;
        int w = this->writeX;

        if(r == this->advanceIndex(w)) { //full
            maxLen =0;
            return 0;
        }

        if(r > w) { // put till r-1 possible
            maxLen = r - w - 1;
        } else { // put unitil end of buffer possible
            maxLen = len - w;
            if(r == 0) { maxLen--; }
        }
        return &(this->buffer[w]);
    }

    /// call this after you have written in the buffer you got from getBufferToWrite
    void writeConcluded(int numOfWrittenElements) {
        this->writeX = advanceIndexEx(this->writeX, numOfWrittenElements);
    }

    // returns pointer to read and maximal number of entries you may read
    Type* getBufferToRead(int& maxLen) {
        int r = this->readX;
        int w = this->writeX;

        if(r == w) { //empty
            maxLen =0;
            return 0;
        }

        if(r < w) { // get till w-1 possible
            maxLen = w - r;
        } else { // get unitil end of buffer possible
            maxLen = len - r;
        }
        return &(this->buffer)[r];
    }

    /// call this after you have read in the buffer you got from getBufferToRead
    void readConcluded(int sizeRed) {
        this->readX=advanceIndexEx(this->readX,sizeRed);
    }

};


#ifndef NO_RODOS_NAMESPACE
}
#endif

