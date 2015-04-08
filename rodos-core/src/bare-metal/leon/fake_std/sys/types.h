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

#ifndef HW_FAKE_SYS_TYPES_H
#define HW_FAKE_SYS_TYPES_H

#include "hw_datatypes.h"

#define __THROW
#define __attribute_dontuse__ __attribute__((__deprecated__))
#define __attribute_malloc__  __attribute__((__malloc__))

typedef unsigned int pid_t;

typedef ptrdiff_t ssize_t;

#endif /* HW_FAKE_SYS_TYPES_H */
