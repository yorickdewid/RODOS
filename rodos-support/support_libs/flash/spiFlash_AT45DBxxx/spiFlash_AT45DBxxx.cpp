/**
 * @file spiFlash_AT45DBxxx.cpp
 * @date 2013/05/20
 * @author Michael Ruffer, Cláudio Sousa
 *
 * @brief driver for spi NOR-flash devices ADESTO AT45DBxxx
 *
 */

#include "flash.h"
#include "checksumes.h"
#include "string_pico.h"

// TODO: - while busy !!! Endlosscheife -> bessere Loesung?

/*STATUS REGISTER
 *
 * bit:		 -----7------------6--------5-------4-------3-------2--------1--------------0-------
 * meaning:	|	Ready	|	Compare	|	1	|	1	|	0	|	1	|	Prot	|	Page Size	|
 * 			 -----------------------------------------------------------------------------------
 * */

SPIFlash_AT45DBxxx::SPIFlash_AT45DBxxx(HAL_SPI * spi, HAL_GPIO * cs) {
	this->spi = spi;
	this->cs = cs;
	initialized = false;
	timeoutInMs = 1000;
}


int32_t SPIFlash_AT45DBxxx::getNumberOfPages() {
	return NUMBER_OF_PAGES;
}


int32_t SPIFlash_AT45DBxxx::getPageSize(uint32_t pageIdx) {
    if (pageIdx>=NUMBER_OF_PAGES) return InvalidPage;
	return PAGE_TOTAL_SIZE;
}


int32_t SPIFlash_AT45DBxxx::init() {
	//spi->init();
	//cs->init(true, 1, 1);

	uint8_t manID = this->getManufacturerID();
	uint8_t familyCode =  this->getDeviceID() >> 13;
	if ((manID == MANUFACTURER_ID) && (familyCode == FAMILY_CODE)){
		initialized = true;
		return Success;
	}else{
		initialized = false;
		return NoInit;
	}
}


void SPIFlash_AT45DBxxx::setTimeout(uint32_t _timeoutInMs) {
    timeoutInMs = _timeoutInMs;
}


// STATUS REGISTER
//
// bit:      -----7------------6--------5-------4-------3-------2--------1--------------0-------
// meaning: | Rdy/Busy  |   Compare |   1   |   1   |   0   |   1   |   Prot    |   Page Size   |
//           -----------------------------------------------------------------------------------
//
bool SPIFlash_AT45DBxxx::busy() {
	if (!initialized) return false;

	uint8_t result[2];
	uint8_t send[2];
	send[0] = RDSR;

	cs->setPins(0);
	spi->write(send, 1);
	spi->read(result, 2);
	cs->setPins(1);
	if ((result[1] & 0x80) == 0x80) {
		return false;	// ready
	}

	return true;		// busy
}


int32_t SPIFlash_AT45DBxxx::readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen) {
	if (!initialized) return NoInit;

    if (checkPageOffset(offsetInPage) != Success) {
        return InvalidPageOffset;
    }

	if (checkPageIdx(pageIdx) != Success) {
		return InvalidPage;
	}

	if (maxLen + offsetInPage > PAGE_TOTAL_SIZE) {
		maxLen = PAGE_TOTAL_SIZE - offsetInPage;
	}

	int64_t startTime = NOW();
	while (busy()){ //wait for previous operation
	    if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
	}

	/*start reading*/
	/*address:
	 * 23 bit address for 528 byte page size
	 * bit 23 (MSB) - don't care
	 * [22-10] 13 bits for page address (0 to 8191)
	 * [9-0] 10 bits for start address within the page*/
	uint8_t command[8];
	command[0] = PAGE_READ; 		//instruction
	command[1] = (pageIdx >> 6) & 0x7F; 	//7 MSB pageidx
	command[2] = (((pageIdx << 2) & 0xFC) | ((offsetInPage >> 8) & 0x03)); //6 LSB pageidx and 2 MSB byte address
	command[3] = offsetInPage & 0xFF; 		//8 LSB byte address

	cs->setPins(0);
	spi->write(command, 8); // 1 byte opcode, 3 byte address, 4 bytes don't care
	spi->read((uint8_t*)destination, maxLen);
	cs->setPins(1);

	return maxLen;
}


int32_t SPIFlash_AT45DBxxx::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t len) {
	if (!initialized) return NoInit;

	/*fill a buffer with the size of a page with 0xFF, change the bytes we want to write and write it to flash*/

	/*address:
	 * 23 bit address for 528 byte page size
	 * bit 23 (MSB) - don't care
	 * [22-10] 13 MSBs don't care or for page address (0 to 8191) for write to memory
	 * [9-0] 10 LSBs for start address within the buffer or don't care for write to memory*/
	if (checkPageOffset(offsetInPage) != Success) {
		return InvalidPageOffset;
	}

	if (checkPageIdx(pageIdx) != Success) {
	    return InvalidPage;
	}

	if (len + offsetInPage > PAGE_TOTAL_SIZE) {
		len = PAGE_TOTAL_SIZE - offsetInPage;
	}

	// prepare page buffer for write process
	memset(pageBuffer, 0xff, PAGE_TOTAL_SIZE); // 0xff -> so we will not overwrite existing data
	memcpy(&pageBuffer[offsetInPage], source, len);

	uint8_t command[4];
	command[0] = BUF1_WR;
	command[1] = 0; 		//don't care
	command[2] = 0;			//don't care + byte address (not used)
	command[3] = 0; 		//byte address (not used)

    int64_t startTime = NOW();
    while (busy()){ //wait for previous operation
        if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
    }

	cs->setPins(0);
	spi->write(command, 4); // 1 byte opcode, 3 byte address, maxlen bytes data
	spi->write(pageBuffer, PAGE_TOTAL_SIZE); // 1 byte opcode, 3 byte address, maxlen bytes data
	cs->setPins(1);

	//write buffer to memory
	command[0] = BF1_MM;
	command[1] = (pageIdx >> 6) & 0x7F; 	//1 bit don't care, 7 MSB pageidx
	command[2] = (pageIdx << 2) & 0xFC;	//6 LSB pageidx, 2 bit don't care
	command[3] = 0; 		//8 bit don't care

    startTime = NOW();
    while (busy()){ //wait for previous operation
        if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
    }

	cs->setPins(0);
	spi->write(command, 4); // 1 byte opcode, 3 byte address
	cs->setPins(1);

	return len;
}


int32_t SPIFlash_AT45DBxxx::eraseAndWritePage(uint32_t pageIdx, void* source,uint32_t maxLen) {
	int32_t retVal = erasePage(pageIdx);

    if ( retVal == 0){
        uint32_t timeOut = 0;
        while (busy()){
            timeOut++;
            Thread::suspendCallerUntil(NOW()+ 5 * MILLISECONDS);
            if (timeOut*5 > TIME_PAGE_ERASE*2) return Timeout; // timeout after twice the erase time
        }

        return writePage(pageIdx, source, maxLen);
    }else{
        return retVal;
    }
}


int32_t SPIFlash_AT45DBxxx::erasePage(uint32_t pageIdx) {
    if (!initialized) return NoInit;

    if ( checkPageIdx(pageIdx) != Success) {
        return InvalidPage;
    }

    uint8_t command[4]; //8bit instruction + 24 bit address
    command[0] = ER_PAGE;
    command[1] = (pageIdx >> 6) & 0x7F;     //1 bit don't care, 7 MSB pageidx
    command[2] = (pageIdx << 2) & 0xFC; //6 LSB pageidx, 2 bit byte address
    command[3] = 0;         //8 bit byte address

    int64_t startTime = NOW();
    while (busy()){ //wait for previous operation
        if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
    }

    cs->setPins(0);
    spi->write(command, 4); // 1 byte opcode, 3 byte address
    cs->setPins(1);

    return Success;
}


int32_t SPIFlash_AT45DBxxx::eraseChip(){
    if (!initialized) return NoInit;

    int64_t startTime = NOW();
    while (busy()){ //wait for previous operation
        if (NOW()-startTime > timeoutInMs*MILLISECONDS) return Timeout;
    }

    uint8_t command[4];
    command[0] = ER_CHIP1;
    command[1] = ER_CHIP2;
    command[2] = ER_CHIP3;
    command[3] = ER_CHIP4;

    cs->setPins(0);
    spi->write(command, 4); // 1 byte opcode, 3 byte address
    cs->setPins(1);

    return Success;
}


int32_t SPIFlash_AT45DBxxx::readID(uint8_t* rdBuf, uint16_t size) {
	uint8_t command = RDID;

	cs->setPins(0);
	spi->write(&command, 1); // 1 byte opcode
	spi->read(rdBuf, size);
	cs->setPins(1);

	return size;
}


uint16_t SPIFlash_AT45DBxxx::getDeviceID() {
	uint8_t rdBuf[3];

	readID(rdBuf,sizeof(rdBuf));

	return (rdBuf[1] << 8) | rdBuf[2];
}


uint8_t SPIFlash_AT45DBxxx::getManufacturerID() {
	uint8_t manID = 0;

	readID(&manID,1);

	return manID;
}


uint8_t SPIFlash_AT45DBxxx::getExtendedDeviceInfoLen() {
	uint8_t rdBuf[4];

	readID(rdBuf,sizeof(rdBuf));

	return rdBuf[3];
}


int32_t SPIFlash_AT45DBxxx::checkPageIdx(uint32_t pageIdx){
    if (pageIdx > NUMBER_OF_PAGES - 1) {
        return InvalidPage;
    }else{
        return Success;
    }
}


int32_t SPIFlash_AT45DBxxx::checkPageOffset(uint32_t pageOffset){
    if (pageOffset > PAGE_TOTAL_SIZE - 1) {
        return InvalidPageOffset;
    }else{
        return Success;
    }
}
