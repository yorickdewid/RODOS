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
            "main > INIT AND MOUNT\n"
            "////////////////////////////\n");

    fileSystem->enableDebugOutput(true);

    //disk must be mounted before anything else can be done
    fileSystem->mountDisk(0);
    xprintf("main > mount done\n");

    //format will create a new fat file system
    //this can be omitted if there already is a fs on the disk
    fileSystem->formatDisk(0);
    xprintf("main > format done\n");


    long freeSpace = fileSystem->freeSpace(0);
    long totalSpace = fileSystem->totalSpace(0);
    xprintf("main > free space %ld MB total space %ld MB\n", (long) (freeSpace / 1000000.0), (long) (totalSpace / 1000000.0));

    //setting time changes RODOS system time
    //if no time is set FAT starts 1980-01-01
    fileSystem->setTime(2012, 03, 03, 07, 31, 12);




    xprintf("\n\n////////////////////////////\n"
            "main > WORKING WITH FILES\n"
            "///////////////////////////\n");
    file->enableDebugOutput(true);
    xprintf("main > create file '/test.txt'\n");
    file->open("/test.txt", file->O_WRITE);
    file->putString("Be excellent to each other. And party on, dudes. -- Bill & Ted");
    file->close();


    //this will not work because it does not create intermediate directories
    xprintf("main > create dir '/FirstDir/ScndDir'\n");
    fileSystem->createDirectory("/FirstDir/ScndDir");
    bool isDir;
    bool exists = fileSystem->fileExists("/FirstDir/ScndDir", &isDir);
    if (!exists) {
        xprintf("main > directory '/FirstDir/ScndDir' does not exist -> correct\n");
    } else {
        xprintf("main > directory '/FirstDir/ScndDir' exists -> NOT CORRECT\n");
    }
    xprintf("main > create dir '/FirstDir'\n");
    fileSystem->createDirectory("/FirstDir");

    xprintf("main > create dir '/FirstDir/ScndDir'\n");
    fileSystem->createDirectory("/FirstDir/ScndDir");
    exists = fileSystem->fileExists("/FirstDir/ScndDir", &isDir);
    if (!exists) {
        xprintf("main > directory '/FirstDir/ScndDir' does not exist -> NOT CORRECT\n");
    } else {
        xprintf("main > directory '/FirstDir/ScndDir' exists -> correct\n");
    }

    xprintf("main > create file '/FirstDir/readme.txt'\n");
    file->open("FirstDir/readme.txt", file->O_WRITE);
    file->putString("Welcome to the readme.");
    file->close();

    xprintf("main > move file 'test.txt' to /FirstDir/readme.txt'\n");
    //this this will not work as a file with this name already exists
    fileSystem->move("/test.txt", "/FirstDir/readme.txt");

    xprintf("main > move file 'test.txt' to /FirstDir/read.txt'\n");
    //this will work. the file can be moved
    fileSystem->move("/test.txt", "/FirstDir/read.txt");
    xprintf("main > moved file > %s to %s\n", "test.txt", "FirstDir/read.txt");
    exists = fileSystem->fileExists("FirstDir/read.txt", &isDir);
    if (exists) {
        xprintf("main > file  read.txt exists > correct\n");
    } else {
        xprintf("main > file  read.txt does not exists > ERROR\n");
    }

    if (isDir) {
        xprintf("main > file  read.txt is dir > ERROR\n");
    } else {
        xprintf("main > file  read.txt is not dir > correct\n");
    }


    fileSystem->copy("FirstDir/readme.txt", "/copy.txt");
    xprintf("main > copied file  'FirstDir/readme.txt' to '/copy.txt'\n");


    long fileSize = fileSystem->getFileSize("/copy.txt");
    xprintf("main > '/copy.txt' has %ld bytes\n", fileSize);



    int y, m, day, h, min, s;
    fileSystem->getModificationDate("/copy.txt", &y, &m, &day, &h, &min, &s);
    xprintf("main > '/copy.txt' was modified %d-%d-%d %d:%d:%d\n", y, m, day, h, min, s);

    fileSize = fileSystem->getFileSize("/FirstDir");
    xprintf("main > '/FirstDir' has %ld bytes (dirs are always 0)\n", fileSize);


    fileSystem->remove("/FirstDir");
    xprintf("main > filesytem removed '/FirstDir/' and all contents\n");

    exists = fileSystem->fileExists("/FirstDir/readme.txt", &isDir);
    if (exists) {
        xprintf("main > file  'FirstDir/readme.txt' exists > ERROR\n");
    } else {
        xprintf("main > file  'FirstDir/readme.txt' does not exists > correct\n");
    }

   


    //this will remove the disk
    fileSystem->unmountDisk(0);

    //we could now free fileSystem (but RODOS does not support this)


  while(1) {
       suspendCallerUntil();
     }
}

};

Fatmos fatmos;