/**
 * @file spiFlash_AT45DBxxx.h
 * @date 2013/05/20
 * @author Michael Ruffer, Cláudio Sousa
 *
 * @brief driver for spi NOR-flashes AT45DBxxx (tested devices: AT45DB321D, AT45DB161)
 *
 */

#ifndef _FLASH_AT45DBXXX_H_
#define _FLASH_AT45DBXXX_H_

#include "hal.h"

class SPIFlash_AT45DBxxx: public Flash {
public:
	/** parameters of the external flash*/
	static const uint32_t PAGE_USER_SIZE = 512;  // please correct
	static const uint32_t PAGE_SPARE_SIZE = 16;  // please correct
	static const uint32_t PAGE_TOTAL_SIZE = PAGE_USER_SIZE + PAGE_SPARE_SIZE;
	static const uint32_t NUMBER_OF_PAGES = 4096; // AT45DB161
	//static const uint32_t NUMBER_OF_PAGES = 8192; // AT45DB321
	static const uint32_t TOTAL_USER_SIZE = NUMBER_OF_PAGES * PAGE_USER_SIZE;
	static const uint32_t TIME_PAGE_ERASE = 35; // Time in Milliseconds
	//static const uint32_t TIME_PAGE_PROG = 6; // Time in Milliseconds
	//static const uint32_t TIME_PAGE_ERASE_AND_PROG = 40; // Time in Milliseconds

private:
	HAL_GPIO *cs;
	HAL_SPI *spi;
	bool initialized;
	uint8_t pageBuffer[PAGE_TOTAL_SIZE];
//	uint32_t timeoutInMs;  // max. time after all functions must return (default 1s)

	int32_t checkPageIdx(uint32_t pageIdx);
	int32_t checkPageOffset(uint32_t pageOffset);

public:
	SPIFlash_AT45DBxxx(HAL_SPI *spi, HAL_GPIO *cs);

	/*
	 * device specific
	 */
	uint16_t getDeviceID();							// returns device ID part1&2
	uint8_t getManufacturerID();						// returns manufacturer ID
	uint8_t getExtendedDeviceInfoLen(); 				// returns length of extended device infos
	int32_t readID(uint8_t* rdBuf, uint16_t maxLen); 	// max. len of device info = 4 byte (IDs,Len) + 255 bytes (ext. dev. info)

	int32_t init();
	void setTimeout(uint32_t timeoutInMs);
	bool busy();

    int32_t eraseChip(); // (1) be careful using this function. device will be busy up to 35s.

	/*
	 * generic flash interface
	 */
    int32_t getNumberOfPages();
    int32_t getPageSize(uint32_t pageIdx);

	int32_t readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen); // (1)

	int32_t writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen); // (1)

	int32_t eraseAndWritePage(uint32_t pageIdx, void* source, uint32_t maxLen); // (1)

	int32_t erasePage(uint32_t pageIdx); // (1)
};

/* (1)
 * - function need some processing time on flash chip
 * - function will wait on ENTRY if device is busy
 */




/* Opcodes for AT45DBxxx
 * not all instructions of the chip are here,
 * only the used ones and some extra from the protection section*/

//control,protection,status...
#define RDSR        0xD7    //read status register
#define RDID        0x9F    //read jedec ID
#define RDSPR       0x32    //read sector protection register
#define SP1         0x3D    //sector protection opcodes
#define SP2         0x2A
#define SP3         0x7F
#define SP4_EN      0xA9    //enable sector protection
#define SP4_DIS     0x9A    //disable sector protection
#define SP4_ERASE   0xCF    //erase sector protection register
#define SP4_PROG    0xFC    //program sector protection register
//read
#define PAGE_READ   0xD2    //direct read from main memory
//write/program
#define BUF1_WR     0x84    //write to buffer 1
#define BUF2_WR     0x87    //write to buffer 2
#define BF1_MM      0x88    //write buffer 1 to main memory (without built in erase)
#define BF2_MM      0x89    //write buffer 2 to main memory (without built in erase)
//erase
#define ER_PAGE     0x81    //erase page
#define ER_BLOCK    0x50    //erase block
#define ER_CHIP1    0xC7    //erase chip
#define ER_CHIP2    0x94    //erase chip
#define ER_CHIP3    0x80    //erase chip
#define ER_CHIP4    0x9A    //erase chip



#define MANUFACTURER_ID 0x1F
#define FAMILY_CODE 0x01
#define DENSITY_CODE 0x07

#endif /* _FLASH_AT45DBXXX_H_ */
