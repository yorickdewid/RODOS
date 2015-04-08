//#include "spiFlash_S70FL01GS.h"
#include "flash.h"
#include "spiFlash_S70FL01GS.h"



SPIFlash_S70FL01GS::SPIFlash_S70FL01GS(HAL_SPI *spi, GPIO_PIN pincs1, GPIO_PIN pincs2) :
	cs1(pincs1),cs2(pincs2),
	flash1(spi,&cs1),flash2(spi,&cs2)
{
	this->spi = spi;
}

int32_t SPIFlash_S70FL01GS::getNumberOfPages() {
	return SPIFlash_S25FL512S::NUMBER_OF_PAGES*2;
}

int32_t SPIFlash_S70FL01GS::getPageSize(uint32_t pageIdx) {
	return SPIFlash_S25FL512S::PAGE_USER_SIZE;
}

int32_t SPIFlash_S70FL01GS::init() {
	//spi->init();
	cs1.init(true, 1, 1);
	cs2.init(true, 1, 1);

	int32_t result;
	result=flash1.init();
	if(result != 0){
		return result;
	}
	return flash2.init();
}

int32_t SPIFlash_S70FL01GS::readPage(uint32_t pageIdx, void *destination, uint32_t maxLen) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.readPage(pageIdx,destination,maxLen);
	}else{
		return flash2.readPage(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES,destination,maxLen);
	}
}

int32_t SPIFlash_S70FL01GS::writePage(uint32_t pageIdx, void *source, uint32_t len) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.writePage(pageIdx,source,len);
	}else{
		return flash2.writePage(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES,source,len);
	}
}

int32_t SPIFlash_S70FL01GS::erasePage(uint32_t pageIdx) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.erasePage(pageIdx);
	}else{
		return flash2.erasePage(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES);
	}
}

int32_t SPIFlash_S70FL01GS::readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.readPageOffset(pageIdx,offsetInPage,destination,maxLen);
	}else{
		return flash2.readPageOffset(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES,offsetInPage,destination,maxLen);
	}
}

int32_t SPIFlash_S70FL01GS::writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t len) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.writePageOffset(pageIdx,offsetInPage,source,len);
	}else{
		return flash2.writePageOffset(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES,offsetInPage,source,len);
	}
}

int32_t SPIFlash_S70FL01GS::eraseAndWritePage(uint32_t pageIdx, void* source,uint32_t maxLen) {
	if(pageIdx < SPIFlash_S25FL512S::NUMBER_OF_PAGES){
		return flash1.eraseAndWritePage(pageIdx,source,maxLen);
	}else{
		return flash2.eraseAndWritePage(pageIdx-SPIFlash_S25FL512S::NUMBER_OF_PAGES,source,maxLen);
	}
}


