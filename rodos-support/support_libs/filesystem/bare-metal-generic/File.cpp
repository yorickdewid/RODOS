#include "File.h"
#include "rodos.h"

#include "./fatfs/ff.h"
#include "./fatfs/diskio.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


File::File() {
    filePtr = xmalloc(sizeof(FIL));
}

File::~File() {
}

int32_t File::open(const char *theFilePath, Flags fl) {
    filePath = theFilePath;
    FRESULT res = f_open((FIL*)filePtr, filePath, fl);
    isFileOpen = true;
    if (res != FR_OK) {
        //some error occured
        isFileOpen = false;
    }
    return res;
}

int32_t File::close() {
    FRESULT res = f_close((FIL*)filePtr);
    isFileOpen = false;
    return res;
}

int32_t File::errorNo() {
    return f_error((FIL*)filePtr);
}

int32_t File::read(void* buf, int32_t len) {

    if (!isFileOpen) {
        return -1;
    }
    UINT readBytes = 0;
    FRESULT res = f_read((FIL*)filePtr, buf, len, &readBytes);
    if (res != FR_OK) {
        return -1;
    }

    return readBytes;
}

int32_t File::write(const void* buf, int32_t len) {
    if (!isFileOpen) {
        return -1;
    }

    UINT writtenBytes = 0;
    FRESULT res = f_write((FIL*)filePtr, buf, len, &writtenBytes);
    if (res != FR_OK) {
        return -1;
    }

    return writtenBytes;
}

int32_t File::putChar(char c) {

    if (!isFileOpen) {
        return -1;
    }

    int32_t res = f_putc(c, (FIL*)filePtr);
    if (res != 1) {
        return -1;
    }
    return res;
}

int32_t File::putString(const char* aString) {

    if (!isFileOpen) {
        return -1;
    }

    int32_t res = f_puts(aString, (FIL*)filePtr);
    if (res < 0) {
        return -1;
    }
    return res;
}

int32_t File::getString(char* stringBuf, int32_t maxlen) {

    if (!isFileOpen) {
        return -1;
    }

    f_gets(stringBuf, maxlen, (FIL*)filePtr);
    return strlen(stringBuf);
}

int32_t File::lseek(uint32_t offset) {

    FRESULT res = f_lseek((FIL*)filePtr, offset);
    return res;
}

uint32_t File::tell() {
    if (!isFileOpen) {
        return -1;
    }
    return f_tell((FIL*)filePtr);
}

int32_t File::sync() {
    FRESULT res = f_sync((FIL*)filePtr);
    return res;
}

bool File::isOpen() {
    return isFileOpen;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

