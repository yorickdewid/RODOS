
#include "Directory.h"
#include "rodos.h"
#include "./fatfs/ff.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


Directory::Directory() {
    dirPtr = xmalloc(sizeof (DIR));
}

Directory::~Directory() {
}

int32_t Directory::open(const char* theDirPath) {
    dirPath = theDirPath;
    FRESULT res = f_opendir((DIR*) dirPtr, dirPath);
    isDirOpen = true;
    if (res != FR_OK) {
        //some error occured
        isDirOpen = false;
    }
    return res;
}

int32_t Directory::resetEntryEnumeration() {

    FRESULT res = f_readdir((DIR*) dirPtr, NULL);
    return res;
}

int32_t Directory::readNextEntry(char* nextFileName, int32_t bufLength, int32_t* filesize) {

    FILINFO fileInfo;

#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fileInfo.lfname = lfn;
    fileInfo.lfsize = sizeof lfn;
#endif

    FRESULT res = f_readdir((DIR*) dirPtr, &fileInfo);

    char* fn;
#if _USE_LFN
    if (*fileInfo.lfname) {
        fn = fileInfo.lfname;
    } else {
        fn = fileInfo.fname;
    }
#else
    fn = fileInfo.fname;
#endif

    strncpy(nextFileName, fn, bufLength);
    *filesize = (int32_t) fileInfo.fsize;

    return res;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
