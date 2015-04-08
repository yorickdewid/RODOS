#include "File.h"
#include "sd_fatfs/src/ff.h"
#include "sd_fatfs/src/diskio.h"
#include "utils/ustdlib.h"
#include <string.h>
#include "params.h"


DIR File::g_sDirObject;
FIL File::g_sFileObject;

File::File()
{
	File("/");
}

File::File(char* fileAndPathName)
{
	//int cnt;
    FRESULT fresult;
    
	//create buffor for pathName
	//cnt=strlen(fileAndPathName);
	strcpy(pathName,fileAndPathName);
	//try to open pathName as directory
    fresult = f_opendir(&g_sDirObject, pathName);

    if(fresult == FR_OK)
    {
    	errorNo=FR_OK;
    	fileType=DIRECTORY;
    }
    else
    {
        fresult = f_open(&g_sFileObject, pathName, FA_READ);

        if(fresult == FR_OK)
        {
        	errorNo=FR_OK;
        	fileType=PLAIN_FILE;
        	f_close (&g_sFileObject);
        }
        else
        {
        	errorNo=fresult;
        	fileType=NOTHING;
        }
    }	
}

bool File::open()
{
	FILINFO g_sFileInfo;
	FRESULT fresult;
	
    fresult = f_opendir(&g_sDirObject, pathName);

    if(fresult == FR_OK)
    {
    	errorNo=FR_OK;
    	fileType=DIRECTORY;
    	return true;
    }
    else
    {	
    	fresult = f_open(&g_sFileObject, pathName, FA_READ | FA_WRITE);
    	if(fresult == FR_OK)
    	{
    		if(f_stat(pathName, &g_sFileInfo)==FR_OK)
    			fileLen=g_sFileInfo.fsize;
    		else
    			fileLen=0;
    		writePointer=fileLen;
    		readPointer=0;
        	fileType=PLAIN_FILE;
    		errorNo=fresult;
    		return true;
    	}
    	else
    	{
    		errorNo=fresult;
    		fileType=NOTHING;
    		return false;
    	}
    }
}
bool File::close()
{
	FRESULT fresult;
	
	if(fileType==PLAIN_FILE)
	{
		fresult=f_close (&g_sFileObject);
		errorNo=fresult;
		if(fresult == FR_OK)
			return true;
		else
			return false;
	}
	else
	{
		errorNo=FR_NO_FILE;
		return false;
	}
}
bool File::create()
{
	FRESULT fresult;
	
	fresult = f_open(&g_sFileObject, pathName, FA_READ | FA_WRITE | FA_CREATE_NEW);
    if(fresult == FR_OK)
    {
    	fileType=PLAIN_FILE;
    	fileLen=0;
    	writePointer=0;
    	readPointer=0;
    	errorNo=fresult;
        return true;
    }
    else
    {
    	errorNo=fresult;
        return false;
    }
}
int File::ls(char* buf, int len)
{
    unsigned long ulTotalSize;
    unsigned long ulFileCount;
    unsigned long ulDirCount;
    int iIndex = 0;
    int iTemp;
    FRESULT fresult;
    FILINFO g_sFileInfo;
    char tab[32];
	
	if(fileType==DIRECTORY)
	{
	    ulTotalSize = 0;
	    ulFileCount = 0;
	    ulDirCount = 0;
	    
	    fresult = f_opendir(&g_sDirObject, pathName);
	    //
	    // Enter loop to enumerate through all directory entries.
	    //
	    for(;;)
	    {
	        //
	        // Read an entry from the directory.
	        //
	        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

	        //
	        // Check for error and return if there is a problem.
	        //
	        if(fresult != FR_OK)
	        {
	        	errorNo=fresult;
	            return -1;
	        }

	        //
	        // If the file name is blank, then this is the end of the
	        // listing.
	        //
	        if(!g_sFileInfo.fname[0])
	        {
	            break;
	        }

	        //
	        // If the attribue is directory, then increment the directory count.
	        //
	        if(g_sFileInfo.fattrib & AM_DIR)
	        {
	            ulDirCount++;
	        }

	        //
	        // Otherwise, it is a file.  Increment the file count, and
	        // add in the file size to the total.
	        //
	        else
	        {
	            ulFileCount++;
	            ulTotalSize += g_sFileInfo.fsize;
	        }

	        //
	        // Print the entry information on a single line with formatting
	        // to show the attributes, date, time, size, and name.
	        //
	        iTemp = usprintf(tab,"%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
	                    (g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
	                    (g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
	                    (g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
	                    (g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
	                    (g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
	                    (g_sFileInfo.fdate >> 9) + 1980,
	                    (g_sFileInfo.fdate >> 5) & 15,
	                     g_sFileInfo.fdate & 31,
	                    (g_sFileInfo.ftime >> 11),
	                    (g_sFileInfo.ftime >> 5) & 63,
	                     g_sFileInfo.fsize,
	                     g_sFileInfo.fname);  

	        if(iIndex + iTemp < len)
	        {
	        	memcpy(&(buf[iIndex]),tab,iTemp);
	        	iIndex += iTemp;
	        }
	        else
	        {
	        	errorNo=14;
	        	return -1;
	        }
	    }   // endfor

	    //
	    // Print summary lines showing the file, dir, and size totals.
	    //
	    iTemp = usprintf(tab,"\n%4u File(s),%10u bytes total\n%4u Dir(s)",
	                ulFileCount, ulTotalSize, ulDirCount);
	    
        if(iIndex + iTemp < len)
        {
        	memcpy(&(buf[iIndex]),tab,iTemp);
        	iIndex += iTemp;
        }
        else
        {
        	errorNo=14;
        	return -1;
        }

	    //
	    // Made it to here, return with no errors.
	    //
        errorNo= FR_OK;
	    return iIndex;
	}
	else
	{
		errorNo=FR_NO_FILE;
		return -1;
	}
}
int File::read(void* buf, int len)
{
    FRESULT fresult;
    unsigned short usBytesRead;
    
	if(fileType==PLAIN_FILE)
	{
		fresult = f_lseek (&g_sFileObject, readPointer);
		if(fresult != FR_OK)
		{
        	errorNo=fresult;
        	return -1;
		}
        //
        // Read a block of data from the file. 
        //
        fresult = f_read(&g_sFileObject, buf, len, &usBytesRead);
        if(fresult == FR_OK)
        {
        	errorNo=FR_OK;
        	readPointer+=usBytesRead;
        	return usBytesRead;
        }
        else
        {
        	errorNo=fresult;
        	return -1;
        }
	}
	else
	{
		errorNo=FR_NO_FILE;
		return -1;
	}
}

int File::write(void* buf, int len)
{
    FRESULT fresult;
    unsigned short usBytesWrite;
    
	if(fileType==PLAIN_FILE)
	{
		fresult = f_lseek (&g_sFileObject, writePointer);
		if(fresult != FR_OK)
		{
        	errorNo=fresult;
        	return -1;
		}
        //
        // Read a block of data from the file. 
        //
        fresult = f_write(&g_sFileObject, buf, len, &usBytesWrite);
        if(fresult == FR_OK)
        {
        	f_sync (&g_sFileObject);
        	errorNo=FR_OK;
        	writePointer+=usBytesWrite;
        	return usBytesWrite;
        }
        else
        {
        	errorNo=fresult;
        	return -1;
        }
	}
	else
	{
		errorNo=FR_NO_FILE;
		return -1;
	}
    
}
File::~File()
{
	if(fileType==PLAIN_FILE)
	{
		f_close (&g_sFileObject);
	}
	
}

/********* Global Fuctions **************/

bool mkdir(char *pathname)
{
	FRESULT fresult;

    //
    // Now creat new directory
    //
    fresult = f_mkdir(pathname);

    if(fresult != FR_OK)
    {
        return	false;
    }
    
    return true;
}
bool rmdir(char *pathname)
{
	FRESULT fresult;
	DIR g_sDirObject;
	
    //
    //Check is it directory
    //
    fresult = f_opendir(&g_sDirObject, pathname);

    if(fresult != FR_OK)
    {
        return false;
    }
    //
    // Now remove directory
    //
    fresult = f_unlink(pathname);

    if(fresult != FR_OK)
    {
        return false;
    }

    return true;
}
bool rm(char *pathname)
{
	FRESULT fresult;

    //
    // Now remove directory
    //
    fresult = f_unlink(pathname);

    if(fresult != FR_OK)
    {
        return false;
    }

    return true;
}

int totalSpace(char *pathName)
{
    unsigned long ulTotalSize;
    FILINFO g_sFileInfo;
    DIR g_sDirObject;
    FRESULT fresult;

    //
    // Open the  directory for access.
    //
    fresult = f_opendir(&g_sDirObject, pathName);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return 0;
    }

    ulTotalSize = 0;

    //
    // Enter loop to enumerate through all directory entries.
    //
    for(;;)
    {
        //
        // Read an entry from the directory.
        //
        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            return 0;
        }

        //
        // If the file name is blank, then this is the end of the
        // listing.
        //
        if(!g_sFileInfo.fname[0])
        {
            break;
        }

        //
        // If the attribue is directory, then increment the directory count.
        //
        if(g_sFileInfo.fattrib & AM_DIR)
        {
        	ulTotalSize +=totalSpace(g_sFileInfo.fname);
        }

        //
        // Otherwise, it is a file.  Increment the file count, and
        // add in the file size to the total.
        //
        else
        {
            ulTotalSize += g_sFileInfo.fsize;
        }
    }   // endfor

    return ulTotalSize;
    
}
int totalSpace()
{
	return totalSpace("/");
}
int freeSpace()
{
    unsigned long freeSpaceSize;
    FRESULT fresult;
    FATFS *pFatFs;
	
    //
    // Get the free space.
    //
    fresult = f_getfree("/", &freeSpaceSize, &pFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return -1;
    }

    return (freeSpaceSize * pFatFs->sects_clust / 2);
}
