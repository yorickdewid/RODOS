/* 
 * File:   File.h
 * Author: gabriel reimers
 *
 * Created on November 10, 2011, 12:54 PM
 */

#include "stdint.h"

#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class File {

private:
	void* filePtr;
	const char* filePath;
	bool isFileOpen;

public:

	enum Flags {
		// open for reading
		// error, if not exists
		O_READ = 0x01, //equals FA_OPEN_EXISTING | FA_READ

		// open for write (does *not* truncate to zero)
		// create, if not exists
		O_WRITE = 0x12, //equals FA_WRITE | FA_OPEN_ALWAYS

		// open for write (does truncate to zero)
		// create, if not exists
		O_WRITENEW = 0x0A //equals FA_WRITE | FA_CREATE_ALWAYS
	};

	File();
	~File();

	// all methods of File require that the file is opened beforehand
	// filesystem must be mounted before calling any file operations
	//FRESULT error code are defined in fat_fs/ff.h

	/**
	 * opens file at given path. This must be done before any other operation
	 * can be performed.
	 * use flags to determine read or write access
	 * @param filePath file to be opened
	 * @param fl read/write flag
	 * @return FRESULT error code (0 if successful)
	 */
	int32_t open(const char *filePath, Flags fl);

	/**
	 * Writes all pending data to disk and closes the file.
	 * @return FRESULT error code (0 if successful)
	 */
	int32_t close();

	/**
	 * Returns non-zero if an error had occurred, 0 if not.
	 * @return non-zero on error, 0 if not
	 */
	int32_t errorNo();

	/**
	 * reads data from file to buffer buf of length len
	 * returns number of bytes read, if return value is smaller than len
	 * -> eof was reached
	 * @param buf buffer where read data will be written to
	 * @param len max number of bytes to be read
	 * @return number of bytes read, -1 on error
	 */
	int32_t read(void* buf, int32_t len);

	/**
	 * writes data from buffer buf of length len to file
	 * returns number of bytes written, if return value is smaller than len
	 * -> disk is full
	 * @param buf data to be written
	 * @param len max number of bytes to be written
	 * @return number of bytes written, -1 on error
	 */
	int32_t write(const void* buf, int32_t len);

	/**
	 * Put a single character to the file
	 * @param c character to write
	 * @return 1 if success -1 if not
	 */
	int32_t putChar(char c);

	/**
	 * Put a null-terminated string to the file
	 * returns number of written chars if success, if this is shorter than
	 * aString length the disk is full
	 * will write eof in the end not null-character
	 * @param aString string to be written
	 * @return number of written chars, -1 on error
	 */
	int32_t putString(const char* aString);

	/**
	 * reads a null terminated string from the file to buffer stringBuf
	 * of maximum length maxlen
	 *  returns number of characters read, if return value is smaller than maxlen
	 * -> eof was reached
	 * @param stringBuf buffer where read string will be written to
	 * @param maxlen max number of bytes to be read
	 * @return number of characters read, -1 on error
	 */
	int32_t getString(char* stringBuf, int32_t maxlen);

	/**
	 * Sets the current read/write pointer position to offset.
	 * If off is bigger than the file size, the file will be extended to match offset.
	 * Use this when writing bigger files (> 2kb) to increase the file size before
	 * writing.
	 * //increase to 2mb and start writing at 0:
	 * lseed(2000000); lseek(0);
	 * @param offset new read/write pointer position
	 * @return FRESULT error code (0 if successful)
	 */
	int32_t lseek(uint32_t offset);

	/**
	 * Returns the current read/write pointer position in the file.
	 * @return read/write pointer position
	 */
	uint32_t tell();

	/**
	 * writes buffered data to disk
	 * has same effect as close (but keeps file open)
	 * -> no need to do sync(); close(); both
	 * @return FRESULT error code (0 if successful)
	 */
	int32_t sync();

	/**
	 * Returns true if file is open.
	 * @return true if open
	 */
	bool isOpen();

    /**
     * returns the file size in bytes
     * @return file size in bytes, -1 on error
     */
	int32_t getFileSize();
};



#ifndef NO_RODOS_NAMESPACE
}
#endif
