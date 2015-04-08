/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "../../bare-metal-generic/fatfs/diskio.h"		/* FatFs lower layer API */
#include "emmc.h"		/* MMC/SDC contorl */

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber (0..) */
) {
	int32_t res = emmc_init();

	if (res != -1)
		return RES_OK;
	else
		return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber (0..) */
) {
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE *buff, /* Data buffer to store read data */
DWORD sector, /* Sector address (LBA) */
BYTE count /* Number of sectors to read (1..128) */
) {
	int32_t res = emmc_read(buff, count * 512, sector);

	if (res != -1)
		return RES_OK;
	else
		return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, /* Physical drive nmuber (0..) */
const BYTE *buff, /* Data to be written */
DWORD sector, /* Sector address (LBA) */
BYTE count /* Number of sectors to write (1..128) */
) {
	int32_t res = emmc_write(buff, count * 512, sector);

	if (res != -1)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {

	switch (cmd) {
	case CTRL_SYNC:
		return RES_OK;
	case GET_SECTOR_SIZE: // Get R/W page size (DWORD)
		*(DWORD*) buff = 512;
		return RES_OK;
	default:
		break;
	}
	return RES_PARERR;
}
#endif

#ifndef NO_RODOS_NAMESPACE
}
#endif

