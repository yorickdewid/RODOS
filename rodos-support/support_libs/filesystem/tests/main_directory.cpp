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
Directory d_;

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
    Directory* d = &d_;


    xprintf("\n////////////////////////////\n"
            "main > TESTING INIT AND MOUNT\n"
            "//////////////////////\n");

    fileSystem->enableDebugOutput(true);

    //disk must be mounted before anything else can be done
    fileSystem->mountDisk(0);
    xprintf("main > mount done\n");

    //format will create a new fat file system
    //this can be omitted if there already is a fs on the disk
    fileSystem->formatDisk(0);
    xprintf("main > format done\n");


    //setting time changes RODOS system time
    fileSystem->setTime(2012, 03, 03, 07, 31, 12);



    xprintf("\n////////////////////////////\n"
            "main > TESTING DIRECTORY FUNTIONS\n"
            "///////////////////////////\n");

    file->enableDebugOutput(true);

    file->open("/test.txt", file->O_WRITE);
    file->putString("This is my first file!\n");
    file->close();
    xprintf("main > created '/test.txt'\n");



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


    //if we create directories one after the other everything will work fine
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


    //fill directory with files
    fileSystem->move("/test.txt", "/FirstDir/test.txt");

    file->open("FirstDir/file2.txt", file->O_WRITE);
    file->putString("This is file2, dude.");
    file->close();

    file->open("FirstDir/file3.txt", file->O_WRITE);
    file->putString("This is file3, bro.");
    file->close();

    //rename entire directory
    fileSystem->move("FirstDir", "MyDir");

    
    
    //open and interate through directory
    d->open("/MyDir");
    d->resetEntryEnumeration();
    xprintf("main > reset entry numeration on  '/MyDir/'\n");

    char nextFileName[100];
    long size;
    d->readNextEntry(nextFileName, 100, &size);

    while (nextFileName[0] != '\0') {
        xprintf("main > next entry in  '/MyDir/' is %s\nsize: %ld\n", nextFileName, size);
        char path[200];
        path[0] = 0;
        xsprintf(path, "/MyDir/%s", nextFileName);

        exists = fileSystem->fileExists(path, &isDir);
        //read content if file
        if (isDir == false) {
            file->enableDebugOutput(true);
            file->open(path, file->O_READ);

            char readStr[200];
            readStr[0] = 0;
            file->getString(readStr, 200);
            xprintf("main > read file > %s\n%s\n", path, readStr);
            file->close();
        }
        nextFileName[0] = 0;

        d->readNextEntry(nextFileName, 100, &size);
    }

    //directories don't need to be closed after use. you can just free them
    
    
   
    //this will remove the disk
    fileSystem->unmountDisk(0);

  
  while(1) {
       suspendCallerUntil();
     }
}

};

Fatmos fatmos;