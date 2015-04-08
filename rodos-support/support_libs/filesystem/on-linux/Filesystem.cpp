/*
 * Filesystem.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: Johannes Freitag
 */

#include "filesystem.h"

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <fstream>

#include <rodos.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


FileSystem::FileSystem() {
}

int32_t FileSystem::mountDisk(int32_t diskNo) {
	//Does not make sense in linux implementation - there are no diskNo
	return 0;
}

int32_t FileSystem::unmountDisk(int32_t diskNo) {
	//Does not make sense in linux implementation - there are no diskNo
	return -1;
}

int32_t FileSystem::formatDisk(int32_t diskNo) {
	//Does not make sense in linux implementation - there are no diskNo
	return -1;
}

int32_t FileSystem::createDirectory(const char *dirPath) {
	//Create new directory
	if (mkdir(dirPath, S_IRWXU | S_IRGRP | S_IXGRP) != 0) {
		return -1;
	} else {
		return 0;
	}
}

int32_t FileSystem::remove(const char *filePath) {
	//Remove file or folder
	if (::remove(filePath) != 0) {
		return -1;
	} else {
		return 0;
	}
}

int32_t FileSystem::move(const char* oldPath, const char* newPath) {

	//Move element only if newPath does not exist
	bool isDirectory;
	if (fileExists(newPath, &isDirectory)) {
		return -1;
	}

	//Move the file/folder
	if (rename(oldPath, newPath) != 0) {
		return -1;
	} else {
		return 0;
	}
}

int32_t FileSystem::copy(const char* oldPath, const char* newPath) {

	//open files
	std::ifstream source(oldPath, std::ios::binary);
	std::ofstream dest(newPath, std::ios::binary);

	//set the iterators
	std::istreambuf_iterator<char> begin_source(source);
	std::istreambuf_iterator<char> end_source;
	std::ostreambuf_iterator<char> begin_dest(dest);
	std::copy(begin_source, end_source, begin_dest);

	//if something went wrong
	if (!dest.good()) {
		source.close();
		dest.close();
		return -1;
	}

	//close the files
	source.close();
	dest.close();

	return 0;

}

int32_t FileSystem::getFileSize(const char* filePath) {
	struct stat fileStat;
	if (!stat(filePath, &fileStat)) {
		//If element is a file return the size
		if (S_ISREG (fileStat.st_mode)) {
			return fileStat.st_size;
		} else {
			//Folder have size 0
			return 0;
		}
	} else {
		return -1;
	}
}

int32_t FileSystem::getModificationDate(const char* filePath, int32_t* year, int32_t* month, int32_t* day, int32_t* hour, int32_t* min, int32_t* sec) {
	struct stat fileStat;
	if (!stat(filePath, &fileStat)) {
		time_t m1 = fileStat.st_mtim.tv_sec;
		//format the time for returning
		struct tm * ptm;
		ptm = gmtime(&m1); //UTC Time
		*year = ptm->tm_year + 1900;
		*month = ptm->tm_mon; //Month start at 0 (0=January, 1=February...)
		*day = ptm->tm_mday;
		*hour = ptm->tm_hour;
		*min = ptm->tm_min;
		*sec = ptm->tm_sec;
		return 0;
	} else {
		return -1;
	}
}

bool FileSystem::fileExists(const char* filePath, bool* isDirectory) {
	struct stat fileStat;
	if (!stat(filePath, &fileStat)) {
		//File exists -> check if its a directory
		if (S_ISREG (fileStat.st_mode)) {
			*isDirectory = false;
		}
		if (S_ISDIR (fileStat.st_mode)) {
			*isDirectory = true;
		}
		return true;
	} else {
		//File does not exist
		return false;
	}
}

int64_t FileSystem::totalSpace(const char* directoryPath) {
	struct statvfs vfsStat;
	if (!statvfs(directoryPath, &vfsStat)) {
		return  (int64_t) ((uint64_t)vfsStat.f_bsize * (uint64_t)vfsStat.f_blocks);
	}
	return -1;
}

int64_t FileSystem::freeSpace(const char* directoryPath) {
	struct statvfs vfsStat;
	if (!statvfs(directoryPath, &vfsStat)) {
		return  (int64_t) ((uint64_t)vfsStat.f_bsize * (uint64_t)vfsStat.f_bavail);
	}
	return -1;
}

void FileSystem::setTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t min, int32_t sec) {
	//Does not make sense in linux implementation - time is provided by the os
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

