/* 
 * File:   setup.h
 * Author: gabriel reimers
 *
 * Created on March 25, 2012, 1:41 PM
 */

#ifndef SETUP_H
#define	SETUP_H


//size of a single page in bytes
#define PAGE_BYTE_SIZE 2048

//size of the spare area of each page in bytes (this must not be smaller than 32)
#define SPARE_BYTES_PER_PAGE 64

//number of pages per block
#define PAGES_PER_BLOCK 64

//number of blocks used for user data (these are visible to the fat fs)
#define NO_OF_PUBLIC_BLOCKS 896

//number of spare blocks. these are used as a replacement for bad blocks
//for the fat-fs these block are invisible
//the spare blocks are located at the very end of the disk
#define NO_OF_SPARE_BLOCKS 127


/////////////////////////////////
//DISK IO
/////////////////////////////////
//ecc will always be written. this flag only applies to read correction
#define ENABLE_ECC false

//this is only experimental. this also requires ENABLE_ECC to be true
//if 1 or 2, diskio will read and error correct after writing data
//this allows to detect damaged blocks on write even if flash chip status is not available
//if set to 0: deactivated
//if set to 1: blocks will be marked as damaged if >= 1 error is detected in read after write check
//             this is not recommeneded because 1-bit errors are so common
//if set to 2: blocks will be marked as damaged if >= 2 error is detected in read after write check
//              this will prevent data loss by uncorrectable errors (>=2)
#define READ_AFTER_WRITE_CHECK 2

//if enabled speeds up read/write dramatically but requires enough RAM
#define USE_BLOCK_SIZE_BUFFER false

//enable if possible. this will speed up read/write a lot
#define USE_ON_CHIP_PAGE_COPY true

//if true this will cause the entire disk to be erased on init
//normally the bad block table and spare area are never erased
//set this in true if the bad block table is damaged
#define FORCE_ERASE_BB_TABLE true


////////////////////////////////
//HAMMING CODE
////////////////////////////////
//byte length of data (without ecc bits)
#define ECC_DATA_BYTE_LENGTH (PAGE_BYTE_SIZE)
// -> data bit length = ECC_DATA_BYTE_LENGTH * 8 

//ecc bit length is the lenth of the hamming code bits
//choose ecc bit length so that: 2^x > data bit length
#define ECC_BIT_LENGTH 14

//ecc bits need to be initialized with 0
//this array must match the ecc bit length
#define ECC_BITS_INIT_VALUE {0,0,0,0,0,   0,0,0,0,0,  0,0,0,0}

//byte length so that bitlength + 2 parity bits will fit in it
//so byte length = ceil(ld(bitlength + 2))
#define ECC_BYTE_LENGTH 2
#define ECC_BYTES_INIT_VALUE {0x00, 0x00}


//these init arrays must have at least data bit-lenght items
//this array must contain at least data-bit-length, each value being (2^x)-1
#define ECC_PARITY_SKIP_COUNTERS_ARRAY {0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767}
//this array must contain at least data-bit-length, each value being 2^x
#define ECC_PARITY_SUM_COUNTERS_ARRAY {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768}



#endif	/* SETUP_H */

