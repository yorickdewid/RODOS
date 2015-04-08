
#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_

#include <stdlib.h>
#include "hw_specific.h"
#include "hw_predefines.h"
#include "params.h"

#include <cdefBF561.h>
#include <config/system_config.h>
//#include <rodos.h> ???


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
#ifndef SCLK
#define SCLK 75000000
#warning SCLK not defined!
#endif

///context size in 4 byte
#define BFIN_CONTEXT_SIZE 45

///Baud Rate  Divisor = (int)(SCLK/(16 * Baud Rate) + 0.5)
///Baud Rate =  114329.2682926829 (SCLK = 75MHz)
#define RODOS_UART_STDIO_SPEED  115200
#define RODOS_UART_STDIO_SPEED_DIVIDER (int) (SCLK/(16 * RODOS_UART_STDIO_SPEED) + 0.5)


/// define how much jaregs are active in this design
// td #define MFC_JAREG_COUNT	2

/// Offset in the FPGA for the Jaregs
// td #define MFC_JAREG_BASE_OFFSETS 0x00,0x08

/// Programmable flags for the Jaregs
// td #define MFC_JAREG_PFS 6,7

/// Channels in use on the Jaregs
// td #define MFC_JAREG_CHANNELS_USED 3

/// Interrupt Vector Group definition of the level activated programmable flags
#define HW_FLAG_LEVEL_IVG 11

/// define how many Programmable flags are edge detecting
// td #define HW_FLAG_EDGE_COUNT 2
/// define which flags are used for timing
// td #define MFC_TIMING_PFS 4,5
/// Interrupt Vector Group definitions for the routines
#define HW_FLAG_EDGE_IVG 10

/// define standard interrupt vector group to hook interrupt servers
#define HW_STD_IVG	13



enum isr_callback_event {
	/// the hook function is calling the callback function using the callback_enabled event
	/// to allow the application to prepare for servicing interrupts.
	ISR_CALLBACK_ENABLED,
	/// the unhook function is calling the callback function using the callback_disabled event
	/// to allow the application to end servicing interrupts
	ISR_CALLBACK_DISABLED,
	/// the service_request event is given with a real interrupt
	ISR_CALLBACK_SERVICE_REQUEST,
	/// the dma service processed all transfer jobs
	ISR_CALLBACK_DMA_DONE,
	/// the dma service ended with an error while processing the transfer jobs, the remaining jobs are cancelled
	ISR_CALLBACK_DMA_ERROR,
	/// hardware error of a device
	ISR_CALLBACK_DRIVER_HW_ERROR,
};


/**
 * standard function-pointer to callbacks to the interrupt system
 * @todo: remove return for future use!
 * @return 0 for interrupt serviced, others for not serviced or errors, not used actually!
 * @param arg pointer to a application defined argument
 * @param event reason of calling the application function
 */
typedef u32 (isr_callback)(isr_callback_event, void* arg);
typedef void (isr_callback_no_arg)();

extern "C" {

/**
*
*/
extern void __asmSaveContextAndCallScheduler_isr();

} // extern "C"


#define CACHE_ENABLE 1
#if CACHE_ENABLE != 1
#warning Caching DISABLED!
#else

#define MFC_DCPLB_MIN 4
#define MFC_ICPLB_MIN 4

#define MFC_IMEM_CONTROL (IMC | ENICPLB)
#define MFC_DMEM_CONTROL (PORT_PREF0 | ACACHE_BCACHE | ENDCPLB)

#define MFC_ICPLB_ADDR_UNUSED ((void *) 0x0)
#define MFC_DLCPB_ADDR_UNUSED ((void *) 0x0)
#define MFC_ICPLB_DATA_UNUSED 0
#define MFC_DCPLB_DATA_UNUSED 0

#define MFC_ICPLB_ADDR0		MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA0	    MFC_ICPLB_DATA_UNUSED



#define MFC_ICPLB_ADDR1      (void *) 0xffa00000 //L1 Instruction RAM
#define MFC_ICPLB_DATA1	     (PAGE_SIZE_1MB |CPLB_VALID)


#define MFC_ICPLB_ADDR2      (void *)0xfeb00000	//L2 Ram
#define MFC_ICPLB_DATA2	     (PAGE_SIZE_1MB | CPLB_L1_CHBL |CPLB_VALID)

#define MFC_ICPLB_ADDR3      (void *) 0x00000000
#define MFC_ICPLB_DATA3	     (PAGE_SIZE_4MB | CPLB_L1_CHBL |CPLB_VALID)

#define MFC_ICPLB_ADDR4      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA4	     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR5      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA5	     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR6      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA6      MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR7      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA7	     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR8      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA8	     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR9      MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA9	     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR10     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA10     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR11     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA11     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR12     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA12     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR13     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA13     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR14     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA14     MFC_ICPLB_DATA_UNUSED

#define MFC_ICPLB_ADDR15     MFC_ICPLB_ADDR_UNUSED
#define MFC_ICPLB_DATA15     MFC_ICPLB_DATA_UNUSED







#define MFC_DCPLB_ADDR0      ((void *) 0x0)
#define MFC_DCPLB_DATA0	     (PAGE_SIZE_4MB | CPLB_L1_AOW | CPLB_L1_CHBL | CPLB_DIRTY | CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)

#define MFC_DCPLB_ADDR1      (void *)0x000400000;
#define MFC_DCPLB_DATA1	     (PAGE_SIZE_4MB | CPLB_L1_AOW | CPLB_L1_CHBL | CPLB_DIRTY | CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)

#define MFC_DCPLB_ADDR2      (void *)0xff800000
#define MFC_DCPLB_DATA2	     (PAGE_SIZE_4MB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)

#define MFC_DCPLB_ADDR3      (void *)0xfeb00000
#define MFC_DCPLB_DATA3	     (PAGE_SIZE_4MB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)

#define MFC_DCPLB_ADDR4      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA4	     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR5      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA5	     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR6      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA6      MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR7      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA7	     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR8      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA8	     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR9      MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA9	     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR10     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA10     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR11     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA11     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR12     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA12     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR13     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA13     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR14     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA14     MFC_DCPLB_DATA_UNUSED

#define MFC_DCPLB_ADDR15     MFC_DLCPB_ADDR_UNUSED
#define MFC_DCPLB_DATA15     MFC_DCPLB_DATA_UNUSED


/// Macros for flush the cache line
#define FLUSH(P)  {asm volatile("FLUSH[%0++];":"+p"(P));}
/// Macros for flush and invalidate the cache line
#define FLUSHINV(P)  {asm volatile("FLUSHINV[%0++];":"+p"(P));}

#endif	// caching enabled

/***********************************************************************************************//**
 * \brief Flush the cache
 *
 * This function flush the cache
 *
 * \param startAddress - Start address of the area.
 * \param stopAddress - Stop address of the area.
 */
void mfc_FlushCacheArea(void* startAddress, void* stopAddress);

/***********************************************************************************************//**
 * \brief Flush and invalidate the cache
 *
 * This function flush and invalidate the cache
 *
 * \param startAddress - Start address of the area.
 * \param stopAddress - Stop address of the area.
 */
void mfc_FlushInvalidateCacheArea(void* startAddress, void* stopAddress);


#ifndef NO_RODOS_NAMESPACE
}
#endif

#include "hw_interrupt.h"
#include "board_config.h"
#include "hw_dma.h"
#include "hw_DMAJob.h"
#include "protectedFifo.h"

#endif

