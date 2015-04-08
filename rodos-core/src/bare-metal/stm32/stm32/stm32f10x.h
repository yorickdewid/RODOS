/*
 * Copyright (c) 2010 Fraunhofer FIRST, Berlin
 * All rights reserved.
 *
 * @author Bernd Oestmann
 */
#ifndef __STM32F10X_H__
#define __STM32F10X_H__

// OBSOLETE: device density is defined in build-script as compilerflag 
// #define	STM32F10X_LD // STM32F10x Low Density with max. 32k flash
// #define	STM32F10X_MD // STM32F10x Medium Density with max. 128k flash -> Olimex STM-P103
// #define	STM32F10X_HD // STM32F10x High Density with max. 512k flash -> multisensorboard

#define	 USE_STDPERIPH_DRIVER

// frequency of quartz crystal connected to stm32
#ifdef STM32F10X_CL
	#define	 HSE_VALUE	((uint32_t)(25 * 1000 * 1000))
#else
	#define	 HSE_VALUE	((uint32_t)(8 * 1000 * 1000))
#endif

#include "stm32f10x_ORIG.h"

#endif /* __STM32F10X_H__ */
