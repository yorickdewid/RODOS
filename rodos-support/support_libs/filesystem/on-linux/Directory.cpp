/*
 * Directory.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: Johannes Freitag
 */

#include "filesystem.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include <rodos.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


#define DIRTYPE_FILE 0x8

Directory::Directory() {
	dirPtr = NULL;
	dirPath = NULL;
	isDirOpen = false;
}

Directory::~Directory() {
	closedir((DIR *) dirPtr);
}

int32_t Directory::open(const char* theDirPath) {
	dirPath = theDirPath;

	dirPtr = opendir(theDirPath);
	//If the directory is not open return -1 (maybe dir does not exist or user has no permission...)
	if (dirPtr == NULL) {
		return -1;
	}
	return 0;
}

int32_t Directory::readNextEntry(char* nextFileName, int32_t bufLength, int32_t* filesize) {
	//read the next file or folder of the current directory
	struct dirent *dirp = readdir((DIR *) dirPtr);
	if (dirp != NULL) {
		//return the next filename
		sprintf(nextFileName, "%.*s", bufLength, dirp->d_name);
		if (dirp->d_type == DIRTYPE_FILE) {
			//if the element is a file, get the filesize
			char filePath[255];
			sprintf(filePath, "%s/%s", dirPath, nextFileName);
			struct stat fileStat;
			if (!stat(filePath, &fileStat)) {
				*filesize = fileStat.st_size;
			} else {
				*filesize = 0;
			}
		} else {
			//Directories have always filesize 0
			*filesize = 0;
		}
		return 0;
	} else {
		//Error or there are no more entries in this folder
		nextFileName[0] = '\0';
		return -1;
	}

}

int32_t Directory::resetEntryEnumeration() {
	//for reseting the entry enumeration just open the folder again
	dirPtr = opendir(dirPath);
	if (dirPtr == NULL) {
		return -1;
	}
	return 0;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

