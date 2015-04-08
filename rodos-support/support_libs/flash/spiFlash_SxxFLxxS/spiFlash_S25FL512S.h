#ifndef _FLASH_S25FL512S_H_
#define _FLASH_S25FL512S_H_


#include "hal.h"
#include "flash/flash_generic.h"



class SPIFlash_S25FL512S: public Flash {
public:
	/** parameters of the external flash*/
	static const uint32_t PAGE_USER_SIZE = 262144;  // 256kB
	static const uint32_t WRITE_BLOCK_SIZE = 512;
	static const uint32_t NUMBER_OF_PAGES = 256;
	static const uint32_t TOTAL_USER_SIZE = NUMBER_OF_PAGES * PAGE_USER_SIZE;
	static const uint32_t MIN_TIME_TO_ERASE_PAGE = 250; // Time in Milliseconds

private:
	HAL_GPIO *cs;
	HAL_SPI *spi;
	bool initialized;
	enum {OP_EREASE,OP_WRITE,OP_OTHER} lastOp;

	bool busy();
	void waitForFlash();
	void setWREN();

public:
	SPIFlash_S25FL512S(HAL_SPI *spi, HAL_GPIO *cs);
	int32_t getNumberOfPages();
	int32_t getPageSize(uint32_t pageIdx);

	uint16_t getDeviceID();							// returns device ID part1&2
	uint8_t getManufacturerID();						// returns manufacturer ID
	uint8_t getExtendedDeviceInfoLen(); 				// returns length of extended device infos
	int32_t readID(uint8_t* rdBuf, uint16_t maxLen); 	// max. len of device info = 4 byte (IDs,Len) + 255 bytes (ext. dev. info)
	int32_t init();



	int32_t readPage(uint32_t pageIdx, void *destination, uint32_t maxLen);
	int32_t writePage(uint32_t pageIdx, void *source, uint32_t maxLen);
	int32_t erasePage(uint32_t pageIdx);
	int32_t readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen);
	int32_t writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen);
	int32_t eraseAndWritePage(uint32_t pageIdx, void* source, uint32_t maxLen);
};

#endif /* _FLASH_S25FL512S_H_ */

