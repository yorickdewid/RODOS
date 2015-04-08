OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(hello_imx)

SECTIONS
{
	. = 0x1FFF0000;
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
