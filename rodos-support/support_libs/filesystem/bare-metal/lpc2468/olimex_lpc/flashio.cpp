
#include <stdio.h>
#include "lpc2468_registers.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "arm_comm.h"
#include "board.h"
#include "flashio.h"
#include "rodos.h"

#define EMC_PERIOD          13.8  // 72MHz

#define P2C(Period)           (((Period<EMC_PERIOD)?0:(Int32U)((Flo32)Period/EMC_PERIOD))+1)

#define NAND_BASE_ADDR  *(volatile Int8U*)0x81000000
#define NAND_CMD_LATCH  *(volatile Int8U*)0x81040000
#define NAND_ADDR_LATCH *(volatile Int8U*)0x81080000

#define NAND_tCSWE            0
#define NAND_tCSOW            0
#define NAND_tREA             60
#define NAND_tRC              120
#define NAND_tWP              60
#define NAND_tTURNE           150

#define NAND_W_TO_B_DLY       20       //
#define NAND_PASS (1<<0)
#define NAND_WP   (1<<7)

#define NAND_RB_PIN (1<<12) // P2.12

// these defines are empty since CS1 is hardware implemented by the parallel port module
#define NandChipEn()
#define NandChipDis()

void NandWaitReady(void) {
    volatile int i; //volatile!!!!
    for (i = 0; i < NAND_W_TO_B_DLY; i++);

    while (!(FIO2PIN & NAND_RB_PIN));
    //xprintf("Nand ready");
}

BYTE getStatus() {
    NAND_CMD_LATCH = 0x70;
    NandWaitReady();
    UINT status = NAND_BASE_ADDR;
    //xprintf("getStatus: %x\n", status);
    return (BYTE) status;
}

bool isWriteProtected() {
    BYTE stat = getStatus();
    if ((stat & (1 << 7)) == 0) {
        return true;
    }
    return false;
}

bool selectDrive(BYTE driveNo) {
    //only one drive - do nothing
}


void NandReset(void) {
    NandChipEn();
    NAND_CMD_LATCH = 0xFF;
    NandWaitReady();
    NandChipDis();
}

void getID(char * pID) {
    NandChipEn();
    //ID Command
    NAND_CMD_LATCH = 0x90;
    //
    NAND_ADDR_LATCH = 0x00;
    NandWaitReady();
    int i;
    for (i = 0; i < 4; i++) {
        *pID++ = NAND_BASE_ADDR;
    }
    NandChipDis();
}

void readBytes(BYTE * pDest, UINT thePageIndex, UINT theColumn, UINT theSize) {

    NandChipEn();
    // issue command for reading
    NAND_CMD_LATCH = 0x00;
    //column address
    NAND_ADDR_LATCH = (theColumn >> 0) & 0xFF;
    NAND_ADDR_LATCH = (theColumn >> 8) & 0x0F;
    //Page Address
    NAND_ADDR_LATCH = (thePageIndex >> 0) & 0xFF;
    NAND_ADDR_LATCH = (thePageIndex >> 8) & 0xFF;
    NAND_CMD_LATCH = 0x30;
    NandWaitReady();
    UINT cnt;
    for (cnt = 0; theSize > cnt; cnt++) {
        *pDest++ = NAND_BASE_ADDR;
    }
    NandChipDis();
}

void readPage(BYTE * pDest, UINT thePageIndex) {

    NandChipEn();
    //Read 1 First page half
    NAND_CMD_LATCH = 0x00;
    //column 0
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = (thePageIndex >> 0) & 0xFF;
    NAND_ADDR_LATCH = (thePageIndex >> 8) & 0xFF;
    NAND_CMD_LATCH = 0x30;

    NandWaitReady();

    UINT cnt;
    for (cnt = 0; PAGE_TOTAL_BYTE_SIZE > cnt; cnt++) {
        *pDest++ = NAND_BASE_ADDR;
    }
    NandChipDis();
}

USHORT readToEndOfBlock(BYTE * pDest, UINT thePageIndex, USHORT maxPages) {
    //assert(NAND_PG_NUM > PageIndex);
    int readPages = 0;
    NandChipEn();
    do {
        //Read1
        NAND_CMD_LATCH = 0x00;
        //column 0
        NAND_ADDR_LATCH = 0;
        NAND_ADDR_LATCH = 0;
        //Page Address
        NAND_ADDR_LATCH = (thePageIndex >> 0) & 0xFF;
        NAND_ADDR_LATCH = (thePageIndex >> 8) & 0xFF;
        NAND_CMD_LATCH = 0x30;
        NandWaitReady();

        UINT cnt;
        for (cnt = 0; PAGE_TOTAL_BYTE_SIZE > cnt; cnt++) {
            *pDest++ = NAND_BASE_ADDR;
        }

        readPages += 1;
        if (readPages >= maxPages) {
            break;
        }

    } while ((++thePageIndex) & (PAGES_PER_BLOCK - 1));

    NandChipDis();
}

BYTE programPage(BYTE * pSrc, UINT thePageIndex) {
    //assert(NAND_PG_NUM > Page);
    
    //long long t0 = NOW() / 1000000;

    NandChipEn();
    NAND_CMD_LATCH = 0x80;
    //column 0
    NAND_ADDR_LATCH = 0;
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = (thePageIndex >> 0) & 0xFF;
    NAND_ADDR_LATCH = (thePageIndex >> 8) & 0xFF;

    NandWaitReady();
    UINT cnt;
    for (cnt = 0; PAGE_TOTAL_BYTE_SIZE > cnt; cnt++) {
        NAND_BASE_ADDR = *pSrc++;
    }
    NAND_CMD_LATCH = 0x10;
    NandWaitReady();
    BYTE nandStatus = getStatus();
    NandChipDis();
    
    //long long t1 = NOW() / 1000000;
    //xprintf("flashio > programmed page in %lldms\n", t1 - t0);
    return nandStatus;
}


BYTE copyPage(UINT theSourcePageIndex, UINT theTargetPageIndex) {
     NandChipEn();
    //Read page to cache
    NAND_CMD_LATCH = 0x00;
    //column 0
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = (theSourcePageIndex >> 0) & 0xFF;
    NAND_ADDR_LATCH = (theSourcePageIndex >> 8) & 0xFF;
    NAND_CMD_LATCH = 0x35;
    NandWaitReady();

  
    //write from cache
    NAND_CMD_LATCH = 0x85;
    //column 0
    NAND_ADDR_LATCH = 0;
    NAND_ADDR_LATCH = 0;
    //Page Address
    NAND_ADDR_LATCH = (theTargetPageIndex >> 0) & 0xFF;
    NAND_ADDR_LATCH = (theTargetPageIndex >> 8) & 0xFF;
    NandWaitReady();
    NAND_CMD_LATCH = 0x10;
    NandWaitReady();
    BYTE nandStatus = getStatus();
    NandChipDis();

    return nandStatus;
}

BYTE eraseBlock(UINT aBlockIndex) {
    UINT Page = aBlockIndex * PAGES_PER_BLOCK;

    NandChipEn();
    NAND_CMD_LATCH = 0x60;
    NAND_ADDR_LATCH = (Page >> 0) & 0xFF;
    NAND_ADDR_LATCH = (Page >> 8) & 0xFF;
    NAND_CMD_LATCH = 0xD0;
    NandWaitReady();
    BYTE nandStatus = getStatus();
    NandChipDis();
    return nandStatus;
}


void syncBuffers() {
    //does nothing because we don't cache anything here
}

bool isBlockValid(UINT aBlockIndex) {
    UINT page;
    BYTE byte;

    //For the first and second page in the Block
    for (page = 0; 2 > page; page++) {
        //the 1th Byte in the Spare array shoud be 0xFF
        readBytes(&byte, aBlockIndex * PAGES_PER_BLOCK + page, DAMAGED_BYTE_OFFSET, 1);
        if (0xFF != byte) {
            return FALSE;
        }
    }
    return true;
}

bool isBlockErased(UINT aBlockIndex) {
    UINT page;
    BYTE byte;

    //For the first and second page in the Block
    for (page = 0; 2 > page; page++) {
        //the 1th Byte in the Spare array shoud be 0xFF
        readBytes(&byte, aBlockIndex * PAGES_PER_BLOCK + page, ERASED_BYTE_OFFSET, 1);
        if (0xFF != byte) {
            return false;
        }
    }
    return true;
}

void markBlockAsDamaged(USHORT aBlockIndex) {
    UINT blockStartPage = aBlockIndex * PAGES_PER_BLOCK;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    int i;
    for (i = 0; i < PAGE_TOTAL_BYTE_SIZE; i++) {
        pageBuf[i] = 0xff;
    }
    pageBuf[DAMAGED_BYTE_OFFSET] = 0x00;

    programPage(pageBuf, blockStartPage);
    programPage(pageBuf, blockStartPage + 1); //also second page in case first is damaged
}

void markBlockAsUsed(USHORT aBlockIndex) {
    UINT blockStartPage = aBlockIndex * PAGES_PER_BLOCK;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    int i;
    for (i = 0; i < PAGE_TOTAL_BYTE_SIZE; i++) {
        pageBuf[i] = 0xff;
    }
    pageBuf[ERASED_BYTE_OFFSET] &= 0xf0;

    programPage(pageBuf, blockStartPage);
    programPage(pageBuf, blockStartPage + 1); //also second page in case first is damaged
}

void initStorage(void) {
    //BYTES ARE NUMBERED LSB TO MSB!!

    // Assign pins to EMC controller(SDRAM & NAND Flash)

    // configure P2.12 - R/#B on NAND Flash
    PINSEL4 &= BIN32(11111100, 11111111, 11111111, 11111111);
    PINMODE4 &= BIN32(11111100, 11111111, 11111111, 11111111);

    // configure #RAS, #CAS, CLKOUT0, #DYCS0, CKEOUT0, DQMOUT0, DQMOUT1
    PINSEL5 &= BIN32(11110000, 11111100, 11111100, 11000000);
    PINSEL5 |= BIN32(00000101, 00000001, 00000001, 00010101);
    PINMODE5 &= BIN32(11110000, 11111100, 11111100, 11000000);
    PINMODE5 |= BIN32(00001010, 00000010, 00000010, 00101010);

    // configure D0 - D15
    PINSEL6 = BIN32(01010101, 01010101, 01010101, 01010101);
    PINMODE6 = BIN32(10101010, 10101010, 10101010, 10101010);

    // configure A0 - A14
    PINSEL8 &= BIN32(11000000, 00000000, 00000000, 00000000);
    PINSEL8 |= BIN32(00010101, 01010101, 01010101, 01010101);
    PINMODE8 &= BIN32(11000000, 00000000, 00000000, 00000000);
    PINMODE8 |= BIN32(00101010, 10101010, 10101010, 10101010);

    // configure A18, A19, #OE, #WE, #CS1
    PINSEL9 &= BIN32(00111111, 11110000, 11111111, 00001111);
    PINSEL9 |= BIN32(01000000, 00000101, 00000000, 01010000);
    PINMODE9 &= BIN32(00111111, 11110000, 11111111, 11111111);
    PINMODE9 |= BIN32(10000000, 00001010, 00000000, 10100000);

    NandChipDis();

    //give power
    PCONP |= (1 << 11);
    EMC_CTRL = 1; //emc enabled
    //
    volatile int i;
    for (i = 10000; i; i--);
    //8Bit Bus
    EMC_STA_CFG1 &= ~(11); //Memory width 8bit
    EMC_STA_CFG1 &= ~(1 << 3); //disabel async page mode
    EMC_STA_CFG1 &= ~(1 << 6); //Chip select active low


    EMC_STA_CFG1 &= ~(1 << 8); //No Extended wait

    EMC_STA_CFG1 |= (1 << 7); //Enable byte line state bit
    EMC_STA_CFG1 &= ~(1 << 19); //disble buffer
    EMC_STA_CFG1 &= ~(1 << 20); //disable write protect
    //Set delays
    EMC_STA_WAITWEN1 = NAND_tCSWE;
    EMC_STA_WAITOEN1 = NAND_tCSOW;
    EMC_STA_WAITRD1 = P2C(NAND_tREA);
    EMC_STA_WAITPAGE1 = 0;
    EMC_STA_WAITWR1 = P2C(NAND_tWP);
    //Reset
    NandReset();
}

