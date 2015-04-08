/* 
 * File:   includes.h
 * Author: Gabriel Reimers
 *
 * Created on March 27, 2012, 5:16 PM
 */



#ifndef _PATHSETUP_H
#define	_PATHSETUP_H

/*
 * This file is only to make switching hardware modules easier.
 * You only need to change the paths here.
 * This is used in: HammingCode, diskIO and ffconf
 */

//enter the path of the hardware module setup file here
#define FLASHIO_SETUP_FILE "olimex_lpc/setup.h" //this is the path from the diskio level
#define DISKIO_FLASHIO_SETUP_FILE "diskio/olimex_lpc/setup.h" //this is the path from the fat_fs level

//enter the path of the hardware module flashIO file here
#define FLASHIO_FILE "olimex_lpc/flashio.h"

#endif	/* _PATHSETUP_H */

