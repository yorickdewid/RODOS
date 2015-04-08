OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc)

/* Define Memories in the system */
/*
MEMORY
{
   xps_bram_if_cntlr_1 : ORIGIN = 0xFFFE0000, LENGTH = 0x00020000
   shared_bram_if_cntlr_0_top : ORIGIN = 0x81818000, LENGTH = 0x00002000
   mpmc_0_MPMC_BASEADDR : ORIGIN = 0x00000000, LENGTH = 0x07FFFFEC
   ppc405_0_iocm_cntlr : ORIGIN = 0xC23F8000, LENGTH = 0x00002000
   ppc405_0_docm_cntlr : ORIGIN = 0x44C00000, LENGTH = 0x00002000
}
*/

/* Define the sections, and where they are mapped in memory */

SECTIONS {
	.bootload0 0xFFFFFE00 : {
   		__bootload0_start = .;
   		*(.bootload0)
   		__bootload0_end = .;
	} 

	.bootload 0xFFFFFFFC : {
   		__bootload_start = .;
   		*(.bootload)
  	__bootload_end = .;
	} 
}

