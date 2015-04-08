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

#ifndef HW_FAKE_ENDIAN_H
#define HW_FAKE_ENDIAN_H

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412

#define __BYTE_ORDER    __BIG_ENDIAN
#define __WORDSIZE      32

#endif /* HW_FAKE_STDINT_H */
