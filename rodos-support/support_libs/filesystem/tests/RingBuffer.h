/* 
 * File:   RingBuffer.h
 * Author: gaby
 *
 * Created on April 17, 2012, 4:45 PM
 */

#ifndef RINGBUFFER_H
#define	RINGBUFFER_H

class RingBuffer {
public:
    RingBuffer();

    //returns number of bytes left in buffer
    int write(void* buf, int numOfBytes);
    int read(void* buf, int numOfBytes);

private:

    int readPageIndex;
    int readByteOffset;
    void* readPageBuffer;
    int bufferdPageIndex;

    int writePageIndex;
    int writeByteOffset;
    void* writePageBuffer;

    int dataSize;

};

#endif	/* RINGBUFFER_H */

