/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.07   (C)ChaN, 2009
/-----------------------------------------------------------------------*/

#ifndef _DISKIO

#include "../typeDefs.h"




/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


//These are documented by ChaN
DSTATUS disk_initialize(BYTE);
DSTATUS disk_status(BYTE);
DRESULT disk_read(BYTE, BYTE*, DWORD, BYTE);
#if	_READONLY == 0
DRESULT disk_write(BYTE, const BYTE*, DWORD, BYTE);
#endif
DRESULT disk_ioctl(BYTE, BYTE, void*);


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	// Drive not initialized
#define STA_NODISK		0x02	// No medium in the drive 
#define STA_PROTECT		0x04	// Write protected


/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC		0	// Mandatory for write functions
#define GET_SECTOR_COUNT	1	//Mandatory for only f_mkfs()
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	//Mandatory for only f_mkfs()
#define CTRL_POWER		4
#define CTRL_LOCK		5
#define CTRL_EJECT		6




//Gabriel Reimers start

/**
 * Does all init stuff. This is called from disk_initialize() (see above).
 */
void initDisk();


/**
 * Finds all blocks on the disk that have a bad block  marker.
 * This is run on initialization to create an initial bad block table.
 * Flash chips often already have marked bad blocks when they come from the factory.
 */
void scanDiskForBadBlocks();

/**
 * Loads the bad block table from the disk if present. 
 * This must only be done if it is sure that there is a correct bad block table.
 * Otherwise this will read arbitrary data and lead to unexpected behavior.
 */
void loadBadBlockTableFromDisk();

/**
 * Creates an entry for a bad block in the bad block table and saves the table to disk.
 * @param blockIndex bad block index
 * @param replacementIndex index of replacement block
 */
void setReplacementForBlock(USHORT blockIndex, USHORT replacementIndex);

/**
 * Applies ECC to pageBuf. If error is detected and is correctable (1 error) this will be done.
 * If errors can not be corrected the data will not be touched.
 * @param pageBuf data to be error checked
 * @return number of errors found
 */
int eccCorrectedPage(BYTE* pageBuf);

/**
 * Checks if a page is part of a damaged block. This is achieved by looking it up 
 * in the bad block table. If it is bad and the bad block table specifies a 
 * replacement block, this methods will calculate the page index within
 * that replacment block and return the correct page index.
 * @param aPage raw page index
 * @return correct page index
 */
UINT badBlockCorrectedPage(UINT aPage);

/**
 * Finds the next free spare block in the spare area of the disk
 * @return next free spare block index
 */
USHORT nextFreeSpareBlock();

/**
 * Finds the next free public block on the public data area of the disk.
 * This will try to return a free block close to a given block index. 
 * (This is to achieve simple wear leveling)
 * @param aBlockIndex any block index near which a free block is needed
 * @return a free block index
 */
USHORT nextFreePublicBlock(USHORT aBlockIndex);

/**
 * Copies a block to another block. Depending on the setup this either
 * copies page by page using ram or if supported using the on chip page copy function.
 * @param sourceBlock the block to be copied
 * @param destBlock the destination block
 * @param startPage the first page of the block to be copied (starting from 0)
 * @param numberOfPages the number of pages to be copied (max: PAGES_PER_BLOCK)
 */
void copyBlock(USHORT sourceBlock, USHORT destBlock, USHORT startPage, USHORT numberOfPages);

/**
 * Writes the content of a page buffer to standart output.
 * This is for debugging.
 * @param pageBuf
 */
void printPageBuf(BYTE* pageBuf);

//Gabriel Reimers end

#define _DISKIO
#endif




