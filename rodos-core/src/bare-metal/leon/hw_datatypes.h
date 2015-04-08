/* -*- Mode: C++ -*- */

/**
* @file hw_datatypes.h
* @date 2008/06/19 8:46
* @author Lutz Dittrich,
*         adapted in 2010 by Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief typedefs for hw specific data types
*
* This also contains stuff you would expect in stdlib.
*/

#ifndef RODOS_HW_DATATYPES_H
#define RODOS_HW_DATATYPES_H

#include <stddef.h>

typedef int            __type_int32__;
typedef unsigned int   __type_uint32__;
typedef unsigned short __type_uint16__;

/** typedefs for fixed width data types */

typedef __type_uint16__ TUDPPortNr;
typedef __type_uint32__ size_t;
typedef __type_uint32__ uintptr_t;
typedef __type_int32__  ptrdiff_t;

#endif /* RODOS_HW_DATATYPES_H */
