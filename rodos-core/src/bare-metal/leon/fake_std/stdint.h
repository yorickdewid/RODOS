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

#ifndef HW_FAKE_STDINT_H
#define HW_FAKE_STDINT_H

#include <endian.h>

typedef signed   char      int8_t;
typedef unsigned char      uint8_t;

typedef signed   short     int16_t;
typedef unsigned short     uint16_t;

typedef signed   int       int32_t;
typedef unsigned int       uint32_t;

typedef signed   long long int64_t;
typedef unsigned long long uint64_t;

#endif /* HW_FAKE_STDINT_H */
