/* 
 * File:   HammingCode.h
 * Author: Gabriel Reimers
 *
 * Created on January 18, 2012, 12:58 PM
 */

#ifndef _HAMMINGCODE_H
#define	_HAMMINGCODE_H


#include "../typeDefs.h"
#include "pathSetup.h"
#include FLASHIO_SETUP_FILE


#define ECC_DATA_BIT_LENGTH (ECC_DATA_BYTE_LENGTH * 8)
#define ECC_FIELD_BIT_LENGTH (ECC_BYTE_LENGTH * 8)
#define ECC_START_POS (ECC_FIELD_BIT_LENGTH - ECC_BIT_LENGTH)
#define ECC_ECC_PARITY_POS (ECC_START_POS - 1)
#define ECC_DATA_PARITY_POS (ECC_START_POS - 2)
#define ECC_CODE_LENGTH (ECC_DATA_BIT_LENGTH + ECC_BIT_LENGTH)

/////////////////
//Public methods
///////////////////
/**
 * calculates extended hamming ecc code for provided data (will write to eccBuf)
 * bit pattern:
 * [0000... dataP eccP px ... p3 p2 p1 p0] bytes will be filled from right to left
 * dataP = extra data parity bit for faster and more secure error detection
 * eccP = extra ecc parity (over px...p0)
 * @param data byte array for wich ecc shall be calculated (must have ECC_DATA_BIT_LENGTH)
 * @param eccBuf data buffer where calculated ecc is written to (must have at least ECC_CODE_LENGTH)
 */
void ECCcodeForData(BYTE* data, BYTE* eccBuf);

/**
 * will correct data and or eccCode if possible, writes both back to parameters
 * returns number of errors:
 * 0: no error - nothing changed
 * 1: one bit error found and corrected
 * 2: two bit error detected but not corrected
 * errors with more than 2 bit will not be detected or corrected wrongly!
 * @param receivedData data with possible errors (overwritten if corrected)
 * @param receivedEccCode received ecc code (overwritten if corrected)
 * @return number of calculated errors (0 if none)
 */
int correctData(BYTE* receivedData, BYTE* receivedEccCode);



//////////////////
//Private Methods
//documented in .cpp file
//////////////////
bool bitAtIndex(BYTE* data, int i);
void setBitAtIndex(BYTE*data, int i, bool value);
BOOL parity(UINT value);
int dataIndexForHammingIndex(int i);
int intValueOfByteArray(BYTE* bytes, int arraySize);


#endif	/* _HAMMINGCODE_H */

