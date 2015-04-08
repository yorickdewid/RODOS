/* 
 * File:   diskio.cpp
 * Author: Gabriel Reimers
 *
 * Created on March 26, 2012, 1:59 PM
 */

#include "diskio.h"
#include "../ff.h"


#include "HammingCode.h"
#include "rodos.h"


#include "pathSetup.h"
#include FLASHIO_SETUP_FILE
#include FLASHIO_FILE

#ifdef NOT_ON_RODOS
#include <string.h> //DELETE WHEN USING RODOS BECAUSE RODOS ALREADY CONTAINS STRING.H
#endif

//#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

#include <time.h> //this is only for debugging (random() needs this)



//bad block management
//pairs of 16 bit addresses (65536) for bad blocks and their replacement
//(<bad block>, <replacement>)
//saved in first block before spare space (32 pages for 65k*32)
//for pageSize = 2048:
//@(16,16) bit -> 512 pairs per page -> 16384 bb-pairs in 32 pages (= 25% of 65536)
//can be held in ram (1 page for up to 512 bb limit)
//finding spare block by finding gaps in the bb array
//LAST PAGE OF FIRST BLOCK BEARS INIT MARKER: "GR_BB_V_xxx"

//-> 16GB manageable
// if more increase bb pair size and use second / third block the same way

//WRITE
//lookup first block if target is damaged
//if not: write
//if damaged: look up replacement block (repeat from step one)
//if writing fails: find replacement block
//write replacement block adress to first block list and repeat writing

//READ
//lookup first block target
//just read, rely on ecc for error correction
//if >1 error found replace block



#define BB_INIT_CODE "GR_BB_V_101"

#define BB_MAX_BAD_BLOCKS NO_OF_SPARE_BLOCKS
#define BB_ADDRESS_SIZE 16

#define DEBUG_CREATE_BIT_ERRORS false

#define ENABLE_BAD_BLOCK_MANAGEMENT true


//This is limited to 2^16 blocks but can be adjusted...

USHORT badBlocks[BB_MAX_BAD_BLOCKS];
USHORT replacementBlocks[BB_MAX_BAD_BLOCKS];
USHORT badBlocksArrayCount = 0;

BOOL isInitialized = false;




/////////////////////////////////////////////////////////////
//Public ChaN FAT methods
///////////////////////////////////////////////////////////

DSTATUS disk_initialize(BYTE theDriveNo) {
                srand(time(NULL)); //this is only for testing

    selectDrive(theDriveNo);

    if (!isInitialized) {
        initDisk(); //this does all the work. see below
        isInitialized = true;
    }

    return disk_status(theDriveNo);
}

DSTATUS disk_status(BYTE theDriveNo) {
    DSTATUS r = !STA_NODISK & !STA_NOINIT & !STA_PROTECT;

    bool driveFound = selectDrive(theDriveNo);

    if (driveFound == false) {
        r |= STA_NODISK;
    }
    if (isInitialized == false) {
        r |= STA_NOINIT;
    }
    if (isWriteProtected()) {
        r |= STA_PROTECT;
    }
    return r;
}

DRESULT disk_read(BYTE theDriveNo, BYTE* theReadDataBuffer,
        DWORD theStartPageIndex, BYTE theNumberOfPagesToRead) {

    //this can be speed up by reading to end of block instead of single pages but
    //would require more ram (one block size)

    selectDrive(theDriveNo);

    if (theStartPageIndex > NO_OF_PUBLIC_PAGES) {
        xprintf("WARNING: diskio > disk_read > reading from spare pages\n"
                "FAT has wrong pages number. Check disk ctrl and defines.\n");
    }
    //xprintf("disk_read: reading %d pages", theNumberOfPagesToRead);
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];

    int page;
    for (page = 0; page < theNumberOfPagesToRead; page++) {
        //find correct block and page adress from bad block table
        UINT currentPageIndex =
                badBlockCorrectedPage(theStartPageIndex + page);
        readPage(pageBuf, currentPageIndex);

        int errorCount = eccCorrectedPage(pageBuf);
        if (errorCount > 1) {
           xprintf("diskio > disk_read error while reading. uncorrectable errors found\n");

            USHORT freeSpareBlock = nextFreeSpareBlock();
            USHORT damagedBlockIndex = currentPageIndex / PAGES_PER_BLOCK;
            setReplacementForBlock(damagedBlockIndex, freeSpareBlock);
            copyBlock(damagedBlockIndex, freeSpareBlock, 0, PAGES_PER_BLOCK);
            markBlockAsDamaged(damagedBlockIndex);
            //next loop iteration will already read from copied spare block
        }
        memcpy(&(theReadDataBuffer[page * PAGE_BYTE_SIZE]), pageBuf, PAGE_BYTE_SIZE);
    }

    return RES_OK;
}

DRESULT disk_write(BYTE theDriveNo, const BYTE* theReadDataBuffer,
        DWORD theStartPageIndex, BYTE theNumberOfPagesToWrite) {
    //xprintf("disk_write > %d pages to page %d",theNumberOfPagesToWrite, theStartPageIndex);

    selectDrive(theDriveNo);

    if (theStartPageIndex > NO_OF_PUBLIC_PAGES) {
        xprintf("WARNING: diskio > disk_write > writing to spare pages\n"
                "FAT has wrong pages number. Check disk ctrl and defines.\n");
    }

    /////////////////////////
    //calculate bad block corrected page index, container block, next block...
    /////////////////////////
    UINT startPage = badBlockCorrectedPage(theStartPageIndex);
    UINT blockIndex = startPage / PAGES_PER_BLOCK;
    UINT blockStartPage = blockIndex * PAGES_PER_BLOCK;
    UINT nextBlockStartPage = (blockIndex + 1) * PAGES_PER_BLOCK;
    UINT writeStopPage = startPage + theNumberOfPagesToWrite; //first page not to be written
    if (nextBlockStartPage < writeStopPage) {
        writeStopPage = nextBlockStartPage;
    }



    ///////////////////////
    //check if page is empty or must be erased
    ////////////////////////
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];

    bool needsToEraseBlock = false;
    if (isBlockErased(blockIndex) == false) {
        //printf("diskio > disk_write manual empty check\n");
        //manually check if block needs to be erased
        UINT page;
        for (page = startPage; page < writeStopPage; page++) {

            readPage(pageBuf, page);
            BYTE usedByte = pageBuf[ERASED_BYTE_OFFSET];
            if (usedByte != 0xff && usedByte != 0xf0) {
                //already in use -> erase nescessary
                needsToEraseBlock = true;
                break;
            }
        }//end for
    } else {
        //is empty -> mark as used now
        markBlockAsUsed(blockIndex);
    }


    ////////////////////
    //if needs to be earsed:
    //copy block to temp block, erase block, copy back;
    ///////////////////
    if (needsToEraseBlock) {
        //xprintf("diskio > disk_write needs to erase block\n");

        USHORT tempBlock = nextFreePublicBlock(blockIndex); //here is the error
        UINT overwriteStartOffset = startPage - blockStartPage;
        UINT overwriteEndOffset = overwriteStartOffset + theNumberOfPagesToWrite - 1;

        if (overwriteStartOffset > 0) {
            copyBlock(blockIndex, tempBlock, 0, overwriteStartOffset);
        }
        copyBlock(blockIndex, tempBlock, (overwriteEndOffset + 1),
                (PAGES_PER_BLOCK - (overwriteEndOffset + 1)));
        BYTE status = eraseBlock(blockIndex);

        if ((status & 0x01) != 0) {
            //error while erasing
            //block must be replaced and disk write repeated
            xprintf("diskio > disk_write error while erasing. try again\n");

            USHORT freeSpareBlock = nextFreeSpareBlock();
            setReplacementForBlock(blockIndex, freeSpareBlock);
            copyBlock(blockIndex, freeSpareBlock, 0, PAGES_PER_BLOCK);
            markBlockAsDamaged(blockIndex);
            //repeat with replacement block
            return disk_write(theDriveNo, theReadDataBuffer,
                    theStartPageIndex, theNumberOfPagesToWrite);
        }

        copyBlock(tempBlock, blockIndex, 0, PAGES_PER_BLOCK);
        eraseBlock(tempBlock); //we don't check if erase successful for the tempblock

        markBlockAsUsed(blockIndex);
    }

    ////////////////////////
    //now write actual data
    ///////////////////////
    DRESULT result = RES_OK;
    UINT page;
    for (page = startPage; page < writeStopPage; page++) {
        memcpy(pageBuf, &theReadDataBuffer[(page - startPage) * PAGE_BYTE_SIZE], PAGE_BYTE_SIZE);
        int b;
        for (b = 0; b < SPARE_BYTES_PER_PAGE; b++) {
            pageBuf[PAGE_BYTE_SIZE + b] = 0xff; //reset buffer
        }
        if (ENABLE_ECC) {
            BYTE eccBuf[ECC_BYTE_LENGTH];
            ECCcodeForData(pageBuf, eccBuf);
            memcpy(&(pageBuf[ECC_CODE_OFFSET]), eccBuf, ECC_BYTE_LENGTH);
            pageBuf[ECC_ENABLED_OFFSET] = 0x00;
        }
        pageBuf[ERASED_BYTE_OFFSET] = 0x00;


        ////////
        //test ecc with random bit error
        ///////
        if (DEBUG_CREATE_BIT_ERRORS) {
            int errPos = rand() % 10;
            //xprintf("generate random error at %d\n", errPos);
            pageBuf[errPos] &= 0xfe;
        }
        ///////////


        BYTE status = programPage(pageBuf, page);

        if ((status & 0x01) != 0) {
            //error while writing
            //block must be replaced and disk write repeated
            xprintf("diskio > disk_write error while writing. try again (status error)\n");

            USHORT freeSpareBlock = nextFreeSpareBlock();
            setReplacementForBlock(blockIndex, freeSpareBlock);
            copyBlock(blockIndex, freeSpareBlock, 0, PAGES_PER_BLOCK);
            markBlockAsDamaged(blockIndex);
            //repeat with replacement block
            return disk_write(theDriveNo, theReadDataBuffer,
                    theStartPageIndex, theNumberOfPagesToWrite);
        } else if ((READ_AFTER_WRITE_CHECK > 0) && ENABLE_ECC) {
            //no status error detected but we now also read back the page
            //and apply ECC to safely detect write failures
            BYTE readBackPageBuf[PAGE_TOTAL_BYTE_SIZE];
                //xprintf("diskio > disk_write RaW check\n");

            readPage(readBackPageBuf, page);
            int errorCount = eccCorrectedPage(readBackPageBuf);
            if (errorCount >= READ_AFTER_WRITE_CHECK) {
                xprintf("diskio > disk_write error while writing. try again (RaW check)\n");

                USHORT freeSpareBlock = nextFreeSpareBlock();
                setReplacementForBlock(blockIndex, freeSpareBlock);
                copyBlock(blockIndex, freeSpareBlock, 0, PAGES_PER_BLOCK);
                markBlockAsDamaged(blockIndex);
                //repeat with replacement block
                return disk_write(theDriveNo, theReadDataBuffer,
                        theStartPageIndex, theNumberOfPagesToWrite);
            }//end write error found
        }//end RaW check
    }//end for


    UINT writtenPagesCount = startPage - writeStopPage;
    if (writtenPagesCount < theNumberOfPagesToWrite) {
        //not all pages did fit into block so reapeat for remaining pages
        const BYTE* readBufPtr = &(theReadDataBuffer[(writtenPagesCount) * PAGE_BYTE_SIZE]);
        DWORD startS = (DWORD) (writeStopPage);
        BYTE noToWrite = (BYTE) (theNumberOfPagesToWrite - writtenPagesCount);
        UINT r = ((UINT) result) | (UINT) disk_write(theDriveNo, readBufPtr, startS, noToWrite);
        result = (DRESULT) r;
    }

    return result;
}

DRESULT disk_ioctl(BYTE theDriveNo, BYTE commandCode, void* parameterBuffer) {
    //Note that ChaN often refers to pages as 'sectors'. page == sector.
    /* 
     CTRL_SYNC	Make sure that the disk drive has finished pending write process. 
     * When the disk I/O module has a write back cache, flush the dirty pages 
     * immediately. This command is not used in read-only configuration.

     * GET_SECTOR_SIZE	Returns page size of the drive into the WORD variable pointed 
     * by Buffer. This command is not used in fixed page size configuration, _
     * MAX_SS is 512.

     * GET_SECTOR_COUNT	Returns number of available pages on the drive into the 
     * DWORD variable pointed by Buffer. This command is used by only f_mkfs 
     * function to determine the volume size to be created.

     * GET_BLOCK_SIZE	Returns erase block size of the flash memory in unit of pages 
     * into the DWORD variable pointed by Buffer. The allowable value is 1 to 32768 
     * in power of 2. Return 1 if the erase block size is unknown or disk devices. 
     * This command is used by only f_mkfs function and it attempts to align data 
     * area to the erase block boundary.

     * CTRL_ERASE_SECTOR	Erases a part of the flash memory specified by a DWORD 
     * array {<start page>, <end page>} pointed by Buffer. When this feature 
     * is not supported or not a flash memory media, this command has no effect. 
     * The FatFs does not check the result code and the file function is not affected 
     * even if the pages are not erased well. This command is called on removing 
     * a cluster chain when _USE_ERASE is 1.
    
     */


    /*
    RES_OK (0) The function succeeded.
    RES_ERROR Any error occured.
    RES_PARER Invalid command code.
    RES_NOTRDY The disk drive has not been initialized.
     */


    DRESULT result;

    result = RES_ERROR;

    selectDrive(theDriveNo);


    switch (commandCode) {
        case CTRL_SYNC:
            // Make sure that no pending write process
            //if there are any write buffers in diskio they must be written to disk now
            syncBuffers(); //call flashio sync
            result = RES_OK;
            break;
        case GET_SECTOR_COUNT: // Get number of pages on the disk (DWORD) 
            *(DWORD*) parameterBuffer = (DWORD) NO_OF_PUBLIC_PAGES;

            result = RES_OK;
            break;

        case GET_SECTOR_SIZE: // Get R/W page size (WORD)
            *(WORD*) parameterBuffer = PAGE_BYTE_SIZE;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE: // Get erase block size in unit of page (DWORD) 
            *(DWORD*) parameterBuffer = (WORD) PAGES_PER_BLOCK;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
    }
    return result;
}




//////////////////////////
//Internal Methods
//Bad block management
//ECC
//////////////////////////

void initDisk() {
    initStorage();

    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    UINT initMarkerPage = (BAD_BLOCK_TABLE_BLOCK * PAGES_PER_BLOCK)
            + PAGES_PER_BLOCK - 1; //last page of bb table block
    readPage(pageBuf, initMarkerPage);
    eccCorrectedPage(pageBuf);
    //xprintf("diskio > init mark '%s'\n", (char*) pageBuf);
    if (FORCE_ERASE_BB_TABLE || (ENABLE_BAD_BLOCK_MANAGEMENT && (strncmp((char*) pageBuf, BB_INIT_CODE, 11) != 0))) {
        xprintf("diskio > init > not init mark found-> erase disk\n");
        //erase disk
        USHORT blockIndex = 0;
        while (blockIndex < BLOCK_TOTAL_COUNT) {
            eraseBlock(blockIndex);
            blockIndex++;
        }

        //find initial bad blocks
        scanDiskForBadBlocks();

        BYTE* initCode = (BYTE*) BB_INIT_CODE;
        memcpy(pageBuf, initCode, 11);

        ECCcodeForData(pageBuf, &(pageBuf[ECC_CODE_OFFSET]));
        pageBuf[ECC_ENABLED_OFFSET] = 0x00;
        pageBuf[ERASED_BYTE_OFFSET] = 0x00;
        programPage(pageBuf, initMarkerPage);

        markBlockAsUsed(BAD_BLOCK_TABLE_BLOCK);
    } else { //bb table available
        xprintf("diskio > init > init mark found-> loading bb table\n");
        loadBadBlockTableFromDisk();
    }

    readPage(pageBuf, initMarkerPage);
    eccCorrectedPage(pageBuf);
    //xprintf("diskio > init dome mark '%s'\n", (char*) pageBuf);
}

void scanDiskForBadBlocks() {
    xprintf("diskio > scan disk for bad blocks\n");

    USHORT blockIndex = 0;
    while (blockIndex < BLOCK_TOTAL_COUNT) {
        bool isOK = isBlockValid(blockIndex);
        if (!isOK) {
            xprintf("diskio > scan found bb %d\n", blockIndex);

            if (blockIndex == 0) {
                xprintf("diskio > scan bb > ATTENTION: BLOCK 0 IS DAMAGED\n "
                        "THIS MEANS THE FAT TABLE WILL NOT WORK CORRECTLY AND YOU BETTER REPLACE THE DISK\n");
            }
            if (blockIndex == BAD_BLOCK_TABLE_BLOCK) {
                xprintf("diskio > scan bb > ATTENTION: BLOCK OF BAD BLOCK TABLE IS DAMAGED\n "
                        "THIS MEANS BB MANAGEMENT WILL NOT WORK CORRECTLY AND YOU BETTER REPLACE THE DISK\n");
            }

            USHORT freeSpareBlock = nextFreeSpareBlock();
            setReplacementForBlock(blockIndex, freeSpareBlock);
        }
        blockIndex++;
    }
    xprintf("diskio > scan disk for bad blocks done %d found\n\n\n", badBlocksArrayCount);

}

void loadBadBlockTableFromDisk() {
    xprintf("diskio > load bad block table from disk\n");

    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    UINT pageIndex = (BAD_BLOCK_TABLE_BLOCK * PAGES_PER_BLOCK);
    USHORT bbIndex = 0;
    USHORT replaceIndex = 0;

    readPage(pageBuf, pageIndex);
    eccCorrectedPage(pageBuf);

    //iterate over first block and copy entries to ram
    UINT byteIndex = 0;
    while (byteIndex < (BB_MAX_BAD_BLOCKS * 2 * BB_ADDRESS_SIZE)) {
        if (byteIndex >= ((pageIndex + 1) * PAGE_BYTE_SIZE)) {
            pageIndex++;
            readPage(pageBuf, pageIndex);
            eccCorrectedPage(pageBuf);
            //we can only correct one error
            //we cannot replace this block so we hope it never gets really damaged...
        }

        UINT inPageOffset = byteIndex % PAGE_BYTE_SIZE;
        BYTE b = pageBuf[inPageOffset];
        if (b == 0xff) {
            //reached end of used entries
            break;
        }
        USHORT* dest;
        if (bbIndex == replaceIndex) {
            dest = &(badBlocks[bbIndex]);
            bbIndex++;
        } else {
            dest = &(replacementBlocks[replaceIndex]);
            replaceIndex++;
        }
        memcpy(dest, &(pageBuf[inPageOffset]), 2);

        byteIndex += 2;
    }
    badBlocksArrayCount = bbIndex;
}

void setReplacementForBlock(USHORT blockIndex, USHORT replacementIndex) {
    if (badBlocksArrayCount >= BB_MAX_BAD_BLOCKS) {
        xprintf("GAME OVER: BadBlock max managable count reached, spare blocks all used.\n"
                "YOU NEED TO REPLACE THE DISK\n");
        return;
    }
    badBlocks[badBlocksArrayCount] = (USHORT) blockIndex;
    replacementBlocks[badBlocksArrayCount] = (USHORT) replacementIndex;
    badBlocksArrayCount++;

    //now write this to disk, too
    int lastEntryIndex = badBlocksArrayCount - 1;
    int bbBytePerPair = (BB_ADDRESS_SIZE * 2);
    UINT firstBBTablePage = BAD_BLOCK_TABLE_BLOCK * PAGES_PER_BLOCK;
    UINT bbTablePageIndex = firstBBTablePage + ((lastEntryIndex * bbBytePerPair) / PAGE_BYTE_SIZE);
    UINT inPageOffset = (lastEntryIndex * bbBytePerPair) % PAGE_BYTE_SIZE;

    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    readPage(pageBuf, bbTablePageIndex);
    eccCorrectedPage(pageBuf);

    BYTE* dest = &(pageBuf[inPageOffset]);
    USHORT* bbsrc = &(badBlocks[lastEntryIndex]);
    memcpy(dest, bbsrc, 1);
    USHORT* rpsrc = &(replacementBlocks[lastEntryIndex]);
    dest = &(pageBuf[inPageOffset + BB_ADDRESS_SIZE]);
    memcpy(dest, rpsrc, 1);
    if (ENABLE_ECC) {
        ECCcodeForData(pageBuf, &(pageBuf[ECC_CODE_OFFSET]));
        pageBuf[ECC_ENABLED_OFFSET] = 0x00;
    }
    pageBuf[ERASED_BYTE_OFFSET] = 0x00;
    programPage(pageBuf, bbTablePageIndex);
    if (badBlocksArrayCount % 4 == 0) {
        //this is necessary because a single page can only be written 3 times 
        //afterwards the block must be erased and rewritten. 
        USHORT tempBlock = nextFreeSpareBlock();
        copyBlock(BAD_BLOCK_TABLE_BLOCK, tempBlock, 0, PAGES_PER_BLOCK);
        eraseBlock(BAD_BLOCK_TABLE_BLOCK);
        copyBlock(tempBlock, BAD_BLOCK_TABLE_BLOCK, 0, PAGES_PER_BLOCK);
        eraseBlock(tempBlock);
    }

}

int eccCorrectedPage(BYTE* pageBuf) {
    if (!ENABLE_ECC) {
        return 0;
    }

    BYTE usedByte = pageBuf[ERASED_BYTE_OFFSET];
    BYTE eccEnabledByte = pageBuf[ECC_ENABLED_OFFSET]; //==0xff if ecc was never calculated for this page
    if (usedByte == 0xff || usedByte == 0xf0 || eccEnabledByte == 0xff) {
        //xprintf("diskIO > ECCCorrectedPage > ecc disabled or not available\n");
        //unused page or ecc disabled -> ecc is not initialized
        return 0;
    }

    BYTE* eccCode = &(pageBuf[ECC_CODE_OFFSET]);
    int errNumber = correctData(pageBuf, eccCode);
    return errNumber;
}

void printPageBuf(BYTE* pageBuf) {
    bool hasSkipped = false;
    bool hasPrinted = false;
    int byte;
    for (byte = 0; byte < PAGE_TOTAL_BYTE_SIZE; byte++) {
        if (pageBuf[byte] != 0 /*&& pageBuf[byte] != 0xff*/) {

            if (hasSkipped) {
                xprintf("[%d] ", byte);
                hasSkipped = false;
            }
            xprintf("%x ", pageBuf[byte]);
            hasPrinted = true;
        } else {
            hasSkipped = true;
        }
    }
    if (hasPrinted) {
        xprintf("\n");
    }
}

UINT badBlockCorrectedPage(UINT aPage) {
    if (!ENABLE_BAD_BLOCK_MANAGEMENT) {
        return aPage;
    }
    USHORT blockIndex = aPage / PAGES_PER_BLOCK;
    int pageOffset = aPage % PAGES_PER_BLOCK;

    int i;
    for (i = 0; i < badBlocksArrayCount; i++) {
        if (blockIndex == badBlocks[i]) {
            xprintf("diskio > badBlockCorrectedPage > for block %d is %d\n", blockIndex, replacementBlocks[i]);

            blockIndex = replacementBlocks[i];
            UINT pageIndex = blockIndex * PAGES_PER_BLOCK + pageOffset;
            if (pageIndex == aPage) {
                xprintf("diskio > badBlockCorrectedPage > ERROR\n"
                        "THE BAD BLOCK TABLE IS DAMAGED\n"
                        "YOU NEED TO ERASE THE DISK\n");
                return pageIndex;
            }
            return (badBlockCorrectedPage(pageIndex));
        }
    }
    return aPage;
}

USHORT nextFreeSpareBlock() {
    if (badBlocksArrayCount >= BB_MAX_BAD_BLOCKS) {
        xprintf("CRITICAL: no spare blocks left, cannot replace bad blocks anymore\n");
        return BB_MAX_BAD_BLOCKS;
    }
    USHORT freeBlockIndex = badBlocksArrayCount + FIRST_SPARE_BLOCK;
    while (!isBlockValid(freeBlockIndex)
            && (freeBlockIndex < (BB_MAX_BAD_BLOCKS + FIRST_SPARE_BLOCK))) {
        freeBlockIndex++;
    }
    return freeBlockIndex;
}

USHORT nextFreePublicBlock(USHORT aBlockIndex) {
    UINT visitedBlocksCount = 1;
    UINT blockIndex = aBlockIndex;
    UINT nextBlockStartPage = badBlockCorrectedPage((aBlockIndex + visitedBlocksCount) * PAGES_PER_BLOCK);

    UINT lastPossibleStartPage = PAGES_PER_BLOCK * NO_OF_PUBLIC_BLOCKS;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];
    while (visitedBlocksCount < NO_OF_PUBLIC_BLOCKS) { //if we visit all and none is free the game is over
        if (nextBlockStartPage >= lastPossibleStartPage) {
            blockIndex = 1; //first public block
        }
        readPage(pageBuf, nextBlockStartPage); //no ecc needed because we only care for used byte
        BYTE usedByte = pageBuf[ERASED_BYTE_OFFSET];
        if (usedByte == 0xff) {
            //free found
            break;
        } else {
            //already in use -> try next
            visitedBlocksCount++;
            blockIndex++;
            nextBlockStartPage = badBlockCorrectedPage(blockIndex * PAGES_PER_BLOCK);
        }
    }
    return nextBlockStartPage / PAGES_PER_BLOCK;

}

void copyBlock(USHORT sourceBlock, USHORT destBlock, USHORT startPage, USHORT numberOfPages) {
    UINT sourceStartPage = sourceBlock * PAGES_PER_BLOCK + startPage;
    UINT destStartPage = destBlock * PAGES_PER_BLOCK + startPage;
    BYTE pageBuf[PAGE_TOTAL_BYTE_SIZE];

    int s;
    for (s = 0; s < numberOfPages; s++) {
        if (USE_ON_CHIP_PAGE_COPY) {
            //xprintf("copy page:\n");
            copyPage(sourceStartPage + s, destStartPage + s);
        } else {
            readPage(pageBuf, s + sourceStartPage);
            if (pageBuf[ERASED_BYTE_OFFSET] == 0xff) {//if not used continue
                continue;
            }
            eccCorrectedPage(pageBuf); //we don't care how many errors we get
            programPage(pageBuf, s + destStartPage);
        }
    }
    markBlockAsUsed(destBlock);
}



