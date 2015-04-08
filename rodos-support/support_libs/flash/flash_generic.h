#pragma once

#include <stdint.h>
//#include "flashconf.h"
#include "rodos.h"

//namespace Common {

/** Abstract class for all flash interfaces.
 *  @author mikschl
 */
class Flash {
protected:
    uint32_t timeoutInMs;

public:
    enum FlashReturnValues{
        Success = 0,
        NoInit = -1, // not initialized or init() failed
        InvalidPage = -2,
        InvalidPageOffset = -3,
        BusErr = -4,
        Timeout = -5,
        NotBlank = -6,
        ReturnMin = NotBlank
    };

    Flash():timeoutInMs(1000){}

    Semaphore protector;

    /** Get the total number of flash pages available
     * @return number of pages 
     */
    virtual int32_t getNumberOfPages() = 0;

    /** Returns the Size of a flashpage"
     * @param pageIdx page (0 ... NumberOfPages-1)
     * @return size of the page in bytes
     */
    virtual int32_t getPageSize(uint32_t pageIdx) = 0;
    
    /** Read flash page
     * @param pageIdx page to read (0 ... NumberOfPages-1)
     * @param destination pointer to the resulting data
     * @param maxLen length of the read data in bytes
     * @return number of bytes read or value <0 on failure
     */
    virtual int32_t readPage(uint32_t pageIdx,  void* destination, uint32_t maxLen){
        return readPageOffset(pageIdx, 0, destination, maxLen);
    }

    /** Write flash page
     * @param pageIdx page to write to (0 ... NumberOfPages-1)
     * @param source pointer to the data
     * @param len length of the data in bytes
     * @return number of bytes written or value <0 on failure
     */
    virtual int32_t writePage(uint32_t pageIdx, void* source, uint32_t len){
        return writePageOffset(pageIdx, 0, source, len);
    }
    
    /** Erase flash page.
     * @param pageIdx page (0 ... NumberOfPages-1)
     */
    virtual int32_t erasePage(uint32_t pageIdx) = 0;

    /** Read in flash page
     * @param pageIdx page to read in (0 ... NumberOfPages-1)
     * @param offsetInPage offset in flash page from the beginning in bytes
     * @param destination pointer to the resulting data
     * @param maxLen length of the read data in bytes
     * @return number of bytes read or value <0 on failure
     */
    virtual int32_t readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen) = 0;

   /** Write in flash page
     * @param pageIdx page to write to (0 ... NumberOfPages-1)
     * @param offsetInPage offset in flash page from the beginning in bytes
     * @param source pointer to the data
     * @param len length of the data in bytes
     * @return number of bytes written or value <0 on failure
     */
    virtual int32_t writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t len) = 0;

    /** First erase and then write to flash page
     * @param pageIdx page to use (0 ... NumberOfPages-1)
     * @param source pointer to the data
     * @param len length of the data in bytes
     * @return number of bytes written or value <0 on failure
     */
    virtual int32_t eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t len) = 0;

    /**
     *  Copys Image from this to another Flash. 
     *  Ignores page boundaries!
     *  May also be used internally, for same flash.
     *  @param fromPageIdx first page of the Image in flash (0 ... NumberOfPages-1)
     *  @param otherFlash other flash
     *  @param toPageIdx target page in the other flash (0 ... NumberOfPages-1)
     *  @param len number of bytes to copy
     */
    int32_t copyToOtherFlash(uint32_t fromPageIdx, Flash *otherFlash, uint32_t toPageIdx, uint32_t len) {
      //use a buffer of 256 byte
      uint8_t data[256];
      uint32_t thisPageOffset = 0;
      uint32_t otherPageOffset = 0;

      if ((int32_t)toPageIdx > getNumberOfPages()) return InvalidPage;
      if ((int32_t)fromPageIdx > getNumberOfPages()) return InvalidPage;

      // Erase first Page in other Flash
      otherFlash->erasePage(toPageIdx);
         
      while(len>0)  {
        uint32_t curSize=0;
        if(len < 256)
          curSize = len;
        else 
          curSize = 256;
          
        // Read Page from this flash
        readPageOffset(fromPageIdx,thisPageOffset,  data, curSize);
        
        thisPageOffset += curSize;
        len -= curSize;

        // Passed the page boarders -> use next page
        if(thisPageOffset > (uint32_t)getPageSize(fromPageIdx)) { // we can cast, because it is already checked, that page is valid
          len += thisPageOffset - getPageSize(fromPageIdx);
          curSize -= thisPageOffset - getPageSize(fromPageIdx);
          thisPageOffset = 0;
          fromPageIdx ++;  
        }
     
        // Write to other flash
        otherFlash->writePageOffset(toPageIdx,otherPageOffset,data,curSize);
        
        otherPageOffset += curSize;

        // Passsed the page boarders -> use next page
        if(otherPageOffset > (uint32_t)otherFlash->getPageSize(toPageIdx)){ // we can cast, because it is already checked, that page is valid
          uint32_t pos = curSize-(otherPageOffset-otherFlash->getPageSize(toPageIdx));
          toPageIdx++;
          otherFlash->eraseAndWritePage(toPageIdx,(data+pos),curSize-pos);
          otherPageOffset = otherPageOffset-otherFlash->getPageSize(toPageIdx-1);
        }
  
      }

      return len;
    }

    /** Checks if a page is completely erased
     * @param pageIdx page to use (0 ... NumberOfPages-1)
     * @return 0 if completely erased or value <0 on failure
     */
    virtual int32_t blankCheck(uint32_t pageIdx = 0, uint8_t blankVal = 0xff){
        return blankCheckOffset(pageIdx,0,blankVal);
    }

    /** Checks if a page is erased starting at offsetInPage
     * @param pageIdx page to use (0 ... NumberOfPages-1)
     * @param offsetInPage offset in flash page from the beginning in bytes
     * @param blankVal value of memory, when it is erased
     * @return 0 if completely erased or value <0 on failure
     */
    virtual int32_t blankCheckOffset(uint32_t pageIdx = 0, uint32_t offsetInPage = 0, uint8_t blankVal = 0xff){
        uint8_t compareBuf[10] = {0};
        memset(compareBuf,blankVal,sizeof(compareBuf));

        if ((int32_t)pageIdx > getNumberOfPages()-1) {
            return InvalidPage;
        }

        if (offsetInPage >= (uint32_t)getPageSize(pageIdx) - 1) { // we can cast, because it is already checked, that page is valid
            return InvalidPageOffset;
        }

        uint32_t pageSize = getPageSize(pageIdx);
        uint32_t bytesToRead = pageSize - offsetInPage;
        int64_t startTime = NOW();
        while(bytesToRead > 0){
            uint8_t buf[sizeof(compareBuf)] = {0};
            int32_t retVal = readPageOffset(pageIdx, pageSize-bytesToRead, buf, sizeof(buf));

            if (retVal >= 0){
                bytesToRead -= retVal;
                if (memcmp(buf,compareBuf,retVal) != 0) {
                    return NotBlank;
                }
            }else{
                return retVal;
            }

            if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
        }

        return Success;
    }
};

/** Flashinterfaces available - see on-"arch" subdirectory for implemantation */
//extern Flash *spiFlash;
//extern Flash *internalFlash;

//}

