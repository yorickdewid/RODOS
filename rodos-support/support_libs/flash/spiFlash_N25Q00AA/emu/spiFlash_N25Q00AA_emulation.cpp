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

SPIFlash_N25Q00AA::SPIFlash_N25Q00AA(HAL_SPI *spi, GPIO_PIN csPin) :
		cs(csPin) {
}

int32_t SPIFlash_N25Q00AA::getNumberOfPages() {
  return NUMBER_OF_PAGES;
}

int32_t SPIFlash_N25Q00AA::getPageSize(uint32_t pageIdx) {
  if(pageIdx >= NUMBER_OF_PAGES)
    return 0;
  return PAGE_USER_SIZE;
}

int32_t SPIFlash_N25Q00AA::readPage(uint32_t pageIdx, void* destination, uint32_t maxLen) {
  return readPageOffset(pageIdx,0,destination,maxLen);
}

int32_t SPIFlash_N25Q00AA::writePage(uint32_t pageIdx, void* source, uint32_t maxLen) {
	return writePageOffset(pageIdx,0,source,maxLen);
}

int32_t SPIFlash_N25Q00AA::erasePage(uint32_t pageIdx) {
  uint8_t ff = 0xff;
  conditionalInitFlashFile();
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE, SEEK_SET);
  for(unsigned int i = 0; i < PAGE_USER_SIZE; i++) fwrite(&ff, 1, 1, flashSimulator);
  fflush(flashSimulator);
  return 1;
}


int32_t SPIFlash_N25Q00AA::readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE + offsetInPage, SEEK_SET);
  fread(destination, MIN(maxLen, PAGE_USER_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_N25Q00AA::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE + offsetInPage, SEEK_SET);
  fwrite(source, MIN(maxLen, PAGE_USER_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_N25Q00AA::eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t maxLen) {
  int32_t retVal = erasePage(pageIdx);
  if ( retVal >= 0){
    Thread::suspendCallerUntil(NOW() + MIN_TIME_TO_ERASE_PAGE * MILLISECONDS);
    return writePage(pageIdx, source, maxLen);
  } else {
    return retVal;
  }
}



