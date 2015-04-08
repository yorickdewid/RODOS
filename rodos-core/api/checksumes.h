

/*********************************************************** Copyright 
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 ** 
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 ** 
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 ** 
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 ** 
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 ** 
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **  
 ****************************************************************************/


/**
* @file debuecksumes.h
* @date 2008/06/17 10:46
* @author Sergio MOntenegro
*
* @brief simple misc functions
*/

#pragma once


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/** Computes a 16 bit checksum (len in bytes) adding bytes and rotating result */
uint32_t checkSum(uint8_t *buf, int len) ;


/** computes a 16 bit crc in a non optimized way **/
uint32_t computeCrc(const uint8_t* buf, int32_t len, int32_t initialValue);


/** An optimized crc generator.
  * first it generates a luck up table to speed up the crc computation
  * use only one in the system. more is not required.
  */

class CRC {
    uint32_t lookUpTable[256];
public:
    CRC(); 
    uint32_t computeCRC(uint8_t* buf, int32_t len, int32_t initialValue);
};


/** Delivers a 16 bit  hash value for a string.
 *  both bytes contain only printable characters
 */

uint16_t hash(const char* str);


/// Computes the checkSum of an array of bytes, according to CCSDS algorithm.
/// Warning: There are several checksum standars. See rodos checksum too
uint32_t checkSumXor(uint8_t *buf, int len);


#ifndef NO_RODOS_NAMESPACE
}
#endif

