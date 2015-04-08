

/**
* @file checksumes.cc
* @date 2008/04/22 17:08
* @author Lutz Dittrich, Sergio Montenegro
*
* @brief small support functions
*
*/
#include <stdint.h>
#include <string_pico.h>
//#include "hw_specific.h"
//#include "rodos.h"
#include "checksumes.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/****************************************************/


#define ROTATE_RIGHT(c) { if ((c) & 01) (c) = ((c) >>1) | 0x8000; else (c) >>= 1; }


/** Computes a 16-bit checksum (len in bytes) adding bytes and rotating result */

uint32_t checkSum(uint8_t *buf, int len) {

	register uint32_t checksum = 0; /* The checksum mod 2^16. */
	register int cnt;

	for(cnt = 0; cnt < len; cnt++) {
		ROTATE_RIGHT (checksum);
		checksum += buf[cnt];
		checksum &= 0xffff; /* Keep it within bounds. */
	}
	return checksum;

}



/** computes a 16-bit crc in a non optimized way **/

uint32_t computeCrc(const uint8_t* buf, int32_t len, int32_t initialValue) {

    int32_t currentValue = initialValue;

    for(int charCnt = 0; charCnt < len; charCnt++) {
        char curChar = buf[charCnt];
        for(int bitCnt = 0; bitCnt < 8; bitCnt++) {
            if((curChar & 0x80) ^ ((currentValue & 0x8000) >> 8)) {
                currentValue = ((currentValue << 1)  ^ 0x1021) & 0xFFFF; // Standard Polinom for CCSDS
            } else {
                currentValue = (currentValue << 1) & 0xFFFF;
            }
            curChar = curChar << 1;
        }
    }
    return currentValue;

}


/** An optimized crc generator.
  * first it generates a luck up table to speed up the crc computation
  */

CRC::CRC() {
		for (int i=0; i < 256; i++) {
			int tmp=0;
			if ((i & 1) != 0)   tmp=tmp ^ 0x1021;
			if ((i & 2) != 0)   tmp=tmp ^ 0x2042;
			if ((i & 4) != 0)   tmp=tmp ^ 0x4084;
			if ((i & 8) != 0)   tmp=tmp ^ 0x8108;
			if ((i & 16) != 0)  tmp=tmp ^ 0x1231;
			if ((i & 32) != 0)  tmp=tmp ^ 0x2462;
			if ((i & 64) != 0)  tmp=tmp ^ 0x48C4;
			if ((i & 128) != 0) tmp=tmp ^ 0x9188;
			lookUpTable [i] = tmp;
		}
}

uint32_t CRC::computeCRC(uint8_t* buf, int32_t len, int32_t initialValue) {

		int32_t currentValue = initialValue;
		for(int i=0; i < len; i++) {
			currentValue = (((currentValue << 8) & 0xFF00) ^
					lookUpTable [(((currentValue >> 8)^ buf[i]) & 0x00FF)]);
		}
		return currentValue;
}


/** Delivers a 16-bit hash value for a string.
 *  both bytes contain only printable characters
 */

uint16_t hash(const char* str) {
	const uint8_t* buf = (uint8_t*) str;
	uint16_t crc = (uint16_t)computeCrc(buf, (int32_t)strlen(str), 0xffff);

	/** To make only printable characters, else it were a normal crc value **/

		uint8_t a = (crc >> 8) & 0xff;
		uint8_t b = (crc & 0xff);

		uint32_t range = 0x7e - 0x20; // Printable ascii chars
		if(a <= 0x20 || a >= 0x7e) a = (a % range) + 0x20;
		if(b <= 0x20 || b >= 0x7e) b = (b % range) + 0x20;

	/******/

	return (a << 8) | b;
}



/** Computes a checksum (len in bytes) */
uint32_t checkSumXor(uint8_t *buf, int len) {

    int32_t *data = (int32_t *)buf;
    int32_t cksum;
    uint32_t i;

    cksum = 0;
    for (i = 0; i < len/sizeof(int32_t); ++i) {
        cksum ^= data[i];
    }
    return cksum;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif


