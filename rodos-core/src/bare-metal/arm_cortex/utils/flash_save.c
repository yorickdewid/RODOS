#include "flash_save.h"
#include <string.h>
#include "../hw/hw_types.h"
#include "../hw/hw_memmap.h"
#include "../hw/hw_ints.h"
#include "../src/lm3s_flash.h"
//*****************************************************************************
// Variables and functions need for save variables in the flash memory
//*****************************************************************************

//*****************************************************************************
//
//	Declare a 1 kB block in the flash aligned on 1024 bytes boundary.
//
//*****************************************************************************
const unsigned char flashBlock[1024]__attribute__ ((aligned(1024))) = { 0x00, 0x00, 0x00, 0x00 };

//*****************************************************************************
//
// !This function writes the variables to the flash.
// ! \param permanentVars keep data write to flash
// ! \return none
//
//*****************************************************************************
void writePermanentsToFlash(PERMANENT_VAR permanentVars)
{
	//
	// Set the uSec value to 50, indicating that the processor is running at
	// 50 MHz.
	//
	FlashUsecSet(50);
	//
	// Erase a block of the flash.
	//
	FlashErase((unsigned long) flashBlock);
    //The length must be a multiple of 4
    FlashProgram((unsigned long*) &permanentVars, (unsigned long) flashBlock, (sizeof(permanentVars) + 3) & ~3);
}

//*****************************************************************************
//
// !This function reads the variables from the flash.
// ! \param permanentVars keep data read from flash
// ! \return none
//
//*****************************************************************************
void readPermanentsFromFlash(PERMANENT_VAR *permanentVars)
{
    memcpy(permanentVars, flashBlock, sizeof(*permanentVars));
}
