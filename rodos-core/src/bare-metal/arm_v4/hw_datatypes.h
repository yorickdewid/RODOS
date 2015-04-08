/**
* @file hw_datatypes.h
* @date 2008/06/19 8:46
* @author Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief typedefs for hw specific data types
*
*/

#ifndef ARMv4__DATATYPES_H
#define ARMv4__DATATYPES_H

namespace RODOS {
typedef unsigned int __type_uint32__;
typedef unsigned short __type_uint16__;

/** typedefs for fixed width data types */

typedef __type_uint16__ TUDPPortNr;
typedef __type_uint32__ size_t;
}
#endif /* ARMv4_DATATYPES_H */
