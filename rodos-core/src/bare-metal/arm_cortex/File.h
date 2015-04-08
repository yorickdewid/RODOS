#ifndef FILE_H_
#define FILE_H_
#include "ff.h"

#define NOTHING 0
#define PLAIN_FILE 1
#define DIRECTORY 2
#define RAW_SD_CARD 3

class File {

private:
	char pathName[64];
static	DIR g_sDirObject;
static	FIL g_sFileObject;

public:

	File();
	File(char* fileAndPathName);  // opens directories to (read only)
	~File();

//	File(); // SD-Card access without file system, just using offsets
	
	bool create();

	bool open();  // if not difficult open for read and write, if difficult 
				 // then bool openForRead(); bool openForWrite();
	bool close();

	int errorNo;	//0 -> ok, else some error number from lasp operation

	int fileType; // eg plainFile, directory, raw-SD-Card

	long fileLen;  // may grow when writing

	long writePointer; // used and incremented for each write, user can modify it
	long readPointer; // used incremented for each read, user can modify it

    int ls(char* buf, int len);
	int read(void* buf, int len); //  return number of bytes read, or 0 or  <0 in case of error
	int write(void* buf, int len); //  return number of bytes written, or 0 or  <0 in case of error
		

};

/* File function return code (FRESULT) and this code are wrote to errorNo */
/*
typedef enum {
    FR_OK = 0,            	// 0 
    FR_NOT_READY,        	// 1 
    FR_NO_FILE,            	// 2 
    FR_NO_PATH,            	// 3 
    FR_INVALID_NAME,    	// 4 
    FR_INVALID_DRIVE,    	// 5 
    FR_DENIED,            	// 6 
    FR_EXIST,            	// 7 
    FR_RW_ERROR,        	// 8 
    FR_WRITE_PROTECTED,    	// 9 
    FR_NOT_ENABLED,        	// 10 
    FR_NO_FILESYSTEM,    	// 11 
    FR_INVALID_OBJECT,    	// 12 
    FR_MKFS_ABORTED        	// 13 
    BUF_LEN_INVALID			//14
} FRESULT;
*/

/********* Global Fuctions **************/

bool mkdir(char *pathname);
bool rmdir(char *patname);
bool rm(char *pathname);

int totalSpace(); // returns total number of  bytes in file system
int freeSpace(); // returns number of free bytes in file system


#endif /*FILE_H_*/
