

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
* @file debug.h
* @date 2008/06/17 10:46
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief debug functions (header)
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "hw_predefines.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/** To be used from printfs, to control screen poss **/
#define SCREEN_CLEAR "\x1B[2J"
#define SCREEN_CLEAR_LINE "\x1B[K"
#define SCREEN_MOVE_CURSOR_TO  "\x1B[%d;%dH"
#define SCREEN_RESET  "\x1B[0m"
#define SCREEN_BLACK  "\x1B[30m"
#define SCREEN_RED    "\x1B[31m"
#define SCREEN_GREEN  "\x1B[32m"
#define SCREEN_YELLOW "\x1B[33m"
#define SCREEN_BLUE   "\x1B[34m"
#define SCREEN_WHITE  "\x1B[37m"

/// counts how many erros have been detected. Normaly it shall be 0
extern unsigned long errorCounter;

/// register the error message of the last Error occured: ist is a uniq pointer too
extern const char* errorMsg;
// extern unsigned long errorCode;


/** Writes 0 terminated  1/0 string representing the input 
 * value, takes the lower 'len' bits
 * warning: it writes in outbuf 'len' bytes + a terminating 0 (len +1)
 * @param[in]  val 4-byte to be represented as list of bits
 * @param[in]  len number of bits used within val
 * @param[in/out]  outBuf buffer to write string 
 * @return  outBuf buffer to write string
 */
char* formatBinary(long val, int len, char* outBuf);

/** Printf protected with a semaphore: Thread safe 
     Warinig: %3.9f means 3 digits then . then 9 digits
 **/
void PRINTF(const char* fmt, ...) __attribute__((__format__(__printf__,1,2)));



/** prints only if negligibleLevel <= printfVerbosity, Important msgs have lower negligibleLevel **/ 
void PRINTF_CONDITIONAL(int negligibleLevel, const char* fmt, ...) __attribute__((__format__(__printf__,2,3)));


/** Writes an error text with a leading title to stdout and keeps 
 * a counter. Used to report programming errors which shall be corrected.
 */
void ERROR(const char* text);

/** Print a character bitwise, highest bit first. 
 *  obsolete! use %b in PRINTF
 *  @param[in]  val 8-bit value to be printed
 **/
//void printBin(unsigned char val); obsolete: use %b in PRINTF

/** Prints an array of bytes as hex values 
 *
 * @param[in] titel leading text in the line printed before values
 * @param[in] ptr pointer to array to be printed as unsigned char
 * @param[in] len number of characters, respectively bytes in the array
 */
void printCharsHex(const char* titel, void* ptr, unsigned int len);
void printCharsHex_Conditional(int negligibleLevel, const char* titel, void* ptr, unsigned int len);

/** Function to read a character from stream, in case one has been written.
 * Normal getchar (in linux) will stop the complete execution. 
 * In order to avoid waiting first isCharRaedy() is called.
 * The implemetation is platform dependend defined in getcharnowait.cpp.
 *
 * @return latest character read, -1 if no character is available
 */
char getcharNoWait();

/** Checks if a new character has been received from stdin.
 * Associated to getcharNoWait().
 * Returns true if a new character is available in order to read it with getcharNoWait.
 *
 * @return 1 if a new character is in the buffer, 0 otherwise
 */
bool isCharReady();

/**
 * Get a string using getcharNoWait().
 * Returns If a string is ready it returns a pointer to it, else it returns 0 (no wait).
 * The maximal length of a string is 120 chars (due to internal buffers).
 * The String will not be modified until the next call
 * to getsNoWait.
 * Not Thread save. Only one reader from stdin is allowed.
 * Else... I dont know, try what it does and let me know.
 */
char* getsNoWait();

/**
 * if you activate the topic cat input, all input string from
 * terminal will be distributed to the topic charInput
 * until you call a getcharNoWait() or getsNoWait();
 */

void activateTopicCharInput();

/**
 * Like in linux fflush(stdout)
 */
void FFLUSH();

#ifndef NO_RODOS_NAMESPACE
}
#endif

/** Sets the pirntf verbosity, 0 == no printf, 100 = print alls
 ** se tutorials/cors/orintfverbosity ***/

extern int printfVerbosity;

#endif /* __DEBUG_H__ */
