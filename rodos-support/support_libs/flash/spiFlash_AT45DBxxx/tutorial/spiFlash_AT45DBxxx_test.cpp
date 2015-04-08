/*
 * flash_AT45DB321D_test.cpp
 *
 * Created on: 19.08.2013
 * Author: Michael Ruffer
 *
 */

#include "rodos.h"
#include "hal/hal_spi.h"
#include "support_libs.h"

/****** Use modulId 2000 just be be found by other example: ceiler **/

static Application module01("Flash-Test", 2000);


HAL_SPI spi1(SPI_IDX1);

//HAL_GPIO csPin(GPIO_071);  // PE7 active low chip select
//HAL_GPIO rstPin(GPIO_002); // PA2 active low reset
//HAL_GPIO wpPin(GPIO_004);  // PA4 active low write protect
HAL_GPIO csPin(GPIO_004);  // PA4 active low chip select
HAL_GPIO rstPin(GPIO_008); // PA8 active low reset
HAL_GPIO wpPin(GPIO_034);  // PC2 active low write protect

SPIFlash_AT45DBxxx spiFlash1(&spi1, &csPin);

uint8_t testBuffer[SPIFlash_AT45DBxxx::PAGE_TOTAL_SIZE + 10] = { 0 };
uint8_t tmpBuffer[SPIFlash_AT45DBxxx::PAGE_TOTAL_SIZE] = { 0 };

class FlashTestThread: public Thread {

public:
    FlashTestThread(const char* name) :
            Thread(name) {
    }

    void init() {
        csPin.init(true, 1, 1);
        wpPin.init(true, 1, 1);
        rstPin.init(true, 1, 1);

        for (uint32_t i = 0; i < sizeof(testBuffer); i++) {
            testBuffer[i] = i;
        }
    }

    void run() {
        uint32_t pageIdx = 1;

        xprintf("**************** init SPI interface ****************\n");
        #define SPI_BR 10000000
        int retVal =  spi1.init(SPI_BR);
        if (retVal >= 0){
            xprintf("SPI interface successfully initialized with baudrate: %d Hz\n",SPI_BR);
        }else{
            xprintf("SPI interface init failed with error: %d\n",retVal);
        }
        xprintf("\n\n");


        xprintf("**************** init SPI flash ****************\n");
        if (spiFlash1.init() != 0) {
            xprintf("flash init failed! break ... \n");
            suspendCallerUntil();
        }else{
            xprintf("SPI flash successfully initialized\n");
        }
        xprintf("\n\n");


        xprintf("**************** read chip IDs ****************\n");
        xprintf("manufacturer ID (expected: 0x1F): %x\n",
                spiFlash1.getManufacturerID());
        xprintf("device ID (expected: 0x2701): %x\n",
                spiFlash1.getDeviceID());
        xprintf("\n\n");


        xprintf("**************** page erase ****************\n");
        uint32_t errCnt = 0;
        if (spiFlash1.erasePage(pageIdx) == 0) {
            memset(tmpBuffer, 0, sizeof(tmpBuffer));
            spiFlash1.readPage(pageIdx, tmpBuffer, sizeof(tmpBuffer));
            for (uint32_t i; i < SPIFlash_AT45DBxxx::PAGE_TOTAL_SIZE; i++) {
                if (tmpBuffer[i] != 0xff)
                    errCnt++;
            }
            xprintf("page %d erased with %d errors\n", pageIdx, errCnt);
        } else {
            xprintf("error in erase command\n", pageIdx);
        }
        xprintf("\n\n");


        xprintf("**************** page write ****************\n");
        int32_t wrCnt = spiFlash1.writePage(pageIdx, testBuffer,
                sizeof(testBuffer));
        xprintf("%d bytes of testBuffer written to page %d:\n", wrCnt, pageIdx);
        xprintf("size of testBuffer: %d\n", sizeof(testBuffer));
        xprintf("\n\n");


        xprintf("**************** page read ****************\n");
        memset(tmpBuffer, 0, sizeof(tmpBuffer));
        uint32_t rdCnt = spiFlash1.readPage(pageIdx, tmpBuffer, sizeof(tmpBuffer));
        xprintf("%d bytes of page %d read.\n", rdCnt, pageIdx);
        for (uint32_t i; i < rdCnt; i++) {
            if (tmpBuffer[i] != i)
                errCnt++;
        }
        xprintf("page %d read with %d errors\n", pageIdx, errCnt);
        xprintf("\n\n");


        xprintf("********* page write/read offset *********\n");
        errCnt = 0;
        uint8_t wrVal = 0;
        uint8_t rdVal = 0;
        if ( (spiFlash1.erasePage(pageIdx) == Flash::Success) && (spiFlash1.blankCheck(pageIdx) == Flash::Success) ) {
            for(uint32_t i = 0;i<spiFlash1.getPageSize(pageIdx);i++){
                rdVal = 0;
                spiFlash1.writePageOffset(pageIdx, i, &wrVal, sizeof(wrVal));
                spiFlash1.readPageOffset(pageIdx, i, &rdVal, sizeof(rdVal));
                if(rdVal != wrVal){
                    errCnt++;
                    //PRINTF("compare error in offset test\n")
                }
                wrVal++;
                if( wrVal == 0xFF) wrVal++; // bad test values
                if (wrVal == 0) wrVal++; // bad test values
            }
            xprintf("page %d read with %d errors\n", pageIdx, errCnt);
            xprintf("\n\n");
        }else{
            xprintf("page %d erase failed\n", pageIdx);
        }


        xprintf("**************** page erase ****************\n");
        errCnt = 0;
        if (spiFlash1.erasePage(pageIdx) == 0) {
            memset(tmpBuffer, 0, sizeof(tmpBuffer));
            spiFlash1.readPage(pageIdx, tmpBuffer, sizeof(tmpBuffer));
            for (uint32_t i; i < SPIFlash_AT45DBxxx::PAGE_TOTAL_SIZE; i++) {
                if (tmpBuffer[i] != 0xff)
                    errCnt++;
            }
            xprintf("page %d erased with %d errors\n", pageIdx, errCnt);
        } else {
            xprintf("error in erase command\n");
        }
        xprintf("\n\n");

        suspendCallerUntil();
    }
};

FlashTestThread f1("FlashTest");
/***********************************************************************/
