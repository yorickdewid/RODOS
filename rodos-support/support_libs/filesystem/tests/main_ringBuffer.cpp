/* 
 * File:   main.cpp
 * Author: gabriel reimers 2012
 *
 * Created on November 10, 2011, 12:37 PM
 */

#include "RingBuffer.h"

#include <stdio.h>
#include <string.h>

#include "rodos.h" 


//global allocation to more easily keep track on the memory we use
//this can of course also be done locally (directly in main) or with alloc
RingBuffer rb_;

static Application module02("FATMOS", 3000);

class Fatmos: public Thread {
public:
  Fatmos(): Thread("FATMOS", 20, 10000) {
  }

  void init() {
    //
  }


 void run() {
    //we prefer to have pointers so we create these:
    RingBuffer* ringBuffer = &rb_;



    xprintf("\n////////////////////////////\n"
            "main > Test RingBuffer\n"
            "////////////////////////////\n");

    char* s = "Hi, this is a ringbuffer, ring buffer ring buffer";
    int dataSize = ringBuffer->write(s, strlen(s)+1);
    xprintf("data in buffer: %d\n", dataSize);

    char s2[100];
    dataSize = ringBuffer->read(s2, 20);
        xprintf("read:\n%s\ndata in buffer: %d\n", s2, dataSize);

  
  while(1) {
       suspendCallerUntil();
     }
}

};

Fatmos fatmos;
