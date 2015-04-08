/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "../../bare-metal-generic/fatfs/diskio.h"		/* FatFs lower layer API */
#include "rodos.h"
#include "stm324xg_eval.h"
#include "stm324xg_eval_sdio_sd.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber (0..) */
) {
	  NVIC_InitTypeDef NVIC_InitStructure;
	  /* Configure the NVIC Preemption Priority Bits */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
	  NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_Init(&NVIC_InitStructure);

	SD_Error res = SD_Init();

	if (res == SD_OK)
		return RES_OK;
	else{
		xprintf("SD Init Error Code: %d\n",res);
		return RES_ERROR;
	}
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber (0..) */
) {
	if(SD_GetState()==SD_CARD_ERROR){
		return RES_ERROR;
	}else{
		return RES_OK;
	}
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE *buff, /* Data buffer to store read data */
DWORD sector, /* Sector address (LBA) */
BYTE count /* Number of sectors to read (1..128) */
) {
	//PRINTF("Reading from sector %d count %d...",sector,count);
	SD_Error res = SD_ReadMultiBlocks(buff,(uint64_t)sector*512LL,512,count);
	if(res == SD_OK){
		res = SD_WaitReadOperation();
	}

	if (res == SD_OK){
		return RES_OK;
	} else {
		xprintf("SD Card: Error Code %d, check enum SD_Error",res);
		return RES_ERROR;
	}
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
	//PRINTF("Writing to sector %d count %d...\n",sector,count);
	SD_Error res = SD_WriteMultiBlocks(buff,(uint64_t)sector*512LL,512,count);
	if(res == SD_OK){
		res = SD_WaitWriteOperation();
	}
	if(res == SD_OK){
		SDTransferState ts;
		 int64_t timeout = NOW() + SD_DATATIMEOUT_MS*MILLISECONDS;
		while( (ts = SD_GetStatus()) != SD_TRANSFER_OK){
			 if(NOW() > timeout){
			   res = SD_DATA_TIMEOUT;
			   break;
		    }
			Thread::yield();
			if(ts == SD_TRANSFER_ERROR){
				res = SD_ERROR;
			}
		}
	}

	if (res == SD_OK){
		return RES_OK;
	} else {
		xprintf("SD Card: Error Code %d, check enum SD_Error\n",res);
		return RES_ERROR;
	}
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

