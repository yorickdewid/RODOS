/* 
 * File:   flashio.h
 * Author: Gabriel Reimers 2012
 *
 * Created on January 5, 2012, 3:51 PM
 */


#ifndef RAMIO_H
#define	RAMIO_H



#include "../../typeDefs.h"
#include "setup.h"

    //spare bytes usage max 32bytes
    //byte 0: reserved for marking bad block // bb is non #ff on samsung chip
    //byte 1: reserved for marking erased/non erased page 
    //#ff is erased; #00 is used;  #f0 page erased but block in use (on first two sectors of block)
    //byte 2: reserved for marking no of times written
    //byte 3-16: free
    //byte 16-31 reserved for (hamming) ecc-code            //#ffh if not calculated
    //remaining bytes free (if spare bigger than 32 byte)

//see setup.h for defines of page size, block size

#define DAMAGED_BYTE_OFFSET (PAGE_BYTE_SIZE)
#define ERASED_BYTE_OFFSET (PAGE_BYTE_SIZE + 1)
#define WRITTEN_NO_BYTE_OFFSET (PAGE_BYTE_SIZE + 2)
#define ECC_ENABLED_OFFSET (PAGE_BYTE_SIZE + 15)
#define ECC_CODE_OFFSET (PAGE_BYTE_SIZE + 16)

#define PAGE_TOTAL_BYTE_SIZE (PAGE_BYTE_SIZE + SPARE_BYTES_PER_PAGE)

#define FIRST_SPARE_BLOCK (NO_OF_PUBLIC_BLOCKS + 1)
#define BAD_BLOCK_TABLE_BLOCK (NO_OF_PUBLIC_BLOCKS)

    //spare blocks include bad block table in first blocks
    //make spare blocks on start of flash
    
#define BLOCK_TOTAL_COUNT (NO_OF_PUBLIC_BLOCKS + NO_OF_SPARE_BLOCKS)
#define NO_OF_PUBLIC_PAGES (NO_OF_PUBLIC_BLOCKS * PAGES_PER_BLOCK)
#define TOTAL_NO_OF_PAGES (BLOCK_TOTAL_COUNT * PAGES_PER_BLOCK)



    /**
     * Provides the ID of the storage.
     * @param pID byte array with length >= 4
     */
    void getID(char* pID);


    /**
     * Initializes Memory Unit
     */
    void initStorage(void);

    
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
    BYTE getStatus();

    
    /**
     * Returns true if the disk is hardware write protected.
     * @return true if write protected
     */
    bool isWriteProtected();

    
    /**
     * Selects the drive to access (if multiple physical drives are present).
     * This affects all subsequent operations.
     * Has no effect if the drive could not be found.
     * @param driveNo index of the drive to access
     * @return true if drive is found, false if no such drive
     */
    bool selectDrive(BYTE driveNo);
    
    /**
     * Reads a single page from the storage.
     * @param pDest byte buffer for read data (must have page size)
     * @param thePage index of page to read
     */
    void readPage(BYTE* pDest, UINT thePageIndex);

    
    /**
     * Reads a all pages to end of block from the storage.
     * @param pDest byte buffer for read data (must be pageSize * maxPages)
     * @param thePage index of page to read
     * @param maxPages max numbuer of pages to read
     * @return number of read pages
     */
    USHORT readToEndOfBlock(BYTE* pDest, UINT thePageIndex, USHORT maxPages);


    /**
     * Writes data to page. (Keep in mind: NAND will only write 0s. 
     * Overwriting 0 with 1 will need erase of block)
     * @param pSrc buffer with data to write (must have page size)
     * @param thePageIndex index of target page
     * @return status (see getStatus())
     */
    BYTE programPage(BYTE* pSrc, UINT thePageIndex);

    
    /**
     * Copies the contents of a page to another page using the internal cache
     * of the flash chip. This is much faster if the chip supports special
     * internal copying. If not this is implemented with readPage and writePage
     * using a software buffer.
     * @param theSourcePageIndex index of the source page
     * @param theTargetPageIndex index of the target page
     * @return  status (see getStatus())
     */
    BYTE copyPage(UINT theSourcePageIndex, UINT theTargetPageIndex);

    
    /**
     * Erases Block (BadBlocks will never be erases).
     * @param aBlockIndex index of block
     * @return status (see getStatus())
     */
    BYTE eraseBlock(UINT aBlockIndex);

    
    /**
     * Writes any buffered data to disk and clears caches.
     */
    void syncBuffers();
    
    /**
     * Checks a byte in the spare area, which indicates if the byte is bad
     * For most devices: Any block where the 1st Byte in the spare area of
     * the 1st or 2nd page(if the 1st page is Bad) does not contain FFh is a Bad Block
     * @param aBlockIndex index of block to check
     * @return true if block is valid, false if damaged
     */
    bool isBlockValid(UINT aBlockIndex);

    
    /**
     * Checks a byte in the spare area, which indicates if the block is erased
     * If the 1st or 2nd page(if the 1st page is Bad) does not contain FFh it is used
     * @param aBlockIndex index of block to check
     * @return true if block is erased, false if in use
     */
    bool isBlockErased(UINT aBlockIndex);

    
    /**
     * Sets a byte in the spare area of the first page to indicate that the block is damaged
     * This method is the corresponding to isBlockDamaged()
     * @param aBlockIndex index of block
     */
    void markBlockAsDamaged(USHORT aBlockIndex);

    
    /**
     * Sets a byte in the spare area of the first page to indicate that the block is used
     * (not erased). This method is the corresponding to isBlockErased()
     * @param aBlockIndex index of block
     */
    void markBlockAsUsed(USHORT aBlockIndex);

    



#endif	/* RAMIO_H */

