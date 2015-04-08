/* 
 * File:   RingBuffer.cpp
 * Author: gaby
 * 
 * Created on April 17, 2012, 4:45 PM
 */

#include <string.h>
#include <stdlib.h>

#include "RingBuffer.h"
#include "fat_fs/diskio/ram/flashio.h"
#include "fat_fs/diskio/ram/setup.h"
#include "fat_fs/typeDefs.h"
#include "rodos.h"

RingBuffer::RingBuffer() {
    readPageBuffer = xmalloc(PAGE_TOTAL_BYTE_SIZE);
    writePageBuffer = xmalloc(PAGE_TOTAL_BYTE_SIZE);

    readPageIndex = 0;
    readByteOffset = 0;

    writePageIndex = 0;
    writeByteOffset = 0;
    bufferdPageIndex = -1;
    dataSize = 0;
}

int RingBuffer::write(void* buf, int numOfBytes) {
    int freeBytesInBuffer = PAGE_BYTE_SIZE - writeByteOffset;
    if (freeBytesInBuffer > numOfBytes) {
        memcpy(&writePageBuffer[writeByteOffset], buf, numOfBytes);
        writeByteOffset += numOfBytes;
        dataSize += numOfBytes;
    } else {
        //does not fit in buffer -> write buffer full
        memcpy(&writePageBuffer[writeByteOffset], buf, freeBytesInBuffer);
        programPage((BYTE*) writePageBuffer, writePageIndex);
        writePageIndex += 1;
        writeByteOffset = 0;
        dataSize += freeBytesInBuffer;


        if (writePageIndex >= TOTAL_NO_OF_PAGES) {
            //wrap around
            writePageIndex = 0;
        }

        if ((readPageIndex - writePageIndex) <= PAGES_PER_BLOCK) {
            //reached read pointer -> erase read page index and move
            //read further so we can overwrite the existing data
            int readBlockIndex = (readPageIndex / PAGES_PER_BLOCK);
            eraseBlock(readBlockIndex);
            readPageIndex = (readBlockIndex + 1) * PAGES_PER_BLOCK;
            if (readPageIndex >= TOTAL_NO_OF_PAGES) {
                readPageIndex = 0;
            }
        }
    }

    if (freeBytesInBuffer < numOfBytes) {
        //not all bytes could be written to buffer -> repeat with remaining
        this->write(&buf[freeBytesInBuffer], numOfBytes - freeBytesInBuffer);
    }

    return dataSize;
}

int RingBuffer::read(void* buf, int numOfBytes) {
    if (dataSize < numOfBytes) {
        xprintf("RingBuffer > read > could not read. "
                "buffer has only %d bytes of data but %d were requested\n",
                dataSize, numOfBytes);
        return dataSize;
    }

    if (readPageIndex == writePageIndex) {
        memcpy(readPageBuffer, writePageBuffer, PAGE_BYTE_SIZE);
    } else if (readPageIndex != bufferdPageIndex) {
        readPage((BYTE*) readPageBuffer, readPageIndex);
    }
    bufferdPageIndex = readPageIndex;

    int bytesInBuffer = PAGE_BYTE_SIZE - readByteOffset;
    if (bytesInBuffer > numOfBytes) {
        //all data can be read from buffer
        memcpy(buf, &readPageBuffer[readByteOffset], numOfBytes);
        readByteOffset += numOfBytes;
        dataSize -= numOfBytes;
    } else {
        //not all data in buffer -> first return available
        memcpy(buf, &readPageBuffer[readByteOffset], bytesInBuffer);
        readPageIndex += 1;
        readByteOffset = 0;
        dataSize -= bytesInBuffer;


        if (readPageIndex >= TOTAL_NO_OF_PAGES) {
            //wrap around
            readPageIndex = 0;
        }
    }

    if (bytesInBuffer < numOfBytes) {
        //if not all bytes could be read from buffer -> repeat with remaining
        this->read(&buf[bytesInBuffer], numOfBytes - bytesInBuffer);
    }

    return dataSize;
}