/* -*- Mode: C++ -*- */

/*********************************************************** Copyright
 **
 ** Copyright (c) 2010, German Aerospace Center (DLR)
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
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2010 DLR
 *
 * @brief Macros to work with bits and bitmasks.
 *
 * Usable from C and Assembler unless noted otherwise.
 */

#ifndef RODOS_HW_LEON2_BITMAGIC_H
#define RODOS_HW_LEON2_BITMAGIC_H


/**
 * HW_POS(X) returns the bit position for a bitfield defined as (POSITION, WIDTH).
 *
 * I.e., this is a simple define returning the first value of a pair.
 */
#define HW_POS(_SPEC)                HW_POS_AUX _SPEC
#define HW_POS_AUX(_POS,_WIDTH)      (_POS)


/**
 * HW_WIDTH(X) returns the number of bits of a bitfield defined as (POSITION, WIDTH).
 *
 * I.e., this is a simple define returning the second value of a pair.
 */
#define HW_WIDTH(_SPEC)              HW_WIDTH_AUX _SPEC
#define HW_WIDTH_AUX(_POS,_WIDTH)    (_WIDTH)


/**
 * HW_UMAX(X) returns the 1-mask of N bits, the largest possible value.
 *
 * E.g., HW_UMAX(8) return 0x000000ff, i.e., 8 one-bits
 */
#define HW_UMAX(_WIDTH)              (~(~0 << (_WIDTH)))


/**
 * HW_BITS(X) returns the shifted mask to extract a bitfield defined as (POSITION, WIDTH).
 *
 * E.g., HW_BITS((8,4)) return 0xf00, i.e., 4 one-bits at position 8.
 */
#define HW_BITS(_SPEC)               (HW_UMAX(HW_WIDTH(_SPEC)) << HW_POS(_SPEC))


/**
 * HW_INSERT(_SPEC,_VAL) inserts the value to the right position.
 * The function only masks those bits that are valid.  The function does not
 * perform culling (HW_CULL()).
 *
 * This is the reverse of HW_EXTRACT().
 */
#define HW_INSERT(_SPEC,_VAL)        (HW_BITS(_SPEC) & ((_VAL) << HW_POS(_SPEC)))


/**
 * HW_EXTRACT(X) extracts the given bits from a value.
 *
 * E.g., HW_EXTRACT((8,4), 0x12345) return 3.
 *
 * This is the reverse of HW_INSERT().
 */
#define HW_EXTRACT(_SPEC,_VAL)       (((_VAL) >> HW_POS(_SPEC)) & HW_UMAX(HW_WIDTH(_SPEC)))


/**
 * Align a number, rounding down.
 * Works only with alignments that are powers of two!
 *
 * Examples:
 *  HW_ALIGN_DOWN(16,8)  = 16
 *  HW_ALIGN_DOWN(25,8)  = 24
 */
#define HW_ALIGN_DOWN(_ALIGN,_VAL)   ((_VAL) & ~((_ALIGN) - 1))


/**
 * Align a number, rounding up.
 * Works only with alignments that are powers of two!
 *
 * Examples:
 *  HW_ALIGN_UP(16,8)  = 16
 *  HW_ALIGN_UP(25,8)  = 32
 */
#define HW_ALIGN_UP(_ALIGN,_VAL)     HW_ALIGN_DOWN(_ALIGN, (_VAL) + ((_ALIGN) - 1))


/**
 * Round up to next power of two minus 1. */
#define HW_NEXT_POWER2MINUS1(_VAL)   HW_NEXT_P2M1_AUX1(_VAL)

#define HW_NEXT_P2M1_AUX1(_VAL)      HW_NEXT_P2M1_AUX2((_VAL)  | ((_VAL) >> 1))
#define HW_NEXT_P2M1_AUX2(_VAL)      HW_NEXT_P2M1_AUX4((_VAL)  | ((_VAL) >> 2))
#define HW_NEXT_P2M1_AUX4(_VAL)      HW_NEXT_P2M1_AUX8((_VAL)  | ((_VAL) >> 4))
#define HW_NEXT_P2M1_AUX8(_VAL)      HW_NEXT_P2M1_AUX16((_VAL) | ((_VAL) >> 8))
#define HW_NEXT_P2M1_AUX16(_VAL)     ((_VAL) | ((_VAL) >> 16))


/**
 * HW_CROP(MIN, MAX, A) returns A if in range, otherwise MIN or MAX.
 *
 * If A is greater than MAX, returns MAX, if A is smaller than MIN,
 * returns A, otherwise, i.e. if A is in range, returns A.
 *
 * This function cannot be used in assembler, because the GNU assembler
 * does not implement the ?: operator.  That's why it is here.
 */
#define HW_CROP(_MIN,_MAX,_VAL) \
        ((_VAL) < (_MIN) ? (_MIN) : ((_VAL) > (_MAX) ? (_MAX) : (_VAL)))


/**
 * Uprounding LOG2 function, i.e., counts the number of bits of an integer.
 *
 * Must be fully in macros to be computable at compile time (I prefer macros
 * over template macro programming).  The value *must* be available at boot
 * time (as a constant in the .text section) even before the global ctors
 * have been called, so there's no way to use real C.  We need them to initialise
 * the memory controller, without which we cannot do anything.
 *
 * This function cannot be used in assembler due to the ?: operator.
 */
#define HW_INTEGER_LENGTH(_VAL)    ((_VAL) & 0xffff0000 ?                      \
                                       HW_INTEGER_LENGTH_16((_VAL) >> 16) + 16 \
                                   :   HW_INTEGER_LENGTH_16(_VAL)              \
                                   )

#define HW_INTEGER_LENGTH_16(_VAL) ((_VAL) & 0xff00 ?                       \
                                       HW_INTEGER_LENGTH_8((_VAL) >> 8) + 8 \
                                   :   HW_INTEGER_LENGTH_8(_VAL)            \
                                   )

#define HW_INTEGER_LENGTH_8(_VAL)  ((_VAL) & 0xf0 ?                         \
                                       HW_INTEGER_LENGTH_4((_VAL) >> 4) + 4 \
                                   :   HW_INTEGER_LENGTH_4(_VAL)            \
                                   )

#define HW_INTEGER_LENGTH_4(_VAL)  ((_VAL) & 0xc ?                          \
                                       HW_INTEGER_LENGTH_2((_VAL) >> 2) + 2 \
                                   :   HW_INTEGER_LENGTH_2(_VAL)            \
                                   )

#define HW_INTEGER_LENGTH_2(_VAL)  ((_VAL) & 0x2 ?                          \
                                       HW_INTEGER_LENGTH_1((_VAL) >> 1) + 1 \
                                   :   HW_INTEGER_LENGTH_1(_VAL)            \
                                   )

#define HW_INTEGER_LENGTH_1(_VAL)  ((_VAL) & 0x1)


#endif /* RODOS_HW_LEON2_BITMAGIC_H */

