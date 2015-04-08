#include <stdio.h>
#include "flash.h"
#include "macros_bits_bytes.h"
// #define MIN(a,b) ( (a)<(b)? (a) : (b) )

	/** parameters of the external flash*/
	static const uint32_t PAGE_USER_SIZE = 262144;  // 256kB
	//static const uint32_t WRITE_BLOCK_SIZE = 512;
	static const uint32_t NUMBER_OF_PAGES = 512;
	//static const uint32_t TOTAL_USER_SIZE = NUMBER_OF_PAGES * PAGE_USER_SIZE;
	static const uint32_t TIME_TO_ERASE_PAGE = 250; // Time in Milliseconds

static FILE *flashSimulator=0;

/** open file as flash simulator*/
static void conditionalInitFlashFile() {
  if(!flashSimulator) flashSimulator = fopen("spi_flash.bin", "r+");
  if(!flashSimulator) flashSimulator = fopen("spi_flash.bin", "w+");
}

SPIFlash_S70FL01GS::SPIFlash_S70FL01GS(HAL_SPI *spi, GPIO_PIN pincs1, GPIO_PIN pincs2) :
		cs1(pincs1),cs2(pincs2),
		flash1(spi,&cs1),flash2(spi,&cs2){
}

int32_t SPIFlash_S70FL01GS::getNumberOfPages() {
  return NUMBER_OF_PAGES;
}

int32_t SPIFlash_S70FL01GS::getPageSize(uint32_t pageIdx) {
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

int32_t SPIFlash_S70FL01GS::erasePage(uint32_t pageIdx) {
  uint8_t ff = 0xff;
  conditionalInitFlashFile();
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE, SEEK_SET);
  for(unsigned int i = 0; i < PAGE_USER_SIZE; i++) fwrite(&ff, 1, 1, flashSimulator);
  fflush(flashSimulator);
  return 1;
}


int32_t SPIFlash_S70FL01GS::readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE + offsetInPage, SEEK_SET);
  fread(destination, MIN(maxLen, PAGE_USER_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_S70FL01GS::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen){
  conditionalInitFlashFile();
  if(offsetInPage > PAGE_USER_SIZE-1) return 0;
  fseek (flashSimulator, pageIdx*PAGE_USER_SIZE + offsetInPage, SEEK_SET);
  fwrite(source, MIN(maxLen, PAGE_USER_SIZE-offsetInPage), 1, flashSimulator);
  fflush(flashSimulator);
  return maxLen;
}

int32_t SPIFlash_S70FL01GS::eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t maxLen) {
  int32_t retVal = erasePage(pageIdx);
  if ( retVal >= 0){
    Thread::suspendCallerUntil(NOW() + TIME_TO_ERASE_PAGE * MILLISECONDS);
    return writePage(pageIdx, source, maxLen);
  } else {
    return retVal;
  }
}




SPIFlash_S25FL512S::SPIFlash_S25FL512S(HAL_SPI * spi, HAL_GPIO * cs) { }
int32_t SPIFlash_S25FL512S::getNumberOfPages() { return 0; }
int32_t SPIFlash_S25FL512S::getPageSize(uint32_t pageIdx) { return 0; }
int32_t SPIFlash_S25FL512S::init() { return 0;}
bool SPIFlash_S25FL512S::busy() { return false;}
void SPIFlash_S25FL512S::setWREN(){ }
void SPIFlash_S25FL512S::waitForFlash(){ }
int32_t SPIFlash_S25FL512S::erasePage(uint32_t pageIdx) {return 0; }
int32_t SPIFlash_S25FL512S::readPage(uint32_t pageIdx, void *destination, uint32_t maxLen) {return 0; }
int32_t SPIFlash_S25FL512S::readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen) { return 0;}
int32_t SPIFlash_S25FL512S::writePage(uint32_t pageIdx, void *source, uint32_t len) { return 0;}
int32_t SPIFlash_S25FL512S::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t len) { return 0;}
int32_t SPIFlash_S25FL512S::eraseAndWritePage(uint32_t pageIdx, void* source,uint32_t maxLen) {return 0; }
int32_t SPIFlash_S25FL512S::readID(uint8_t* rdBuf, uint16_t size) {return 0; }
uint16_t SPIFlash_S25FL512S::getDeviceID() { return 0;}
uint8_t SPIFlash_S25FL512S::getManufacturerID() { return 0;}
uint8_t SPIFlash_S25FL512S::getExtendedDeviceInfoLen() { return 0;}


