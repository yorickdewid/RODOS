/* 
 * File:   FileSystem.h
 * Author: gabriel reimers
 *
 * Created on November 10, 2011, 12:54 PM
 */

#include "stdint.h"

#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class FileSystem {

private:
    void* fatfsPtr;

public:
    
    // does nothing
    // call mountDisk before using any file operation
    FileSystem();

    /**
     * Mounts a disk. This must be done before anything else can be done.
     * @param diskNo index of disk to be mounted (0 is first)
     * @return FRESULT error code (0 if ok)
     */
    int32_t mountDisk(int32_t diskNo);


    /**
     * unmounts the disk. after unmounding no more actions can be performed.
     * @param diskNo index of disk to be unmounted (0 is first)
     * @return FRESULT error code (0 if ok)
     */
    int32_t unmountDisk(int32_t diskNo);


    /**
     * formats the disk and creates a new FAT (all data will be lost!)
     * disk must be mounted before formatting can be done
     * @param diskNo index of disk to be formatted (0 is first)
     * @return FRESULT error code (0 if successful)
     */
    int32_t formatDisk(int32_t diskNo);


    /**
     * creates a new Directory at the given path
     * intermediate directories must already exist. only the last path
     * component will be created
     * @param dirPath path of folder to be created
     * @return FRESULT error code (0 if successful)
     */
    int32_t createDirectory(const char *dirPath);


    /**
     * removes the item at the given path (if it exists)
     * if path is a folder all contents will be deleted recursively
     * @param filePath file to be removed
     * @return FRESULT error code (0 if successful)
     */
    int32_t remove(const char *filePath);


    /**
     * moves/renames the file at oldPath to newPath
     * subdirectories must exist for newPath
     * existing files will not be replaced. will fail if target already exists.
     * @param oldPath
     * @param newPath
     * @return FRESULT error code (0 if successful)
     */
   int32_t move(const char* oldPath, const char* newPath);


    /**
     * copies the file at oldPath to newPath
     * subdirectories must exist for newPath
     * existing files will not be replaced. will fail if target already exists.
     * @param oldPath
     * @param newPath
     * @return FRESULT error code (0 if successful)
     */
    int32_t copy(const char* oldPath, const char* newPath);

    /**
     * returns the file Size in bytes
     * @param filePath file to calculate
     * @return file size in bytes, -1 on error
     */
    int32_t getFileSize(const char* filePath);

    
    /**
     * returns the file's modification date (creation date for folders)
     * @param filePath file
     * @return FRESULT error code (0 if successful)
     */
    int32_t getModificationDate(const char* filePath, int32_t* year, int32_t* month, int32_t* day, int32_t* hour, int32_t* min, int32_t* sec);

    /**
     * returns true if file exists
     * assigns true to isDirectory if is directory
     * returns false in case of error
     * @param filePath absolute path of file
     * @param isDirectory 
     * @return true if file exists, false if not and on error
     */
    bool fileExists(const char* filePath, bool* isDirectory);


    /**
     * returns total number of  bytes in file system
     * @param directoryPath (on fatfs use "diskNo:" e.g. "0:")
     * @return total disk size in bytes, -1 on error
     */
    int64_t totalSpace(const char* directoryPath);

    /**
     * returns number of free bytes in file system
     * this scans the entire disk and will take long. Avoid to call.
     * @param directoryPath (on fatfs use "diskNo:" e.g. "0:")
     * @return free bytes, -1 on error
     */
    int64_t freeSpace(const char* directoryPath);

    /**
     * Sets the file system time. If no time is set the system will start 1980-01-01.
     * This affects the operating system time on RODOS (or other embedded systems)
     * @param year
     * @param month
     * @param day
     * @param hour
     * @param min
     * @param sec
     */
    void setTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t min, int32_t sec);


};



#ifndef NO_RODOS_NAMESPACE
}
#endif
