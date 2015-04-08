/* 
 * File:   Directory.h
 * Author: gabriel reimers
 *
 * Created on November 10, 2011, 12:54 PM
 */

#include "stdint.h"

#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class Directory {
private:

    void* dirPtr;
    const char* dirPath;
    bool isDirOpen;

public:
    Directory();
    ~Directory();

    // all methods of Directory require that the directory is opened beforehand
    // filesystem must be mounted before calling any file operations
    //FRESULT error code are defined in fat_fs/ff.h

    /**
     * opens directory at given path
     * directory objects don't need to be closed
     * @param theDirPath directory to open
     * @return FRESULT error code (0 if successful)
     */
    int32_t open(const char* theDirPath);

    /**
     * writes path of next file (or subdirectory) to nextPath
     * nextPath will be NULL if end of directory reached or in case of error
     * writes filesize of next file to filesize (0 if end or error)
     * 
     * @param nextFileName buffer where next file name will be written to
     * @param bufLength max number of file name chars to read
     * @param filesize file size of next file
     * @return FRESULT error code (0 if successful)
     */
    int32_t readNextEntry(char* nextFileName, int32_t bufLength, int32_t* filesize);


    /**
     * resets the entry enumeration so you can go thrugh the directory
     * from start
     * @return FRESULT error code (0 if successful)
     */
    int32_t resetEntryEnumeration();

};

#ifndef NO_RODOS_NAMESPACE
}
#endif
