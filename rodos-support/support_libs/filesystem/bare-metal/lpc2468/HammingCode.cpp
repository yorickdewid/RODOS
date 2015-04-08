
// Copyright (c) 2012 Gabriel Reimers - (mr-fridge.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
// of the Software, and to permit persons to whom the Software is furnished to do 
// so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// v1.0 (modified) 2012-01-28

/*
This uses a modified version of the extended hamming code. 
ECC attributes:
- detects and corrects 1 error
- detects 2 errors
- nothing guaranteed for >2 errors (might be detected)

Code attributes:
- separates data from ecc bits

==========================
An intro to hamming code
==========================

A hamming code needs ceil(ld(dataBitLength)) bits. So for say 1024 bit you need 11 ecc bit.
These bits are called parity bits (p0, p1, p2, ...)

(Most sources on the internet claim that a hamming code must always have a length
that is x^2 including the parity bits. But this is not generally true as you will see.)

To (intuitively) calculate the parity bits you put them into your 
data at each position that is == ((x^2) -1). Namely: 0, 1, 3, 7, ... 
 
Example: 
Data:           1100 1010 
                0123 4567

Becomes:        __1_ 100_ 1010
                0123 4567 8901
                pp p    p     

As you can see you have 8 data bits and as 2^3 == 8 you need 3+1 parity bits.
These parity bits p0, p1, p2, p3 now are the blanks at positions 0, 1, 3, 7

The rule how to caclulate these parity bits is cumbersome to explain but
easy to see in an example:
 
   p0 =         __1_ 100_ 1010
                0 2  4 6  8 0 
   p0 => _+1+1+0+1+1 = 4 -> parity = 0
   
   p1 =         0_1_ 100_ 1010
                 12  45   89
   p1 => _+1+1+0+1+0 = 3 -> parity = 1
  
   p2 =         011_ 100_ 1010
                   3 456     1
   p2 => _+1+0+0+1 = 2 -> parity = 0
 
   p3 =         0110 100_ 1010
                        7 8901234
   p3 => _+1+0+1+0 = 2 -> parity = 0
  
So basically px is calculated by adding 2^x bit, skipping 2^x bit, adding 2^x, skipping 2^x....
Surprisingly each parity bit will only sum over data bits but not over other parity bits. 
However calculation starts with the parity bit itself which is blank (_) but is just handled like a 0
It is no problem if the calculation rule does not exaclty match the code length
like with p3 where not all required 8 positions can be summed up (in contrast to what wikipedia tells you). 
Just stop adding on the end of the code word.
 
Having calculated this you can just extract the parity bits to form a stand-alone ecc-code.
In this example it would be ecc = 0100 where the left most bit is p0. 
It will be easier for error correction if we flip this horizontally so p0 is the right most
because it then resembles a binary number where the right most bit is the least significant.
so we get ecc(flipped) = 0010.

Hamming Error correction
-----------------
So lets assume an error occurs. The hamming code can detect up to 2 bit errors and
is able to correct 1 error. This works quite easily: Suppose you receice transmitted data dT 
and the according transmitted eccT. You now just recalculate the ecc for dT and compar the result
with eccT. If they are the same, no error occurred (that we could detect).
If not the difference between ecc and eccT will tell you the position of the corrupted bit.
By xor-ing ecc and eccT you will get a bit vector that you can just interpret as a binary number
(that is why we flipped the ecc above). 
Example:
You receive data dT (from the example above but with and bit flip on pos 5)
             1100 1110 
             0123 4567
and eccT     0010 

now you calculate ecc for dT like above and will get 1000 (right most is p0)
xor(0010, 1000)= 1010 => 10 (as a binary number)
now you say "but 10 is wrong. the error is on pos 5"
yes it is BUT:
when inserting the binary bit blanks the error is on pos 9
__1_ 100_ 1110
0123 4567 8901
and because when no error occurs xor(ecc, eccT) = 0 we will need to shift our numbers by one
so: the correct error position = xor(ecc, eccT) - 1
so: 10 indeed is correct and to get the original data you just flip bit 10-1 = 9

Got it? Not so difficult, right?!

The hamming diestance of this code is 3 which means that we can detect and correct single
bit errors. Double bit errors however will be detected as error but cannot be 
traced back to an original code word (that is we see there is an error but we
cannot determine if it is a 2 bit or single bit error). This is why we use and
extended version of the hamming code here, adding two additional parity bits
for easier error detection. 
Or ecc array thus has the pattern: [dataParity, eccParity, px, ..., p2, p1, p0]
The data parity bit indicates the parity of all data bits, while the
eccParity bit stores the parity over all hamming parity bits p0 to px.
So this addP bit must be extracted first before calculating any xor values but it gives us
valueable information because we then know if a even or odd number of errors occurred
as well as if it was in the ecc part or in the data part.
This makes it possible to savely detect 2 errors and correct 1 error.


===================
How this code works
===================
We want to have the parity bits separated from the data bits but we don't
want to create copies of the data array with the ecc blanks inserted. 
Luckily the ecc bits are all inserted at pos x^2-1 so we can just 'simulate'
the ecc positions. When we iterate of the data array we check if the position 
is a power to two minus one and if it is we either return 0 (when calculating the ecc)
or we just return the corresponding bit from the separated ecc array.
For this we have a function "convertCodeWordIndexToReal" which will return -1 if
an index is a parity bit ((2^x)-1) or just the correct data bit index in any other case.

 */


#include "HammingCode.h"
#include "rodos.h"


static unsigned short byteParity[] ={
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0,
    1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1,
    0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0,
    1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1,
    0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
    0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1,
    0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1,
    0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0,
    0, 1, 0, 1, 1, 0
};

int parityOfData(BYTE* data, int length) {
    int p = 0;
    int i;
    for (i = 0; i < length; i++) {
        p += byteParity[data[i]];
    }
    return (p % 2);
}

/**
 * calculates extended hamming ecc code for provided data (will write to eccBuf)
 * bit pattern:
 * [0000... dataP eccP px ... p3 p2 p1 p0] bytes will be filled from right to left
 * dataP = extra data parity bit for faster and more secure error detection
 * eccP = extra ecc parity (over px...p0)
 * @param data for which the ecc shall be calculated (will not be changed)
 * @param eccBuf where the ecc is written to (will be changed, init with 0)
 */
void ECCcodeForData(BYTE* data, BYTE* eccBuf) {
    int i;
    bool bit;
    int dataBitParity = 0; //this is for the extended hamming code. an additional parity bit.
    int eccBitParity = 0; //this is for the extended hamming code. an additional parity bit.

    int paritySkipCounters[] = ECC_PARITY_SKIP_COUNTERS_ARRAY;
    int paritySumCounters[] = ECC_PARITY_SUM_COUNTERS_ARRAY;
    int parityBits[ECC_BIT_LENGTH] = ECC_BITS_INIT_VALUE;

    for (i = 0; i < ECC_BYTE_LENGTH; i++) {
        eccBuf[i] = 0x00;
    }
    //we iterate over the data array using the skipCounters and sumCounter
    //to calculate the parityBits (skip 2^x, sum 2^x...)
    int byte;
    int b;
    for (byte = 0; byte < (ECC_CODE_LENGTH / 8); byte++) {
        if (data[byte] == 0) {
            continue; //this will speed up things a lot. Ohh yeah!
        }
        for (b = 0; b < 8; b++) {
            i = byte*8 + b;
            //printf("ECC > regarding bit %i\n", i);

            int dataIndex = dataIndexForHammingIndex(i);
            if (dataIndex < 0) {
                //index = parity bit. -> regard as zero
                bit = false;
            } else {
                bit = bitAtIndex(data, dataIndex);
                if (bit) {
                    dataBitParity++;
                }
            }

            //printf("pos %i %i:", i, bit);

            //now iterate over parity bits and check if bit is skipped or added
            int p;
            for (p = 0; p < ECC_BIT_LENGTH; p++) {

                if (paritySkipCounters[p] == 0) { //add
                    if (bit) {
                        parityBits[p] += 1;
                    }
                    //printf(" p%i now %i", p, parityBits[p]);
                    paritySumCounters[p] -= 1;
                    if (paritySumCounters[p] == 0) {
                        paritySkipCounters[p] = (1 << p); //(reset to 0,2,4,8,16...)
                        paritySumCounters[p] = (1 << p); //(reset to 0,2,4,8,16...)
                    }
                } else { //skip
                    paritySkipCounters[p] -= 1;
                }
            }
            //printf("\n");
        }
    }



    //will swap order so msb is leftmost (left: px; right: p0)
    int p;
    for (p = 0; p < ECC_BIT_LENGTH; p++) {
        //printf("eccCode > parity %i: %i\n", p, parityBits[p]);
        if (parityBits[p] % 2 == 0) {
            setBitAtIndex(eccBuf, (ECC_FIELD_BIT_LENGTH - p - 1), false);
        } else {
            setBitAtIndex(eccBuf, (ECC_FIELD_BIT_LENGTH - p - 1), true);
            eccBitParity++;
        }
    }

    UINT c = intValueOfByteArray(eccBuf, ECC_BYTE_LENGTH);
    //printf("calculate > code %x \n", c);

    setBitAtIndex(eccBuf, (ECC_ECC_PARITY_POS), (eccBitParity % 2 != 0));
    setBitAtIndex(eccBuf, (ECC_DATA_PARITY_POS), (dataBitParity % 2 != 0));
    c = intValueOfByteArray(eccBuf, ECC_BYTE_LENGTH);
    //printf("calculate > code dp %i ep %i %x\n", dataBitParity, eccBitParity, c);


}

/** will correct data and or eccCode if possible
 * returns number of errors:
 * 0: no error - nothing changed
 * 1: one bit error found and corrected
 * 2: two bit error detected but not corrected
 * errors with more than 2 bit will not be detected or corrected wrongly!
 */
int correctData(BYTE* receivedData, BYTE* receivedEccCode) {

    //ECC-Code format:
    //0...[data parity bit][ecc parity bit][hamming code]

    UINT receivedHamming = intValueOfByteArray(receivedEccCode, ECC_BYTE_LENGTH);
    //printf("correct > received hamming %x\n", receivedHamming);
    receivedHamming &= ~(3 << ECC_BIT_LENGTH); //delete data and ecc parity bits from ecc (3 == 11b)

    BOOL receivedDataParity = bitAtIndex(receivedEccCode, ECC_DATA_PARITY_POS);
    BOOL receivedEccParity = bitAtIndex(receivedEccCode, ECC_ECC_PARITY_POS);
    //printf("correct > original hamming %x with data parity %i ecc parity %i\n", receivedHamming, receivedDataParity ? 1 : 0, receivedEccParity ? 1 : 0);

    ////////////////////////////////////////
    //now recalculate ecc to check for errors
    ////////////////////////////////////////
    BYTE calculatedEccCode[ECC_BYTE_LENGTH] = ECC_BYTES_INIT_VALUE;
    ECCcodeForData(receivedData, calculatedEccCode);

    UINT calculatedHamming = intValueOfByteArray(calculatedEccCode, ECC_BYTE_LENGTH);
    calculatedHamming &= ~(3 << ECC_BIT_LENGTH); //delete data and ecc parity bits from ecc (3 == 11b)

    BOOL calcDataParity = bitAtIndex(calculatedEccCode, ECC_DATA_PARITY_POS);
    BOOL calcEccParity = parity(receivedHamming);

    //BOOL  testDataP = parityOfData(receivedData, DATA_BYTE_LENGTH) != 0;
    //printf("correct > calc data parity %i test data parity %i\n", calcDataParity ? 1 : 0, testDataP ? 1 : 0);

    //printf("correct > calculated hamming %x with data parity %i ecc parity %i\n", calculatedHamming, calcDataParity ? 1 : 0, calcEccParity ? 1 : 0);


    //////////////////
    //now we got everything to track down and silently replace the error
    //first we check the parity bits. if there is no error all parity bits must 
    //be unchanged. 
    /////////////////
    unsigned long errorPos = (calculatedHamming ^ receivedHamming); //xor
    if (errorPos == 0) { //no error found
        if (receivedDataParity == calcDataParity && receivedEccParity == calcEccParity) {
            //printf("correct > no error found\n");
            return 0;
        } else if (receivedDataParity != calcDataParity) {
            setBitAtIndex(receivedEccCode, ECC_DATA_PARITY_POS, calcDataParity);
            xprintf("correct > data parity bit wrong\n");
            return 1;
        } else if (receivedEccParity != calcEccParity) {
            setBitAtIndex(receivedEccCode, ECC_ECC_PARITY_POS, calcEccParity);
            UINT correctedHamming = intValueOfByteArray(receivedEccCode, ECC_BYTE_LENGTH);

            xprintf("correct > ecc parity bit wrong original %x calc %x corrected %x recP %i calcP %i\n", receivedHamming, calculatedHamming, correctedHamming, receivedEccParity ? 1 : 0, calcEccParity ? 1 : 0);
            return 1;
        } else {
            xprintf("correct > two wrong parities -> uncorrectable\n");
            return 2;
        }
    }

    ///////////////////////////////////////////////////////////////
    //an error was found, so lets have a look at the error position.
    //if there is a single error exactly one parity bit must be wrong
    ///////////////////////////////////////////////////////////////
    errorPos -= 1; //error syndrom pos starts at 1 because 0 == no error
    int realPos = dataIndexForHammingIndex(errorPos);
    if (realPos < 0) {//error in ecc code

        if (receivedEccParity == calcEccParity || receivedDataParity != calcDataParity) {
            //ecc error parity must be unequal and no additional data error
            xprintf("correct > error in ecc and 2 parity errors -> uncorrectable\n");
            return 2;
        }
        int eccPos = ECC_FIELD_BIT_LENGTH - 1;
        while (errorPos > 0) {
            errorPos = errorPos >> 1;
            eccPos--;
        }
        BOOL wrongBitValue = bitAtIndex(receivedEccCode, eccPos);
        setBitAtIndex(receivedEccCode, eccPos, !wrongBitValue);
        xprintf("correct > error in ecc corrected\n");
        return 1;

    } else {//error in data
        if (receivedDataParity == calcDataParity || receivedEccParity != calcEccParity) {
            //data error parity must be unequal and no additional ecc error
            xprintf("correct > error in data and 2 parity errors -> uncorrectable "
                    "%x calc %x  recP %i calcP %i errPos %i\n", receivedHamming,
                    calculatedHamming, receivedEccParity ? 1 : 0,
                    calcEccParity ? 1 : 0, realPos);
            return 2;
        }
        BOOL wrongBitValue = bitAtIndex(receivedData, realPos);
        setBitAtIndex(receivedData, realPos, !wrongBitValue);
        xprintf("correct > error in data corrected\n");
        return 1;
    }
}

/**
 * This is for internal usage only.
 * We do not keep a separate code word array with the parity bits between the data bits.
 * so we need to recalculate hamming code word indexes to the real data array index
 * As parity bits are not inside the data array this will return -1 for these positions.
 * @param i
 * @return data index, -1 if i==parity index ((2^x)-1)
 */
int dataIndexForHammingIndex(int i) {
    int realIndex = i; //i max: Code_length, real max: datalength
    int p;
    int nextParityPos = 0;
    for (p = 0; p < ECC_BIT_LENGTH; p++) {
        nextParityPos = (1 << p) - 1; //0,1,3,7,15,31...
        if (i == nextParityPos) {
            return -1; //parityposition is not defined 
        }
        if (i < nextParityPos) {
            break;
        }
        realIndex -= 1;
    }
    return realIndex;
}

/**
 * calculates the parity of an integer. 
 * @param value
 * @return true if parity is odd, false if even
 */
BOOL parity(UINT value) {
    BOOL parity = false;
    UINT parityMask = 1;
    while (parityMask != 0) {
        if (parityMask & value) {
            parity = !parity;
        }
        parityMask = parityMask << 1;
    }
    return parity;
}

/**
 * returns the bit at position i of a byte array
 * 0 is the leftmost bit
 * @param data
 * @param i index
 * @return true if 1, false if 0
 */
bool bitAtIndex(BYTE* data, int i) {

    int byteIndex = i / 8;
    int offset = i % 8;
    BYTE b = data[byteIndex];
    BYTE mask = 1;
    mask = mask << (7 - offset);
    //printf("getBit > byte: %i offset: %i mask %i\n",byteIndex, offset, mask);

    return ((mask & b) != 0);
}

/**
 * sets the bit at position i of a byte array.
 * 0 is the leftmost bit
 * @param data byte array (will be modified)
 * @param i index
 * @param value true for 1, false for 0
 */
void setBitAtIndex(BYTE*data, int i, bool value) {

    int byteIndex = i / 8;
    int offset = i % 8;
    BYTE mask = 1;
    mask = mask << (7 - offset);
    //printf("setBit > byte: %i offset: %i mask %i\n",byteIndex, offset, mask);
    if (value == false) {
        data[byteIndex] &= ~mask; //pull to 0
    } else {
        data[byteIndex] |= mask; //push to 1
    }
}

/**
 * Returns integer value of byte arrays (in big endian - the only true endian)
 * this can of course only convert arrays up to intmax = 32 bit = 4 byte
 * @param bytes
 * @param arraySize
 * @return 
 */
int intValueOfByteArray(BYTE* bytes, int arraySize) {
    UINT intValue = 0;
    int i;
    for (i = 0; i < arraySize; i++) {
        intValue += (bytes[i] << (8 * (arraySize - i - 1)));
    }
    return intValue;
}









