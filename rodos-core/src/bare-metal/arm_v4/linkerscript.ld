
/* Sizes of the stacks used by the application. NOTE: you need to adjust */
USR_STACK_SIZE = 2048;
IRQ_STACK_SIZE = 64;
FIQ_STACK_SIZE = 64;
SVC_STACK_SIZE = 256;
ABT_STACK_SIZE = 1024;		/* used for abt- and und-mode */

ENTRY(_startup)

/* LPC2468 memory map */
MEMORY
{
	flash  (rx):  ORIGIN = 0          , LENGTH = 512k
	ram    (!r):  ORIGIN = 0x40000000 , LENGTH =  64k
}

SECTIONS
{
	. = 0;
	
	.text :
	{
		*(.startup)
		*(.text)
		*(.text.*)
		*(.eh_frame*)
	} > flash

	.ctors :
	{
		. = ALIGN (4);
		__start_ctors = . ;
		*(SORT_BY_NAME(.ctors))
		*(SORT_BY_NAME(.init_array))
		__end_ctors = . ;
	} > flash
	_etext = . ;
	
	__exidx_start = .;
	__exidx_end = .;
	
	.data :
	{
 		. = ALIGN(4);
		_data = .;
		*(.rodata*)
		*(.data)
		*(.data.*)
		_edata = .;
	} >ram AT >flash
	
	.bss :
	{
		_bss_start = .;
		*(.bss)
		*(.bss.*)
	} >ram
	. = ALIGN(4);
	_bss_end = .;
	
	.stack :
	{
		. = ALIGN (4);
		__stack_start = . ;
		
		. += USR_STACK_SIZE;
		. = ALIGN (4);
		__usr_stack_top = . ;
		
		. += FIQ_STACK_SIZE;
		. = ALIGN (4);
		__fiq_stack_top = . ;
		
		. += IRQ_STACK_SIZE;
		. = ALIGN (4);
		__irq_stack_top = . ;
		
		. += SVC_STACK_SIZE;
		. = ALIGN (4);
		__svc_stack_top = . ;
		
		. += ABT_STACK_SIZE;
		. = ALIGN (4);
		__abt_stack_top = . ;
		
		__stack_end = .;
	} > ram
	. = ALIGN(4);
}
