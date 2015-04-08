
/*
 * File:   main.cpp
 * Author: gabriel reimers 2012
 * Created on November 10, 2011, 12:37 PM
 */

#include "filesystem.h"
#include "rodos.h"


FileSystem fileSystem;
File file;

class FatmosFile: public Thread {
public:
    FatmosFile(): Thread("FATMOSFile", 20, 10000) { }

    void run() {
        //disk must be mounted before anything else can be done
         fileSystem.mountDisk(0);

        /** Writting a different positions ***/

        file.open("test.txt", file.O_WRITE);

        file.putString("This is my first");
        file.putString("file.");
        file.putString(" do you like it?");

        int32_t t = file.tell();
        PRINTF("we have written %d chars\n", t);

        file.lseek(t-8);
        file.write("LIKE000", 4); // only 4 chars, rest dont care
        file.sync();
        file.close();

        /************ Reading **********/

        file.open("test.txt", file.O_READ);

        char readBuf[100];
        int32_t len = file.read(readBuf, 10);
        readBuf[len] = 0;
        PRINTF("read after write  the first 10 chars: %d bytes: '%s'\n", len, readBuf);

        len = file.read(readBuf, 100);
        readBuf[len] = 0;
        PRINTF("read after write  the rest: %d bytes: '%s'\n", len, readBuf);

        file.lseek(10);
        len = file.read(readBuf, 100);
        readBuf[len] = 0;
        PRINTF("read skeep 10 chars : %d bytes: '%s'\n", len, readBuf);
        file.close();

        PRINTF("please ^C to terminate\n");

    }

} fatmosFile;


