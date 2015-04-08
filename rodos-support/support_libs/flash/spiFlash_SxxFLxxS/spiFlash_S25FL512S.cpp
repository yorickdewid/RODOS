#include "spiFlash_S25FL512S.h"

#include "rodos.h"


#define CMD_WREN  0x06
#define CMD_RDSR1 0x05
#define CMD_4READ 0x13
#define CMD_4PP   0x12
#define CMD_4SE   0xDC
#define CMD_RDID  0x9F

#define MANUFACTURER_ID 0x01
#define DEVICE_ID       0x0220


SPIFlash_S25FL512S::SPIFlash_S25FL512S(HAL_SPI * spi, HAL_GPIO * cs) {
	this->spi = spi;
	this->cs = cs;
	initialized = false;
	lastOp=OP_OTHER;
}

int32_t SPIFlash_S25FL512S::getNumberOfPages() {
	return NUMBER_OF_PAGES;
}

int32_t SPIFlash_S25FL512S::getPageSize(uint32_t pageIdx) {
	return PAGE_USER_SIZE;
}

int32_t SPIFlash_S25FL512S::init() {
	//spi->init();
	//cs->init(true, 1, 1);

	uint8_t manID = this->getManufacturerID();
	uint16_t devID =  this->getDeviceID();
	if ((manID == MANUFACTURER_ID) && (devID == DEVICE_ID)){
		initialized = true;
		return 0;
	}else{
		initialized = false;
		return -1;
	}
}

bool SPIFlash_S25FL512S::busy() {
	if (!initialized) return false;

	uint8_t result;
	uint8_t send;
	send = CMD_RDSR1;

	cs->setPins(0);
	spi->write(&send, 1);
	spi->read(&result, 1);
	cs->setPins(1);

	return (result & 0x01) == 1; //if WIP==1 => busy
}

void SPIFlash_S25FL512S::setWREN(){
	uint8_t send;
	send = CMD_WREN;
	waitForFlash();
	cs->setPins(0);
	spi->write(&send, 1);
	cs->setPins(1);
}



void SPIFlash_S25FL512S::waitForFlash(){
	while(busy()){
		switch(lastOp){
		case OP_EREASE:
			Thread::suspendCallerUntil(NOW()+50*MILLISECONDS);
			break;
		case OP_WRITE:
		default:
			Thread::yield();
		}
	}
	lastOp=OP_OTHER;
}



int32_t SPIFlash_S25FL512S::erasePage(uint32_t pageIdx) {
	if (!initialized) return -1;

	if (pageIdx > NUMBER_OF_PAGES - 1) {
		ERROR("invalid page\n\n");
		return -2;
	}

	setWREN();

	uint32_t addr = pageIdx*PAGE_USER_SIZE;
	uint8_t command[5];
	command[0] = CMD_4SE;		//instruction
	command[1] = (addr >> 24) & 0xFF;
	command[2] = (addr >> 16) & 0xFF;
	command[3] = (addr >> 8 ) & 0xFF;
	command[4] =  addr        & 0xFF;

	waitForFlash();
	cs->setPins(0);
	spi->write(command, 5); // 1 byte opcode, 4 byte address
	cs->setPins(1);

	lastOp=OP_EREASE;
	return 0;
}

int32_t SPIFlash_S25FL512S::readPage(uint32_t pageIdx, void *destination, uint32_t maxLen) {
	return readPageOffset(pageIdx,0,destination,maxLen);
}

int32_t SPIFlash_S25FL512S::readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen) {
	if (!initialized) return -1;

	if (offsetInPage +maxLen > PAGE_USER_SIZE) {
		ERROR("offset and/or size is higher than page's last byte\n");
		offsetInPage = 0;
	}

	if (pageIdx > NUMBER_OF_PAGES - 1) {
		ERROR("invalid page\n\n");
		return 0;
	}

	waitForFlash(); //wait for previous operation

	/*start reading*/
	uint32_t addr= pageIdx*PAGE_USER_SIZE + offsetInPage;

	uint8_t command[5];
	command[0] = CMD_4READ;		//instruction
	command[1] = (addr >> 24) & 0xFF;
	command[2] = (addr >> 16) & 0xFF;
	command[3] = (addr >> 8 ) & 0xFF;
	command[4] =  addr        & 0xFF;

	cs->setPins(0);
	spi->write(command, 5); // 1 byte opcode,  4 byte address
	spi->read((uint8_t*)destination, maxLen);
	cs->setPins(1);

	lastOp=OP_OTHER;
	return maxLen;
}


int32_t SPIFlash_S25FL512S::writePage(uint32_t pageIdx, void *source, uint32_t len) {
	return writePageOffset(pageIdx,0,source,len);
}

int32_t SPIFlash_S25FL512S::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t len) {
	if (!initialized) return -1;

	if (offsetInPage + len > PAGE_USER_SIZE) {
		ERROR("offset and/or len is higher than page's last byte\n");
		return -1;
	}

	if (pageIdx > NUMBER_OF_PAGES - 1) {
		ERROR("invalid page\n\n");
		return 0;
	}


	//we may only write in 512 byte aligend memory reginos with one command

	uint32_t addr= pageIdx*PAGE_USER_SIZE + offsetInPage;
	uint32_t bytesLeft=len;
	uint8_t* writeData = (uint8_t*)source;

	while(bytesLeft>0){
		uint32_t currentBlockSize = WRITE_BLOCK_SIZE - (addr % WRITE_BLOCK_SIZE);
		currentBlockSize = (currentBlockSize > bytesLeft) ? bytesLeft : currentBlockSize;

		setWREN();

		uint8_t command[5];
		command[0] = CMD_4PP;		//instruction
		command[1] = (addr >> 24) & 0xFF;
		command[2] = (addr >> 16) & 0xFF;
		command[3] = (addr >> 8 ) & 0xFF;
		command[4] =  addr        & 0xFF;

		waitForFlash();
		cs->setPins(0);
		spi->write(command, 5); // 1 byte opcode, 4 byte address, maxlen bytes data
		spi->write(writeData, currentBlockSize); // 1 byte opcode, 4 byte address, maxlen bytes data
		cs->setPins(1);
		lastOp=OP_WRITE;

		writeData += currentBlockSize;
		bytesLeft -= currentBlockSize;
		addr 	  += currentBlockSize;
	}

	return len;
}

int32_t SPIFlash_S25FL512S::eraseAndWritePage(uint32_t pageIdx, void* source,uint32_t maxLen) {
	int32_t result;
	result=erasePage(pageIdx);
	if(result < 0){
		return result;
	}
	Thread::suspendCallerUntil(NOW()+ MIN_TIME_TO_ERASE_PAGE * MILLISECONDS);
	return writePage(pageIdx, source, maxLen);
}

int32_t SPIFlash_S25FL512S::readID(uint8_t* rdBuf, uint16_t size) {
	uint8_t command = CMD_RDID;

	waitForFlash();
	cs->setPins(0);
	spi->write(&command, 1); // 1 byte opcode
	spi->read(rdBuf, size);
	cs->setPins(1);

	return size;
}

uint16_t SPIFlash_S25FL512S::getDeviceID() {
	uint8_t rdBuf[3];

	readID(rdBuf,sizeof(rdBuf));

	return (rdBuf[1] << 8) | rdBuf[2];
}

uint8_t SPIFlash_S25FL512S::getManufacturerID() {
	uint8_t manID = 0;

	readID(&manID,1);

	return manID;
}

uint8_t SPIFlash_S25FL512S::getExtendedDeviceInfoLen() {
	uint8_t rdBuf[4];

	readID(rdBuf,sizeof(rdBuf));

	return rdBuf[3];
}
