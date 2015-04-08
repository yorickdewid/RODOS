/*
 * File.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: Johannes Freitag
 */

#include "filesystem.h"

#include <fstream>
#include <sys/stat.h>
#include <rodos.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


File::File() {
	filePath = NULL;
	isFileOpen = false;
	filePtr = new std::fstream; //construct the filestream
}

File::~File() {
}

int32_t File::open(const char *filePath, Flags fl) {

	this->filePath = filePath;

	if (fl == O_READ) {
		//open file in READ mode
		((std::fstream*) filePtr)->open(filePath, std::fstream::in);
		isFileOpen = ((std::fstream*) filePtr)->is_open();
	} else if (fl == O_WRITE) {
		//open file in READ/WRITE mode
		struct stat fileStat;
		if (!stat(filePath, &fileStat)) {
			((std::fstream*) filePtr)->open(filePath, std::fstream::in | std::ios::out);
		} else {
			//If file does not exist open a new one
			((std::fstream*) filePtr)->open(filePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
		}
		isFileOpen = ((std::fstream*) filePtr)->is_open();
	} else if (fl == (O_WRITENEW)) {
		//open file in READ/WRITE mode - new file
		((std::fstream*) filePtr)->open(filePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
		isFileOpen = ((std::fstream*) filePtr)->is_open();
	}

	if (isFileOpen == true) {
		return 0;
	} else {
		return -1;
	}
}

int32_t File::close() {
	//close the filestream
	((std::fstream*) filePtr)->close();
	isFileOpen = false;
	return 0;
}

int32_t File::errorNo() {
	//no error counter at linux
	return -1;
}

int32_t File::read(void* buf, int32_t len) {
	//only if the file is open
	if (!((std::fstream*) filePtr)->is_open()) {
		return -1;
	}

	//read data from the file and return the number of bytes read
	((std::fstream*) filePtr)->read((char*) buf, len);
	((std::fstream*) filePtr)->seekp(((std::fstream*) filePtr)->tellg());
	if (!((std::fstream*) filePtr)->eof())
		return len;
	else
		return ((std::fstream*) filePtr)->gcount();
}

int32_t File::write(const void* buf, int32_t len) {
	//only if the file is open
	if (((std::fstream*) filePtr)->is_open()) {
		((std::fstream*) filePtr)->write((const char*) buf, len);
		((std::fstream*) filePtr)->seekg(((std::fstream*) filePtr)->tellp());
		//if everything is ok return the number of bytes written
		if (((std::fstream*) filePtr)->good())
			return len;
	}

	return -1;
}

int32_t File::putChar(char c) {
	return write(&c, 1);
}

int32_t File::putString(const char* aString) {
	return write(aString, strlen(aString));
}

int32_t File::getString(char* stringBuf, int32_t maxlen) {
	//get number of bytes and terminate the string
	int32_t length = read((void*) stringBuf, maxlen - 1);
	stringBuf[length] = '\0';
	return length;
}

int32_t File::lseek(uint32_t offset) {

	if (((std::fstream*) filePtr)->is_open()) {
		//if pointer was eof, clear the eof flag to proceed
		if (((std::fstream*) filePtr)->eof()) {
			((std::fstream*) filePtr)->clear();
		}
		//place the read/write pointer
		((std::fstream*) filePtr)->seekg(offset);
		((std::fstream*) filePtr)->seekp(offset);
		if (((std::fstream*) filePtr)->good())
			return 0;
	}

	return -1;

}

uint32_t File::tell() {
	//if file in READ mode
	if (((std::fstream*) filePtr)->is_open()) {
		if (((std::fstream*) filePtr)->tellg() == ((std::fstream*) filePtr)->tellp()) {
			return ((std::fstream*) filePtr)->tellg();
		}
	}
	return -1;
}

int32_t File::sync() {
	((std::fstream*) filePtr)->sync();
	((std::fstream*) filePtr)->flush();
	return 0;
}

bool File::isOpen() {
	return isFileOpen;
}

int32_t File::getFileSize() {

	std::streampos fsize = 0;
	std::streampos pos = 0;

	//if file is open
	if (((std::fstream*) filePtr)->is_open()) {
		pos = ((std::fstream*) filePtr)->tellg(); //current position
		((std::fstream*) filePtr)->seekg(0, std::ios::end);
		fsize = ((std::fstream*) filePtr)->tellg(); //eof
		((std::fstream*) filePtr)->seekg(pos); //restore current position
		return fsize;
	}

	return -1;

}


#ifndef NO_RODOS_NAMESPACE
}
#endif

