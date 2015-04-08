/*
 * File:   main.cpp
 * Author: gabriel reimers 2012
 *
 * Created on November 10, 2011, 12:37 PM
 */

#include "filesystem.h"
#include "rodos.h"


FileSystem fileSystem;
File       file;
Directory  directory;

class FatmosDir: public Thread {
public:
    FatmosDir(): Thread("FATMOSDir", 20, 10000) { }

    void run() {

        fileSystem.mountDisk(0);

        fileSystem.createDirectory("newDir");
        bool isDir;
        fileSystem.fileExists("newDir", &isDir);

        //fill directory with files
        fileSystem.move("test.txt", "newDir/test.txt");

        //open and interate through directory
        directory.open("newDir");
        directory.resetEntryEnumeration();
        
        char nextFileName[100];
        int32_t size;
        directory.readNextEntry(nextFileName, 100, &size);

        while (nextFileName[0] != '\0') {
            PRINTF("next entry in newDir is %s\nsize: %ld\n", nextFileName, size);
            directory.readNextEntry(nextFileName, 100, &size);
        }

        //this will remove the disk
        //fileSystem->unmountDisk(0);
        PRINTF("please ^C to terminate\n");
    }

} fatmosDir;

