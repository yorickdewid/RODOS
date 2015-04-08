#include <stdio.h>
#include "flash.h"

#define FLASH_SIZE 1048576  // = 1 Megabyte

static FILE *internalFlashSimulator = 0;
uint8_t flashContents[FLASH_SIZE];

static void conditionalInitInternalFlashFile() {
  if(!internalFlashSimulator) internalFlashSimulator = fopen("internal_flash.bin", "r+");
  if(!internalFlashSimulator) internalFlashSimulator = fopen("internal_flash.bin", "w+");
  for(int i = 0;i<FLASH_SIZE;i++) {
    flashContents[i]=0xff;
  }
  rewind(internalFlashSimulator);
  fread(flashContents, FLASH_SIZE, 1, internalFlashSimulator);
}

INTFlash_STM32F4::INTFlash_STM32F4() {
}

void * INTFlash_STM32F4::getMemoryPointerOffset(uint32_t pageIdx,uint32_t offsetInPage) {
  conditionalInitInternalFlashFile();
  return (void*)((stm32f4_flash_startAddress[pageIdx] + offsetInPage - 0x08000000)+(uint32_t)flashContents);
}

void * INTFlash_STM32F4::getMemoryPointer(uint32_t pageIdx) {
  return getMemoryPointerOffset(pageIdx,0);
}



int32_t INTFlash_STM32F4::read(uint32_t address, void* destination, uint32_t maxLen) {
  conditionalInitInternalFlashFile();
  fseek (internalFlashSimulator, address - 0x08000000, SEEK_SET);
  fread(destination, maxLen, 1, internalFlashSimulator);
  fflush(internalFlashSimulator);
  
  return maxLen;
}

int32_t INTFlash_STM32F4::write(uint32_t address, void* source, uint32_t maxLen) {
  conditionalInitInternalFlashFile();
  fseek (internalFlashSimulator, address-0x08000000, SEEK_SET);
  fwrite(source, maxLen, 1, internalFlashSimulator);
  fflush(internalFlashSimulator);
  memcpy(&flashContents[address-0x08000000],source,maxLen);
  return maxLen;
}


int32_t INTFlash_STM32F4::getNumberOfPages() {
  return NUMBER_OF_PAGES;
}

int32_t INTFlash_STM32F4::getPageSize(uint32_t pageIdx){
  return stm32f4_flash_pageSize[pageIdx];
}
    
/*int32_t INTFlash_STM32F4::readPage(uint32_t pageIdx,  void* destination, uint32_t maxLen) {
  return readPageOffset(pageIdx,0, destination, maxLen);
}
   

int32_t INTFlash_STM32F4::writePage(uint32_t pageIdx, void* source, uint32_t maxLen) {
  return writePageOffset(pageIdx,0, source, maxLen);
}*/
   

int32_t INTFlash_STM32F4::readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen){
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return -1; }
  // Stay in Page Boundaries  
  if((int32_t)(maxLen + offsetInPage) > getPageSize(pageIdx))
    maxLen = getPageSize(pageIdx)-offsetInPage;
  return read(stm32f4_flash_startAddress[pageIdx] + offsetInPage, destination, maxLen);
}
 

int32_t INTFlash_STM32F4::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen) {
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return -1; }
  // Stay in Page Boundaries  
  if((int32_t)(maxLen + offsetInPage) > getPageSize(pageIdx))
    maxLen = getPageSize(pageIdx)-offsetInPage;
  return write(stm32f4_flash_startAddress[pageIdx]+offsetInPage, source, maxLen);;
}

int32_t INTFlash_STM32F4::erasePage(uint32_t pageIdx) {
  uint8_t ff = 0xff;
  conditionalInitInternalFlashFile();
  fseek (internalFlashSimulator, stm32f4_flash_startAddress[pageIdx]-0x08000000, SEEK_SET);
  for(unsigned int i = 0; i < stm32f4_flash_pageSize[pageIdx]; i++) {
    fwrite(&ff, 1, 1, internalFlashSimulator);
    flashContents[i+stm32f4_flash_startAddress[pageIdx]-0x08000000]=0xff;
  }
  fflush(internalFlashSimulator);
  return 1;
}

int32_t INTFlash_STM32F4::eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t maxLen){
  int32_t retVal = erasePage(pageIdx);
  if ( retVal == 0){
    return writePage(pageIdx, source, maxLen);
  } else {
    return retVal;
  }
}

