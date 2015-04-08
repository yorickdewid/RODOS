#include "flash.h"
#include "stm32f4xx_flash.h"



INTFlash_STM32F4::INTFlash_STM32F4() {
}



void * INTFlash_STM32F4::getMemoryPointerOffset(uint32_t pageIdx,uint32_t offsetInPage) {
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return NULL; }
  return (void*)(stm32f4_flash_startAddress[pageIdx] + offsetInPage);
}

void * INTFlash_STM32F4::getMemoryPointer(uint32_t pageIdx) {
   return getMemoryPointerOffset(pageIdx,0);
}



/**
 * Read from internal flash memory.
 * @param address 		the source adress
 * @param destination the address read bytes are copied to
 * @param maxLen 			num of bytes to read
 */
int32_t INTFlash_STM32F4::read(uint32_t address, void* destination, uint32_t maxLen) {
  if(!IS_FLASH_ADDRESS(address)) { ERROR("invalid address\n\n"); return ReturnMin-1; }
  memcpy(destination,(uint8_t *)address,maxLen);
  return maxLen;
}

/**
 * Write to internal flash memory.
 * @param address the destination address
 * @param source 	source adress of the data to write
 * @param maxLen	number of bytes to write
 */
int32_t INTFlash_STM32F4::write(uint32_t address, void* source, uint32_t maxLen) {
  if(!IS_FLASH_ADDRESS(address)) { ERROR("invalid address\n\n"); return ReturnMin-1; }

  FLASH_Unlock();

  if((address) % 4 == 0 && (uint32_t)source % 4 == 0){ //Check for alignment
    while(maxLen >= 4){
      FLASH_ProgramWord(address,*((uint32_t*)source));
      address += 4;
      source = (uint8_t *)source + 4;
      maxLen -=4;
    }
  }

  while(maxLen > 0){
    FLASH_ProgramByte(address,*((uint8_t*)source));
    address += 1;
    source = (uint8_t *)source + 1;
    maxLen -=1;
  }

  FLASH_Lock();

  return maxLen;
}


/**
 * Erase page.
 * @param pageIdx Page to erase
 */
int32_t INTFlash_STM32F4::erasePage(uint32_t pageIdx) {
  // Move PageID to right bit position for Flash Control Register
  pageIdx *= 8;

  if(!IS_FLASH_SECTOR(pageIdx)) { ERROR("invalid page\n\n"); return InvalidPage; }
  FLASH_Unlock();
  FLASH_EraseSector(pageIdx, VoltageRange_3);
  FLASH_Lock();

  return Success;
}

/**
 * Return number of pages.
 */
int32_t INTFlash_STM32F4::getNumberOfPages() {
  return NUMBER_OF_PAGES;
}

/**
 * Get page size
 * @param pageIdx the page id
 */
int32_t INTFlash_STM32F4::getPageSize(uint32_t pageIdx){
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return InvalidPage; }
  return stm32f4_flash_pageSize[pageIdx];
}


int32_t INTFlash_STM32F4::readPageOffset(uint32_t pageIdx,  uint32_t offsetInPage, void* destination, uint32_t maxLen){
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return InvalidPage; }
  // Stay in Page Boundaries
  if((int32_t)(maxLen + offsetInPage) > getPageSize(pageIdx))
    maxLen = getPageSize(pageIdx)-offsetInPage;
  read(stm32f4_flash_startAddress[pageIdx] + offsetInPage, destination, maxLen);
  return maxLen;
}

int32_t INTFlash_STM32F4::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen) {
  if(pageIdx >= NUMBER_OF_PAGES) { ERROR("invalid page\n\n"); return InvalidPage; }
  // Stay in Page Boundaries
  if((int32_t)(maxLen + offsetInPage) > getPageSize(pageIdx))
    maxLen = getPageSize(pageIdx)-offsetInPage;
  write(stm32f4_flash_startAddress[pageIdx]+offsetInPage, source, maxLen);
  return maxLen;
}

int32_t INTFlash_STM32F4::eraseAndWritePage (uint32_t pageIdx, void* source, uint32_t maxLen){
  erasePage(pageIdx);
  return writePage(pageIdx, source, maxLen);
}

void INTFlash_STM32F4::init() {
  if(FLASH_GetStatus() != FLASH_COMPLETE)
  {           // clear flash error
    FLASH_Unlock(); // Unlock the Flash Program Erase controller
    FLASH_ClearFlag(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR |
        FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR |
        FLASH_FLAG_OPERR | FLASH_FLAG_EOP); // clear error status
    FLASH_Lock();   // Lock the Flash Program Erase controller
  }
}
