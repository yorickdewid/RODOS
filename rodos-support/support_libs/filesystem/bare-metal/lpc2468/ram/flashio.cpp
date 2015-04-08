
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "flashio.h"
#include "rodos.h"

typedef enum {
    b0000 = 0x00,
    b0001 = 0x01,
    b0010 = 0x02,
    b0011 = 0x03,
    b0100 = 0x04,
    b0101 = 0x05,
    b0110 = 0x06,
    b0111 = 0x07,
    b1000 = 0x08,
    b1001 = 0x09,
    b1010 = 0x0A,
    b1011 = 0x0B,
    b1100 = 0x0C,
    b1101 = 0x0D,
    b1110 = 0x0E,
    b1111 = 0x0F
} T_BIN_NIBBLE;

/* 2 nibbles to 8 bit */
#define B8(msn,lsn) ( ( b##msn<<4 ) |( b##lsn ) )

/* 4 nibbles to 16 bit */
#define B16(msn,n2, n1,lsn) ( (unsigned short)( b##msn<<12 ) |(unsigned short)( b##n2 <<8 ) |(unsigned short)( b##n1 <<4 ) |(unsigned short)( b##lsn ) )

/* Sample usage:
B8(0101,0101) = 85
B16(1010,1010 , 0101,0101) = 43605
 */


BYTE disk[TOTAL_NO_OF_PAGES][PAGE_TOTAL_BYTE_SIZE];

/**
 * Provides the ID of the storage.
 * @param pID byte array with length >= 4
 */
void getID(char * pID) {
    pID[0] = 'R';
    pID[1] = 'A';
    pID[2] = 'M';
    pID[3] = 'D';

}

/**
 * Returns the status of the last action (like read and write success)
 * 
 * bit          read    write     erase   
 * (LS to MS)
 * 0            pass: 0 / fail:1
 * 1            na
 * 2            na
 * 3            na
 * 4            na
 * 5            na
 * 6            ready: 1 / busy: 0
 * 7            not protected: 1 / protected: 0
 * 
 * @return Byte containing status
 */
BYTE getStatus() {

    return B8(0100, 0000);
}

bool isWriteProtected() {
    return false;
}

bool selectDrive(BYTE driveNo) {
    //only one drive - do nothing
}

void readPage(BYTE * pDest, UINT thePageIndex) {
    if (thePageIndex >= TOTAL_NO_OF_PAGES) {
        xprintf("ERROR: ramio > readPage > page out of bounds\n");
    }
    //printf("ramio > readPage > %i in block %i\n", page, page/GR_SECTORS_PER_BLOCK);

    int byte = 0;
    for (byte = 0; byte < (PAGE_TOTAL_BYTE_SIZE); byte++) {
        BYTE d = disk[thePageIndex][byte];
        pDest[byte] = d;
        //pDest[byte] = disk[page][byte];
    }
}

USHORT readToEndOfBlock(BYTE * pDest, UINT thePageIndex, USHORT maxPages) {
    //assert(NAND_PG_NUM > PageIndex);
    int startPage = thePageIndex;
    int readPages = 0;
    int page = thePageIndex % PAGES_PER_BLOCK;
    for (; page < PAGES_PER_BLOCK; page++) {

        int byte = 0;
        for (byte = 0; byte < (PAGE_TOTAL_BYTE_SIZE); byte++) {
            pDest[(page * (PAGE_TOTAL_BYTE_SIZE)) + byte] = disk[startPage + page][byte];
        }

        readPages += 1;
        if (readPages >= maxPages) {
            break;
        }
    }
    return readPages;
}

BYTE programPage(BYTE * pSrc, UINT thePageIndex) {

    int byte = 0;
    for (byte = 0; byte < PAGE_TOTAL_BYTE_SIZE; byte++) {
        disk[thePageIndex][byte] &= pSrc[byte];
        //only writes zeros, to simulate flash
    }
    return getStatus();
}

BYTE copyPage(UINT theSourcePageIndex, UINT theTargetPageIndex) {

    int byte = 0;
    for (byte = 0; byte < PAGE_TOTAL_BYTE_SIZE; byte++) {
        disk[theTargetPageIndex][byte] &= disk[theSourcePageIndex][byte];
        //only writes zeros, to simulate flash
    }
    return getStatus();
}

BYTE eraseBlock(UINT aBlockIndex) {
    // Int32U Page = Block * NAND_PG_PER_BLK;
    if (isBlockValid(aBlockIndex) == false) {
        //invalid blocks must never be erased!
        return B8(0100, 0000); //OKSTATUS
    }

    int startPage = aBlockIndex * PAGES_PER_BLOCK;

    int page;
    for (page = 0; page < PAGES_PER_BLOCK; page++) {
        int byte = 0;
        for (byte = 0; byte < (PAGE_TOTAL_BYTE_SIZE); byte++) {
            disk[startPage + page][byte] = 0xff;
        }
    }
    return getStatus();
}

void syncBuffers() {
    //does nothing because we don't cache anything here
}

bool isBlockValid(UINT aBlockIndex) {
    UINT page;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];

    //For the first and second page in the Block
    for (page = 0; page < 2; page++) {
        //the 1th Byte in the Spare array is 0xFF for valid blocks
        readPage(pageBuf, aBlockIndex * PAGES_PER_BLOCK + page);
        if (0xFF != pageBuf[DAMAGED_BYTE_OFFSET])
            return false;
    }
    return (true);
}

bool isBlockErased(UINT aBlockIndex) {
    UINT page;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];

    //For the first and second page in the Block
    for (page = 0; page < 2; page++) {
        //the 1th Byte in the Spare array is 0xFF for valid blocks
        readPage(pageBuf, aBlockIndex * PAGES_PER_BLOCK + page);
        if (0xFF != pageBuf[ERASED_BYTE_OFFSET])
            return false;
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
    //erase all -> everything to #ffffff
    xprintf("ramio > initStorage pagess %d @ %d byte \n", TOTAL_NO_OF_PAGES, PAGE_TOTAL_BYTE_SIZE);


    UINT page = 0;
    for (page = 0; page < TOTAL_NO_OF_PAGES; page++) {

        int byte = 0;
        for (byte = 0; byte < (PAGE_TOTAL_BYTE_SIZE); byte++) {
            disk[page][byte] = 0xff;

            //create initial bad blocks:
            if (byte == DAMAGED_BYTE_OFFSET) {

                if (page == (PAGES_PER_BLOCK * 19)) {
                    disk[page][byte] = 0xAC; // non ff value
                }

                if (page == (PAGES_PER_BLOCK * 86)) {
                    disk[page][byte] = 0xAF; // non ff value
                }

                if (page == (PAGES_PER_BLOCK * 203)) {
                    disk[page][byte] = 0x00; // non ff value
                }

                if (page == (PAGES_PER_BLOCK * 303)) {
                    disk[page][byte] = 0xBC; // non ff value
                }
            } //end if damaged
        } //end for byte
    } //end for pages
} //end function


/**
 * Prints all pages in the given range. Printable Bytes will be printed as chars,
 * all other bytes as hex values. Sequences of 00h will be skipped
 * @param startPage first page to print
 * @param noOfPages number of pages to print
 */
void printPages(UINT theStartPage, int noOfPagesToPrint) {
    int no = noOfPagesToPrint;
    if (no < 1 || no > TOTAL_NO_OF_PAGES) {
        no = TOTAL_NO_OF_PAGES;
    }

    xprintf("Disk:\n");

    int skipped = 0;
    int skippedPages = 0;
    int sec;

    for (sec = theStartPage; sec < (no + theStartPage); sec++) {
        int byte = 0;
        bool pageIsEmpty = true;
        for (byte = 0; byte < PAGE_TOTAL_BYTE_SIZE; byte++) {
            BYTE currentChar = disk[sec][byte];
            bool isCharPrintable = (currentChar >= 32 && currentChar <= 126);

            if (currentChar != 0xff && currentChar != 0) {
                if (skipped > 8) {
                    xprintf(" skipped to %d/%d: ", sec, byte);
                    skipped = 0;

                }
                pageIsEmpty = false;

                if (isCharPrintable) {
                    xprintf("%c ", currentChar);
                } else {
                    xprintf("%x ", currentChar);
                }
            } else {

                if (skipped < 8) {
                    if (isCharPrintable) {
                        xprintf("%c ", currentChar);
                    } else {
                        xprintf("%x ", currentChar);
                    }
                }
                skipped++;
            }

        }
        if (!pageIsEmpty) {
            if (skippedPages > 0) {
                xprintf("skipped to page %d", sec + 1);
                skippedPages = 0;
            }
            xprintf("\n");
        } else {
            skippedPages++;
        }
    }

    if (skippedPages > 0) {
        xprintf(" skipped %d pages to %d\n", skippedPages, sec);
    }

    xprintf("\n\n\n");

}

