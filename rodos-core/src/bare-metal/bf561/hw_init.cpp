/***********************************************************************************************//**
 * \ingroup rodos
 * \file hw_init.cpp
 * \version 0.1
 * \date 18.02.2014
 * \author Georg Raith
 *
 * \brief 
 * \b History: \n
 *
 * 18.02.2014 rg: initial version
 */


#include "hw_config.h"
#include "board_config.h"
#include "hw_stdio_serial.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


volatile u32 core_syscfg;
volatile u32 core_hwerrcause;
volatile u32 core_excause;
volatile void* core_hwerror_address;
volatile void* core_exception_address;
volatile bool core_sftreset;

static void hw_fatal_error() {
	system_enter_critical_region(NULL); // never exit the region!
	xprintf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	xprintf("F a t a l   E R R O R\n");
	xprintf("Core Syscfg:\t\t0x%lx\n", core_syscfg);
	xprintf("Core HW Excause:\t0x%lx\n",core_hwerrcause);
	xprintf("Core Excause:\t\t0x%lx\n", core_excause);
	xprintf("HW Error Address:\t0x%08lx\n", (unsigned long) core_hwerror_address);
	xprintf("Core Exception Address:\t0x%08lx\n", (unsigned long) core_exception_address);
	while (1)
		__asm__("idle");
}

#if CACHE_ENABLE == 1
struct mm_table_entry {
	u32 start;
	u32 end;
	u32 mode;
};

mm_table_entry mm_data_table[] = {
		{0x00800000, 0x02000000, (PAGE_SIZE_4MB | CPLB_L1_AOW | CPLB_L1_CHBL | CPLB_DIRTY | CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)},//sdram
		{0x00200000, 0x04000000, (PAGE_SIZE_4MB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID) },//sdram no cache
		{0x28000000, 0x2c000000, (PAGE_SIZE_4MB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID) },//fpga
		{0x20000000, 0x24000000, (PAGE_SIZE_4MB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID) },//flash rom
		{0x24000000, 0x24200000, (PAGE_SIZE_1MB | CPLB_L1_AOW | CPLB_L1_CHBL | CPLB_DIRTY | CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID)}, //sram
		{0x2C000000, 0x2c000100, (PAGE_SIZE_1KB |CPLB_USER_WR | CPLB_SUPV_WR | CPLB_USER_RD | CPLB_VALID) },//Lan Chip
};

mm_table_entry mm_instruction_table[] = {
		{0, 0,(PAGE_SIZE_1KB | CPLB_L1_CHBL |CPLB_VALID) }
#warning no valid table entries
};
#endif //#if CACHE_ENABLE == 1

__attribute__((section(".l1code.text"))) void dcplb_update() {
#if CACHE_ENABLE == 1
	static u8 replacement_counter=0;
	static u32 cdplb_miss=0;

	u8 i;
	s8 table_entry = -1;

	u32 fault_address;

	u32 address_mask;

	u32 *pDCPLB_DATAx;
	u32 *pDCPLB_ADDRx;


	cdplb_miss++;
	fault_address = (u32) *pDCPLB_FAULT_ADDR;

	for (i=0; i<sizeof(mm_data_table)/sizeof(mm_table_entry); i++) {
		if( ( mm_data_table[i].start <= fault_address) && (mm_data_table[i].end > fault_address) ) {
			table_entry = i; //table entry found
			break;
		}
	}

	if (table_entry == -1) {
		hw_fatal_error();
//		while (1)
//			__asm__("idle");
#warning find out how to hande default values;
	}

	//find the next dcplb entry for replacement for replacement of the Data
	replacement_counter = (replacement_counter+1)%(16 - MFC_DCPLB_MIN) + MFC_DCPLB_MIN;
	pDCPLB_DATAx = (u32*)DCPLB_DATA0;
	pDCPLB_ADDRx = (u32*)DCPLB_ADDR0;
	pDCPLB_DATAx += replacement_counter;
	pDCPLB_ADDRx += replacement_counter;

	//find out Page Size
	switch((mm_data_table[table_entry].mode &  PAGE_SIZE_4MB) >> 16) {
	case 0:
		address_mask = 0xfffffc00; //1k
		break;
	case 1:
		address_mask = 0xfffff000; //4k
		break;
	case 2:
		address_mask = 0xfff00000; //1M
		break;
	case 3:
		address_mask = 0xffC00000; //4M
		break;
	default:
		//could never occur
		break;
	}

	//maybe disabling should move a little bit up
	ssync();
	*pDMEM_CONTROL &= (~ENDCPLB); //Disable CPLB
	ssync();
	*pDCPLB_ADDRx = ( fault_address & address_mask); //alignment of address mask
	*pDCPLB_DATAx = mm_data_table[i].mode;
	//set values
	ssync();
	*pDMEM_CONTROL |= ENDCPLB; //reenable CPLB
	ssync();
#endif //#if CACHE_ENABLE == 1
}


__attribute__((section(".l1code.text"))) void icplb_update() {
#if CACHE_ENABLE == 1
	static u8 replacement_counter=0;
	static u32 icplb_miss=0;

	u8 i;
	s8 table_entry = -1;

	u32 fault_address;

	u32 address_mask;

	u32 *pICPLB_DATAx;
	u32 *pICPLB_ADDRx;


	icplb_miss++;
	fault_address = (u32) *pICPLB_FAULT_ADDR;

	for (i=0; i<sizeof(mm_instruction_table)/sizeof(mm_instruction_table); i++) {
		if( ( mm_instruction_table[i].start <= fault_address) && (mm_instruction_table[i].end > fault_address) ) {
			table_entry = i; //table entry found
			break;
		}
	}

	if (table_entry == -1) {
		hw_fatal_error();
#warning find out how to hande default values;
	}

	//find the next dcplb entry for replacement for replacement of the Data
	replacement_counter = (replacement_counter+1)%(16 - MFC_ICPLB_MIN) + MFC_ICPLB_MIN;
	pICPLB_DATAx = (u32*)ICPLB_DATA0;
	pICPLB_ADDRx = (u32*)ICPLB_ADDR0;
	pICPLB_DATAx += replacement_counter;
	pICPLB_ADDRx += replacement_counter;

	//find out Page Size
	switch((mm_instruction_table[table_entry].mode &  PAGE_SIZE_4MB) >> 16) {
	case 0:
		address_mask = 0xfffffc00; //1k
		break;
	case 1:
		address_mask = 0xfffff000; //4k
		break;
	case 2:
		address_mask = 0xfff00000; //1M
		break;
	case 3:
		address_mask = 0xffC00000; //4M
		break;
	default:
		//could never occur
		break;
	}

	//maybe disabling should move a little bit up
	ssync();
	*pIMEM_CONTROL &= (~ENICPLB); //Disable CPLB
	ssync();
	*pICPLB_ADDRx = ( fault_address & address_mask); //alignment of address mask
	*pICPLB_DATAx = mm_instruction_table[i].mode;
	//set values
	ssync();
	*pIMEM_CONTROL |= ENICPLB; //reenable CPLB
	ssync();
#endif //if CACHE_ENABLE == 1
}

/***********************************************************************************************//**
 * \brief Flush the cache
 *
 * This function flush the cache. The dirty lines will be stored into the external memory.
 *
 * \param startAddress - Start address of the area.
 * \param stopAddress - Stop address of the area.
 */
__attribute__((section(".l1code.text"))) void mfc_FlushCacheArea(void* startAddress, void* stopAddress)
{
#if CACHE_ENABLE == 1
	startAddress = (void *) (((u32) startAddress) & (~31));
	while (startAddress < stopAddress)
		FLUSH(startAddress);
	ssync();
#endif
}

/***********************************************************************************************//**
 * \brief Flush and invalidate the cache
 *
 * This function flush and invalidate the cache. The dirty lines will be stored into the
 * external memory. Line marked as invalid.
 *
 * \param startAddress - Start address of the area.
 * \param stopAddress - Stop address of the area.
 */
__attribute__((section(".l1code.text"))) void mfc_FlushInvalidateCacheArea(void* startAddress, void* stopAddress)
{
#if CACHE_ENABLE == 1
	startAddress = (void *) (((u32) startAddress) & (~31));
	while (startAddress < stopAddress)
		FLUSHINV(startAddress);
	ssync();
#endif
}



/**
 * \brief Enable core B
 */

void coreb_enable ()
{
	*pSICA_SYSCR &= 0xffdf;
}

/**
 * \brief
 * sdram initialization
 * configuration in system_config.h
 */
void sdram_init() {
	*pEBIU_SDBCTL = MFC_EBIU_SDBCTL;
	*pEBIU_SDRRC = MFC_EBIU_SDRRC;
	*pEBIU_SDGCTL = MFC_EBIU_SDGCTL;
}

/**
 * \brief
 * Async memory bank initialization
 * configuration in system_config.h
 */
void amb_init() {

	*pEBIU_AMGCTL = MFC_EBIU_AM_CTL;
	*pEBIU_AMBCTL0 = MFC_EBIU_AMCTL0;
	*pEBIU_AMBCTL1 = MFC_EBIU_AMCTL1;
}


u32 system_mmu_init() {
#if CACHE_ENABLE == 1
	*pIMEM_CONTROL = LRUPRIORST; //Disable Instruction Cache for configuration
	ssync();




	*pICPLB_ADDR0 = MFC_ICPLB_ADDR0;
	*pICPLB_DATA0 = MFC_ICPLB_DATA0;
	*pICPLB_ADDR1 = MFC_ICPLB_ADDR1;
	*pICPLB_DATA1 = MFC_ICPLB_DATA1;
	*pICPLB_ADDR2 = MFC_ICPLB_ADDR2;
	*pICPLB_DATA2 = MFC_ICPLB_DATA2;
	*pICPLB_ADDR3 = MFC_ICPLB_ADDR3;
	*pICPLB_DATA3 = MFC_ICPLB_DATA3;
	*pICPLB_ADDR4 = MFC_ICPLB_ADDR4;
	*pICPLB_DATA4 = MFC_ICPLB_DATA4;
	*pICPLB_ADDR5 = MFC_ICPLB_ADDR5;
	*pICPLB_DATA5 = MFC_ICPLB_DATA5;
	*pICPLB_ADDR6 = MFC_ICPLB_ADDR6;
	*pICPLB_DATA6 = MFC_ICPLB_DATA6;
	*pICPLB_ADDR7 = MFC_ICPLB_ADDR7;
	*pICPLB_DATA7 = MFC_ICPLB_DATA7;
	*pICPLB_ADDR8 = MFC_ICPLB_ADDR8;
	*pICPLB_DATA8 = MFC_ICPLB_DATA8;
	*pICPLB_ADDR9 = MFC_ICPLB_ADDR9;
	*pICPLB_DATA9 = MFC_ICPLB_DATA9;
	*pICPLB_ADDR10= MFC_ICPLB_ADDR10;
	*pICPLB_DATA10= MFC_ICPLB_DATA10;
	*pICPLB_ADDR11= MFC_ICPLB_ADDR11;
	*pICPLB_DATA11= MFC_ICPLB_DATA11;
	*pICPLB_ADDR12= MFC_ICPLB_ADDR12;
	*pICPLB_DATA12= MFC_ICPLB_DATA12;
	*pICPLB_ADDR13= MFC_ICPLB_ADDR13;
	*pICPLB_DATA13= MFC_ICPLB_DATA13;
	*pICPLB_ADDR14= MFC_ICPLB_ADDR14;
	*pICPLB_DATA14= MFC_ICPLB_DATA14;
	*pICPLB_ADDR15= MFC_ICPLB_ADDR15;
	*pICPLB_DATA15= MFC_ICPLB_DATA15;


	ssync();
	*pIMEM_CONTROL = MFC_IMEM_CONTROL;
	ssync();

	*pDMEM_CONTROL = 0;
	ssync();
	*pDCPLB_ADDR0 = MFC_DCPLB_ADDR0;
	*pDCPLB_DATA0 = MFC_DCPLB_DATA0;
	*pDCPLB_ADDR1 = MFC_DCPLB_ADDR1;
	*pDCPLB_DATA1 = MFC_DCPLB_DATA1;
	*pDCPLB_ADDR2 = MFC_DCPLB_ADDR2;
	*pDCPLB_DATA2 = MFC_DCPLB_DATA2;
	*pDCPLB_ADDR3 = MFC_DCPLB_ADDR3;
	*pDCPLB_DATA3 = MFC_DCPLB_DATA3;
	*pDCPLB_ADDR4 = MFC_DCPLB_ADDR4;
	*pDCPLB_DATA4 = MFC_DCPLB_DATA4;
	*pDCPLB_ADDR5 = MFC_DCPLB_ADDR5;
	*pDCPLB_DATA5 = MFC_DCPLB_DATA5;
	*pDCPLB_ADDR6 = MFC_DCPLB_ADDR6;
	*pDCPLB_DATA6 = MFC_DCPLB_DATA6;
	*pDCPLB_ADDR7 = MFC_DCPLB_ADDR7;
	*pDCPLB_DATA7 = MFC_DCPLB_DATA7;
	*pDCPLB_ADDR8 = MFC_DCPLB_ADDR8;
	*pDCPLB_DATA8 = MFC_DCPLB_DATA8;
	*pDCPLB_ADDR9 = MFC_DCPLB_ADDR9;
	*pDCPLB_DATA9 = MFC_DCPLB_DATA9;
	*pDCPLB_ADDR10= MFC_DCPLB_ADDR10;
	*pDCPLB_DATA10= MFC_DCPLB_DATA10;
	*pDCPLB_ADDR11= MFC_DCPLB_ADDR11;
	*pDCPLB_DATA11= MFC_DCPLB_DATA11;
	*pDCPLB_ADDR12= MFC_DCPLB_ADDR12;
	*pDCPLB_DATA12= MFC_DCPLB_DATA12;
	*pDCPLB_ADDR13= MFC_DCPLB_ADDR13;
	*pDCPLB_DATA13= MFC_DCPLB_DATA13;
	*pDCPLB_ADDR14= MFC_DCPLB_ADDR14;
	*pDCPLB_DATA14= MFC_DCPLB_DATA14;
	*pDCPLB_ADDR15= MFC_DCPLB_ADDR15;
	*pDCPLB_DATA15= MFC_DCPLB_DATA15;

	ssync();
	*pDMEM_CONTROL = MFC_DMEM_CONTROL;
	ssync();
#endif
	return 0;
}

void system_decode_sysstate () {
	u32 syscfg;
	u32 seqstat;
	asm ("%0 = SEQSTAT;"
			: "=r" (seqstat)
			: /* no inputs */  );
	asm ("%0 = SYSCFG;"
			: "=r" (syscfg)
			: /* no inputs */ );

	core_syscfg = syscfg;
	core_hwerrcause = (seqstat >> 14) & 0x1f;
	core_excause = (seqstat & 0x3f);
	core_sftreset = ((seqstat >> 13) & 0x1) == 1;
	asm ("%0 = RETI;"
			: "=r" (core_hwerror_address)
			: /* no inputs */ );
	asm ("%0 = RETX;"
			: "=r" (core_exception_address)
			: /* no inputs */ );

}

__attribute__((exception_handler)) __attribute__((section(".l1code.text"))) void core_hardware_interrupt(){
	ssync();

	system_decode_sysstate();
	while(1) {
		asm ("idle;");
	}
}

__attribute__((exception_handler)) __attribute__((section(".l1code.text")))void core_exception_interrupt(){
	u32 seqstat;
	asm ("%0 = SEQSTAT;"
			: "=r" (seqstat)
			: /* no inputs */  );

	if((seqstat & SEQSTAT_EXCAUSE) == 0x26) { //Data Cplb miss
		dcplb_update();
		return;
	}

	if((seqstat & SEQSTAT_EXCAUSE) == 0x2c) { //Data Cplb miss
		icplb_update();
		return;
	}

	system_decode_sysstate();
//	while(1) {
//		asm ("idle;");
//	}
	hw_fatal_error();
}

/**
 * \brief Raise interrupt 14 to stay in supervisor mode after Returning to the idle thread
 *
 * \return error code
 */
void getHighPrio() {
	void * interrupt_vector;//only a buffer


	interrupt_vector = *pEVT14;
	*pEVT14 = &&interrupt_raised;
	__asm__("RAISE 14");
	//while(1) ; //interrupt 14 hast to be raised
interrupt_raised:
	*pEVT14 = interrupt_vector;
	return;

}




/**
 *  \brief System initialization
 *  It should be called for each core
 *  Each init function should be implemented here
 */
void hwInit() {
///this routines are only necessary for corea
#ifdef __BFIN_COREA
	*pIMEM_CONTROL = LRUPRIORST; //Disable Instruction Cache for configuration
	ssync();
	*pIMEM_CONTROL = 1;	// reset L1 Instruction Memory control register
	ssync();
	*pDMEM_CONTROL = 9; // reset L1 Data memory control register
	ssync();

	// disable interrupts during setup using the 'clear register'
	// @todo besseren Platz finden!
	*pFIO0_MASKA_C = 0xffff;
	*pFIO0_MASKB_C = 0xffff;
	*pFIO0_DIR = 0;
	*pFIO0_EDGE = 0;
	*pFIO0_EDGE = 0;
	*pFIO0_POLAR = 0;
	*pFIO0_INEN = 0;

	coreb_enable();

	HW_InterruptInit();
	UART_inititialize(RODOS_UART_STDIO_SPEED_DIVIDER);

	// clear hardware exception just in case and hook exception handler
	*pILAT = 1<<3;
	*pEVT3 = (void (*)) (&core_exception_interrupt);


	// finally clear hardware hardware error and hook interrupt handler
	*pILAT = 1<<5;
	*pEVT5 = (void (*)) (& core_hardware_interrupt);
	*pIMASK |= 1<<5;

	system_mmu_init();
	hw_dma_init();
	system_decode_sysstate();
	// calibrate idle loop

#endif
///this routines are necessary for coreb
#ifdef __BFIN_COREB
#endif

	// call board init if exists

	board_init();
	getHighPrio();
}


#ifndef NO_RODOS_NAMESPACE
}
#endif


