#include <stdint.h>
#include "stream-bytesex.h"
#include "macros_bits_bytes.h"

/** Bytesex Convertions
 *  byte string allwas in Big-endian (motorola) format
 *  other formats in host representation
 *
 * Author    : Sergio Montenegro
 * Date      : 23.10.2008
 * Descript  : Byte-String to Values
 */

/************ From a gib-endian byte stram  to internal representation ****/

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

uint16_t bigEndianToInt16_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return   (((uint16_t)(byteStream[0])) << 8)
           |  ((uint16_t)(byteStream[1]));
}

uint32_t bigEndianToInt32_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return    (((uint32_t)(byteStream[0])) << 24)
            | (((uint32_t)(byteStream[1])) << 16)
            | (((uint32_t)(byteStream[2])) <<  8)
            |  ((uint32_t)(byteStream[3]));
}


uint64_t bigEndianToInt64_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return   (((uint64_t)(byteStream[0])) << 56)
           | (((uint64_t)(byteStream[1])) << 48)
           | (((uint64_t)(byteStream[2])) << 40)
           | (((uint64_t)(byteStream[3])) << 32)
           | (((uint64_t)(byteStream[4])) << 24)
           | (((uint64_t)(byteStream[5])) << 16)
           | (((uint64_t)(byteStream[6])) <<  8)
           |  ((uint64_t)(byteStream[7]));
}


float bigEndianToFloat(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        float value;
        uint32_t lvalue;
    } value_union;
    value_union.lvalue = bigEndianToInt32_t(byteStream);
    return value_union.value;
}

double bigEndianToDouble(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        double value;
        uint64_t llvalue;
    } value_union;
    value_union.llvalue = bigEndianToInt64_t(byteStream);
    return value_union.value;
}

/************ From internal representation to big-endian  byte stram  ****/

void int16_tToBigEndian(void* buff, uint16_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 8) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 0) & 0xFF);
}

void int32_tToBigEndian(void* buff, uint32_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 24) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 16) & 0xFF);
    byteStream[2] = (uint8_t) ((value >>  8) & 0xFF);
    byteStream[3] = (uint8_t) ((value >>  0) & 0xFF);
}


void int64_tToBigEndian(void* buff, uint64_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 56) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 48) & 0xFF);
    byteStream[2] = (uint8_t) ((value >> 40) & 0xFF);
    byteStream[3] = (uint8_t) ((value >> 32) & 0xFF);
    byteStream[4] = (uint8_t) ((value >> 24) & 0xFF);
    byteStream[5] = (uint8_t) ((value >> 16) & 0xFF);
    byteStream[6] = (uint8_t) ((value >>  8) & 0xFF);
    byteStream[7] = (uint8_t) ((value >>  0) & 0xFF);
}

void floatToBigEndian(void* buff, float value_) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        float value;
        unsigned lvalue;
    } value_union;
    value_union.value = value_;
    int32_tToBigEndian(byteStream, value_union.lvalue);
}

void doubleToBigEndian   (void* buff, double value_) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        double value;
        uint64_t llvalue;
    } value_union;
    value_union.value = value_;
    int64_tToBigEndian(byteStream, value_union.llvalue);
}



/*************************************************/

void setBitInByteStream (void *byteStream, int bitIndex, bool value) {
    int byteIndex = bitIndex / 8;
    int bitPos =    bitIndex % 8;
    unsigned char* bytes = (unsigned char*)byteStream;

    SET_BIT(bytes[byteIndex], bitPos, (value? 1 : 0));
}


int getBitFromByteStream(const void *byteStream, int bitIndex) {

    unsigned char* bytes = (unsigned char*)byteStream;

    unsigned char selectedByte = bytes[bitIndex/8];
    return GET_BIT(selectedByte, (bitIndex % 8));

}


/***********************************************************/


/** Sets a bit field form 0 to 16 bits for CCSDS bitd fields 
  * Warning: CCSDS -> Bit 0 = most significant bit!
  **/

void setBitField(void* buffer, int bitPos, int numOfBits, uint32_t val) {
    unsigned char* buf = (unsigned char*) buffer;
    int byteIndex = bitPos / 8;
    bitPos        = bitPos % 8;
    int shifts    = 24 - (bitPos + numOfBits);
    int32_t mask     = ((1 << numOfBits) -1) << shifts;

    val = val << shifts;

    // get the word as big-endian (CPU independent)
    uint32_t word    = (buf[byteIndex] << 16);
	if(bitPos+numOfBits > 8 ) word |=  buf[byteIndex+1] << 8; //Do not read byte not required to avoid reading beyond the buffer
	if(bitPos+numOfBits > 16) word |=  buf[byteIndex+2];

    word  &= ~ mask;        // Clear bit field
    word  |=  mask & val;   // set the correspondig bits

    // store the word as big-endian (CPU independent)
    buf[byteIndex]                             = (word >> 16) & 0xff; // Most Significant first
    if(bitPos+numOfBits > 8 ) buf[byteIndex+1] = (word >> 8 ) & 0xff;
    if(bitPos+numOfBits > 16) buf[byteIndex+2] =  word        & 0xff;
}


/** gets a bit field form 0 to 16 bits for CCSDS bitd fields 
  * Warning: CCSDS -> Bit 0 = most significant bit!
  **/

uint32_t getBitField(void* buffer, int bitPos, int numOfBits) {
    unsigned char* buf = (unsigned char*) buffer;
    int byteIndex = bitPos / 8;
    bitPos        = bitPos % 8;
    int shifts    = 24 - (bitPos + numOfBits);
    int32_t mask     = ((1 << numOfBits) -1);

    // get the word as big-endian (CPU independent)
    uint32_t word    = (buf[byteIndex] << 16);
    if(bitPos+numOfBits > 8 ) word |=  buf[byteIndex+1] << 8; //Do not read byte not required to avoid reading beyond the buffer
    if(bitPos+numOfBits > 16) word |=  buf[byteIndex+2];

    return  (word >> shifts) &  mask;
}

uint32_t getSetBits(const uint32_t &value) {

    // reference: Hacker's Delight, p. 66, Figure 5-2
    uint32_t x = value;
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003F;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif


