#ifndef _FLASH_N25Q00AA_H_
#define _FLASH_N25Q00AA_H_


#include "hal.h"
#include "flash/flash_generic.h"



class SPIFlash_N25Q00AA: public Flash {
public:
	/** parameters of the external flash*/
	static const uint32_t PAGE_USER_SIZE = 4096;  // 4kB
	static const uint32_t WRITE_BLOCK_SIZE = 256;
	static const uint32_t NUMBER_OF_PAGES = 32768;
	static const uint32_t TOTAL_USER_SIZE = NUMBER_OF_PAGES * PAGE_USER_SIZE;
	static const uint32_t MIN_TIME_TO_ERASE_PAGE = 200; // Time in Milliseconds

private:
	HAL_GPIO cs;
	HAL_SPI *spi;
	bool initialized;
	enum {OP_EREASE,OP_WRITE,OP_OTHER} lastOp;

	bool busy();
	void waitForFlash();
	void setWREN();
	void set4ByteAddr();

public:
	SPIFlash_N25Q00AA(HAL_SPI *spi, GPIO_PIN csPin);
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

#endif /* _FLASH_N25Q00AA_H_ */

