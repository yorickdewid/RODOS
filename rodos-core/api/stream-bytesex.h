
#pragma once

/** Bytesex Convertions
 * byte string allwas in Big-endian (motorola) format
 * other formats in host representation
 *
 * Author    : Sergio Montenegro
 * Date      : 23.10.2008
 * Descript  : Byte-String to Values
 */

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/************ From a big-endian byte stream  to internal representation ****/

uint16_t  bigEndianToInt16_t (const void* byteStream);
uint32_t  bigEndianToInt32_t (const void* byteStream);
uint64_t  bigEndianToInt64_t (const void* byteStream);
float  	  bigEndianToFloat   (const void* byteStream);
double    bigEndianToDouble  (const void* byteStream);

/************ From internal representation to big-endian  byte stream  ****/

void int16_tToBigEndian (void* byteStream, uint16_t value);
void int32_tToBigEndian (void* byteStream, uint32_t value);
void int64_tToBigEndian (void* byteStream, uint64_t value);
void floatToBigEndian   (void* byteStream, float    value);
void doubleToBigEndian  (void* byteStream, double   value);


void setBitInByteStream (void *byteStream, int bitIndex, bool value);
int getBitFromByteStream(const void *byteStream, int bitIndex);

/// sets serval bits in a bigEndian bitfeld of max 16 bits (warning: ccsds: 0 = msb)
void setBitField(void* buffer, int bitPos, int numOfBits, uint32_t val);
/// gets severa bits from a bigEndia bitfled of mayx 16 bits (warning: ccsds: 0 = msb)
uint32_t getBitField(void* buffer, int bitPos, int numOfBits);

/// gets the number of bits set 
uint32_t getSetBits(const uint32_t &value);


#ifndef NO_RODOS_NAMESPACE
}
#endif

