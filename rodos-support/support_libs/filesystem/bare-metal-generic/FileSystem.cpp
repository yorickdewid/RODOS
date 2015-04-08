#include "FileSystem.h"

#include "rodos.h"

#include "./fatfs/ff.h"
#include "./fatfs/diskio.h"
#include "./fatfs/fatTime.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

FileSystem::FileSystem() {
	fatfsPtr = xmalloc(sizeof(FATFS));
}

int32_t FileSystem::mountDisk(int32_t diskNo) {
	FRESULT res = f_mount(diskNo, (FATFS*) fatfsPtr);
	return res;
}

int32_t FileSystem::unmountDisk(int32_t diskNo) {
	FRESULT res = f_mount(diskNo, 0);
	return res;
}

int32_t FileSystem::formatDisk(int32_t diskNo) {
//does not make sense for raspberry pi because OS and data are on the same sd card
	return -1;
}

int32_t FileSystem::createDirectory(const char *dirPath) {
	FRESULT res = f_mkdir(dirPath);
	return res;
}

int32_t FileSystem::remove(const char *filePath) {
	bool isDir = false;
	bool isExists = false;
	isExists = fileExists(filePath, &isDir);
	if (!isExists) {
		return FR_NO_FILE;
	}

	if (!isDir) {
		FRESULT res = f_unlink(filePath);
		return res;
	} else {
		// is Directory -> first delete all content
		DIR dir;
		FRESULT res = f_opendir(&dir, filePath);
		FILINFO fileInfo;
#if _USE_LFN
		static char lfn[_MAX_LFN + 1];
		fileInfo.lfname = lfn;
		fileInfo.lfsize = sizeof lfn;
#endif

		if (res == FR_OK) {
			while (true) {
				res = f_readdir(&dir, &fileInfo);
				if (res != FR_OK || fileInfo.fname[0] == 0) {
					break; // Break on error or end of dir
				}
				if (fileInfo.fname[0] == '.' && fileInfo.fname[1] == '\0') {
					continue; //Ignore dot entry
				}
				if (fileInfo.fname[0] == '.' && fileInfo.fname[1] == '.'
						&& fileInfo.fname[2] == '\0') {
					continue; //Ignore dotDot entry
				}

				char* fn;
#if _USE_LFN
				char currentFile[2500]; //this is arbitrary. Change if needed.
				//But c++ only accepts static values.
				if (*fileInfo.lfname) {
					fn = fileInfo.lfname;
				} else {
					fn = fileInfo.fname;
				}
#else
				char currentFile[200]; //this is arbitrary. Change if needed.
				//But c++ only accepts static values.
				fn = fileInfo.fname;
#endif

				if ((strlen(filePath) + strlen(fn)) > (sizeof currentFile)) { //prevent overflow
					return FR_INVALID_PARAMETER;
				}

				xsprintf(currentFile, "%s/%s", filePath, fn);

				remove(currentFile);
			}
		}
		return res;
	}
}

int32_t FileSystem::move(const char* oldPath, const char* newPath) {
	FRESULT res = f_rename(oldPath, newPath);
	return res;
}

int32_t FileSystem::copy(const char* oldPath, const char* newPath) {

	FIL fsrc, fdst; // file objects
	BYTE buffer[4096]; //file copy buffer
	UINT br, bw; // File read/write count
	// Open source file on the drive 1
	FRESULT res = f_open(&fsrc, oldPath, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK) {
		return res;
	}

	// Create destination file on the drive 0
	res = f_open(&fdst, newPath, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) {
		return res;
	}

	// Copy source to destination
	while (true) {
		res = f_read(&fsrc, buffer, sizeof(buffer), &br); // Read a chunk of src file
		if (res || br == 0)
			break; // error or eof
		res = f_write(&fdst, buffer, br, &bw); //Write it to the dst file
		if (res || bw < br)
			break; // error or disk full
	}

	// Close open files
	f_close(&fsrc);
	f_close(&fdst);
	return res;
}

int32_t FileSystem::getFileSize(const char* filePath) {
	FILINFO fileInfo;

	FRESULT res = f_stat(filePath, &fileInfo);
	if (res != FR_OK) {
		return -1;
	}

	return (int32_t) fileInfo.fsize;
}

int32_t FileSystem::getModificationDate(const char* filePath, int32_t* year, int32_t* month,
		int32_t* day, int32_t* hour, int32_t* min, int32_t* sec) {
	FILINFO fileInfo;
	FRESULT res = f_stat(filePath, &fileInfo);
	if (res != FR_OK) {
		*year = 1980;
		*month = 1;
		*day = 1;
		*hour = 0;
		*min = 0;
		*sec = 0;
		return res;
	}

	WORD date = fileInfo.fdate;
	WORD time = fileInfo.ftime;
	//date bit format (16 bit):
	//yyyy yyym mmmd dddd
	*year = ((date >> 9) & 127) + 1980;
	*month = (date >> 5) & 15;
	*day = date & 31;

	//time bit format (16 bit):
	//hhhh hMMM MMMm mmmm
	*hour = (time >> 11) & 31;
	*min = (time >> 5) & 63;
	*sec = (time & 31) * 2; //FAT only saves 2sec resolution
	return res;
}

bool FileSystem::fileExists(const char* filePath, bool* isDirectory) {
	FILINFO fileInfo;
	FRESULT res = f_stat(filePath, &fileInfo);
	if (res != FR_OK) {
		//some error occured
		*isDirectory = false;
		return false;
	}

	BYTE attributes = fileInfo.fattrib;

	//#define AM_DIR	0x10	// Directory
	*isDirectory = ((attributes & AM_DIR) != 0x00);
	return true;
}

int64_t FileSystem::totalSpace(const char* directoryPath) {

	DWORD tot_sect = (((FATFS*) fatfsPtr)->n_fatent - 2)
			* ((FATFS*) fatfsPtr)->csize;
	DWORD sectorSize;
	DRESULT res = disk_ioctl((BYTE) directoryPath[0], GET_SECTOR_SIZE, &sectorSize);
	if (res != RES_OK) {
		//some error occured
		return -1;
	}
	return tot_sect * sectorSize;
}

int64_t FileSystem::freeSpace(const char* directoryPath) {
	DWORD fre_clust, fre_sect;
	xprintf("FileSystem > freeSpace\n");

	FATFS* fs = (FATFS*) fatfsPtr;
	// Get volume information and free clusters of drive
	FRESULT res = f_getfree( directoryPath, &fre_clust, &fs);

	if (res != FR_OK) {
		//some error occured
		return -1;
	}

	//Get total sectors and free sectors
	fre_sect = fre_clust * ((FATFS*) fatfsPtr)->csize;

	DWORD sectorSize;
	DRESULT dres = disk_ioctl((BYTE) directoryPath[0], GET_SECTOR_SIZE, &sectorSize);
	if (dres != RES_OK) {
		//some error occured
		return -1;
	}
	return fre_sect * sectorSize;
}

void FileSystem::setTime(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t min,
		int32_t sec) {
	setCurrentTime(year, month, day, hour, min, sec);
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
