
/* 
 * File:   main.cpp
 * Author: gabriel reimers 2012
 *
 * Created on November 10, 2011, 12:37 PM
 */

#include "filesystem.h"
#include <stdio.h>
#include <string.h>

#include "rodos.h" 


//global allocation to more easily keep track on the memory we use
//this can of course also be done locally (directly in main) or with alloc
FileSystem fs_;
File f_;

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
    FileSystem* fileSystem = &fs_;
    File* file = &f_;


    xprintf("\n////////////////////////////\n"
            "main > TESTING INIT AND MOUNT\n"
            "//////////////////////\n");

    //disk must be mounted before anything else can be done
    fileSystem->mountDisk(0);
    xprintf("main > mount done\n");

    //format will create a new fat file system
    //this can be omitted if there already is a fs on the disk
    fileSystem->formatDisk(0);
    xprintf("main > format done\n");


    //setting time changes RODOS system time
    fileSystem->setTime(2012, 03, 03, 07, 31, 12);



    xprintf("\n\n////////////////////////////\n"
            "main > FILE FUNTIONS\n"
                 "///////////////////////////\n");

    file->open("./test.txt", file->O_WRITE);
    xprintf("main > open '/test.txt' done\n");

    file->putString("This is my first file!\n");
    xprintf("main > put string done\n");

    long t = file->tell();
    xprintf("main > tell '/test.txt': %ld\n", t);
    //to append a string we have to go back 1 byte because a \0 has already been written
    file->lseek(t - 1);
    xprintf("main > seek '/test.txt' to %ld - 1\n", t);

    char buf[6];
    xsprintf(buf, "HELLO");
    file->write(buf, 6);
    xprintf("main > writing HELLO to '/test.txt'\n");

    file->sync();
    xprintf("main > sync '/test.txt' done\n");

    file->lseek(t - 7);
    file->putChar('C');
    file->putChar('H');

    xprintf("main > put char CH to '/test.txt' done\n");

    file->close();
    xprintf("main > close '/test.txt' for write done\n");

    


    /////////////////READING


    file->open("./test.txt", file->O_READ);
    xprintf("main > open '/test.txt' for read done\n");

    char getBuf[50];
    file->getString(getBuf, 50);
    xprintf("main > getstring '/test.txt': \n%s\n", getBuf);


    char readBuf[50];
    file->lseek(0);
    file->read(readBuf, 50);
    readBuf[49] = 0;//end string
    xprintf("main > read 50 from '/test.txt': \n%s\n", readBuf);

    int err = file->errorNo();
    xprintf("main > get error number '/test.txt': \n%d\n", err);

    file->close();
    xprintf("main > close '/test.txt' for read\n");


  while(1) {
       suspendCallerUntil();
     }
}

};

Fatmos fatmos;
