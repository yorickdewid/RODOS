#include <stdio.h>
#include "flash.h"
#include "macros_bits_bytes.h"
// #define MIN(a,b) ( (a)<(b)? (a) : (b) )

static FILE *flashSimulator=0;

/** open file as flash simulator*/
static void conditionalInitFlashFile() {
  if(!flashSimulator) flashSimulator = fopen("spi_flash.bin", "r+");
  if(!flashSimulator) flashSimulator = fopen("spi_flash.bin", "w+");
}

SPIFlash_AT45DBxxx::SPIFlash_AT45DBxxx(HAL_SPI * spi, HAL_GPIO * cs) { }

int32_t SPIFlash_AT45DBxxx::getNumberOfPages() {
  return NUMBER_OF_PAGES;
}

int32_t SPIFlash_AT45DBxxx::getPageSize(uint32_t pageIdx) {
  if(pageIdx >= NUMBER_OF_PAGES)
    return 0;
  return PAGE_USER_SIZE;
}

/*int32_t SPIFlash_AT45DBxxx::readPage(uint32_t pageIdx, void* destination, uint32_t maxLen) {
  conditionalInitFlashFile();
  fseek (flashSimulator, pageIdx*PAGE_TOTAL_SIZE, SEEK_SET);
  fread(destination, MIN(maxLen, PAGE_TOTAL_SIZE), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_AT45DBxxx::writePage(uint32_t pageIdx, void* source, uint32_t maxLen) {
  conditionalInitFlashFile();
  fseek (flashSimulator, pageIdx*PAGE_TOTAL_SIZE, SEEK_SET);
  fwrite(source, MIN(maxLen, PAGE_TOTAL_SIZE), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}*/

int32_t SPIFlash_AT45DBxxx::erasePage(uint32_t pageIdx) {
  uint8_t ff = 0xff;
  conditionalInitFlashFile();
  fseek (flashSimulator, pageIdx*PAGE_TOTAL_SIZE, SEEK_SET);
  for(unsigned int i = 0; i < PAGE_TOTAL_SIZE; i++) fwrite(&ff, 1, 1, flashSimulator);
  fflush(flashSimulator);
  return 1;
}


int32_t SPIFlash_AT45DBxxx::readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_TOTAL_SIZE + offsetInPage, SEEK_SET);
  fread(destination, MIN(maxLen, PAGE_TOTAL_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_AT45DBxxx::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_TOTAL_SIZE + offsetInPage, SEEK_SET);
  fwrite(source, MIN(maxLen, PAGE_TOTAL_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_AT45DBxxx::eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t maxLen) {
  int32_t retVal = erasePage(pageIdx);
  if ( retVal >= 0){
    Thread::suspendCallerUntil(NOW() + TIME_PAGE_ERASE * MILLISECONDS);
    return writePage(pageIdx, source, maxLen);
  } else {
    return retVal;
  }
}

