
/*
 * File:   main.cpp
 * Author: gabriel reimers 2012
 * Created on November 10, 2011, 12:37 PM
 */

#include "filesystem.h"
#include "rodos.h"


//FileSystem fileSystem;
File inFile;
File outFile;

class FatmosFile: public Thread {
public:
    FatmosFile(): Thread("FATMOSFile", 20, 10000) { }

    void run() {
        char readBuf[100];
        int32_t len;
        
        // fileSystem.mountDisk(0);


        outFile.open("test.txt", File::O_WRITE);
        inFile.open( "test.txt", File::O_READ);

        outFile.putString("This is my first file.");
        len = inFile.read(readBuf, 100);
        readBuf[len] = 0;
        PRINTF("read after fist wirte %d bytes = '%s'\n", len, readBuf);

        outFile.putString(" do you like it?");
        len = inFile.read(readBuf, 100);
        readBuf[len] = 0;
        PRINTF("read after fist wirte %d bytes = '%s'\n", len, readBuf);

        inFile.close();
        outFile.close();

        PRINTF("Please ^C to terminate\n");

    }

} fatmosFile;


