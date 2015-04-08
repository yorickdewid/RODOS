
/* 
 * File:   main.cpp
 * Author: gabriel reimers 2012
 *
 * Created on November 10, 2011, 12:37 PM
 */

#include "FileSystem.h"
#include "File.h"
#include "Directory.h"
#include <stdio.h>
#include <string.h>
#include "fat_fs/ff.h"
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

    xprintf("main > memory usage of FileSystem: %ld\n", (sizeof(FileSystem) + sizeof(FATFS)));
    xprintf("main > memory usage of File: %ld\n", sizeof(File) + sizeof(FIL));
    xprintf("main > memory usage of Directory: %ld\n", sizeof(Directory) + sizeof(DIR));

    xprintf("\n////////////////////////////\n"
            "main > TESTING INIT AND MOUNT\n"
            "//////////////////////\n");

    xprintf("main > test output %lld\n", (long long)1);

    fileSystem->enableDebugOutput(true);

    //disk must be mounted before anything else can be done
    long long t0 = NOW() / 1000000;
    fileSystem->mountDisk(0);
    long long t1 = NOW() / 1000000;
    xprintf("main > mount done in %lldms\n", t1 - t0);

    //format will create a new fat file system
    //this can be omitted if there already is a fs on the disk
    t0 = NOW() / 1000000;
    fileSystem->formatDisk(0);
    t1 = NOW() / 1000000;
    xprintf("main > format done in %lldms\n", t1 - t0);


    //setting time changes RODOS system time
    fileSystem->setTime(2012, 03, 03, 07, 31, 12);




    xprintf("\n\n////////////////////////////\n"
            "main > FILE FUNTIONS\n"
                 "///////////////////////////\n");
    file->enableDebugOutput(true);
    
    t0 = NOW() / 1000000;
    file->open("/test.txt", file->O_WRITE);
    file->putString("This is my first file!\n");
    file->close();
    t1 = NOW() / 1000000;
    xprintf("main > create '/test.txt' done in %lldms\n", t1 - t0);

    
    
    //create a big file
    char fillString[] = "To start press any key... Where's the 'any' key? "
    "I see Esc, Catarl, and Pig Up. There doesn't seem to be any 'any' key. "
    "Wo!! All this computer hacking is making me thirsty. I think I'll order a 'Tab'.";
    t0 = NOW() / 1000000;
    file->open("/2mb.txt", file->O_WRITE); 
    
    //it is much faster if we tell the file system beforehand
    //how big this file will roughly be
    file->lseek(2000000); //increase file size to 2mb
    file->lseek(0); //set read write point back to 0
    //if the file has to be increased with each write command the fat table has
    //to be updated with each write. this is very slow.
    
    file->sync();
    int i = 0;
    for(i = 0; i < 10000; i++) {
            //fillString has 200bytes
        //write it 10000 times -> 2MB
        //xprintf("write 200b\n");
        file->write(fillString, 200);
    }
    
    file->close();
    t1 = NOW() / 1000000;
    xprintf("main > create '/2mb.txt' done in %lldms\n", t1 - t0);
    
    
    /////////////////READING
    
    
    t0 = NOW() / 1000000;
    file->open("/2mb.txt", file->O_READ);    
    for(i = 0; i < 10000; i++) {
            //fillString has 200bytes
        //read it 10000 times -> 2MB
        file->read(fillString, 200);
    }
    file->close();
    t1 = NOW() / 1000000;
    xprintf("main > read '/2mb.txt' done in %lldms\n", t1 - t0);
    
    
        t0 = NOW() / 1000000;
    file->open("/test.txt", file->O_READ);
    char getBuf[50];
    file->getString(getBuf, 50);
    file->close();
   t1 = NOW() / 1000000;
    xprintf("main > open '/test.txt' for read done in %lldms\n", t1 - t0);    
    xprintf("main > getstring '/test.txt': \n%s\n", getBuf);


  while(1) {
       suspendCallerUntil();
     }
}

};

Fatmos fatmos;
