#ifndef _FLASH_S70FL01GS_H_
#define _FLASH_S70FL01GS_H_

#include "rodos.h"
#include "hal.h"
#include "spiFlash_S25FL512S.h"


class SPIFlash_S70FL01GS: public Flash {

private:
	HAL_GPIO cs1;
	HAL_GPIO cs2;
	HAL_SPI *spi;

	SPIFlash_S25FL512S flash1;
	SPIFlash_S25FL512S flash2;

public:
	SPIFlash_S70FL01GS(HAL_SPI *spi, GPIO_PIN pincs1, GPIO_PIN pincs2);
	int32_t getNumberOfPages();
	int32_t getPageSize(uint32_t pageIdx);
	int32_t init();

	int32_t readPage(uint32_t pageIdx, void *destination, uint32_t maxLen);
	int32_t writePage(uint32_t pageIdx, void *source, uint32_t maxLen);
	int32_t erasePage(uint32_t pageIdx);
	int32_t readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen);
	int32_t writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen);
	int32_t eraseAndWritePage(uint32_t pageIdx, void* source, uint32_t maxLen);
};

#endif /* _FLASH_S70FL01GS_H_ */
