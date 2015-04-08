/* -*- Mode: C -*- */

/**
 * @file
 * @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
 *
 * Copyright 2008-2010 DLR
 *
 * @brief Definitions similar to libc for printf library
 *
 * This file is not provided in the gcc distribution from Gaisler.
 */

#ifndef HW_FAKE_LIMITS_H
#define HW_FAKE_LIMITS_H

#include <endian.h>

#define CHAR_BIT    8
                    
#define UCHAR_MIN   0
#define UCHAR_MAX   255

#define CHAR_MAX    127
#define CHAR_MIN    (-CHAR_MAX - 1)

#define SCHAR_MAX   CHAR_MAX
#define SCHAR_MIN   CHAR_MIN

#define USHRT_MAX   65535
#define SHRT_MAX    32767
#define SHRT_MIN    (-SHRT_MAX - 1)

#define UINT_MAX    4294967295U
#define INT_MAX     2147483647
#define INT_MIN     (-INT_MAX - 1)

#define ULONG_MAX   4294967295UL
#define LONG_MAX    2147483647L
#define LONG_MIN    (-LONG_MAX - 1L)

#define ULLONG_MAX  18446744073709551615ULL
#define LLONG_MAX   9223372036854775807LL
#define LLONG_MIN   (-LLONG_MAX - 1LL)

#endif /* HW_FAKE_STDINT_H */
