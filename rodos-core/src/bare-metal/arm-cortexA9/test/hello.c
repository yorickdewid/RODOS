//#include "registers/regsuart.h"

#include "console_interface.h"

extern unsigned long __bss_start__, __bss_end__;

void clean_bss(){
	unsigned long *cp = (unsigned long*)&__bss_start__;
	while (cp < (unsigned long*)&__bss_end__) {
		*cp++ = 0;
	}
}

//static hw_module_t uart_port = {
//    "UART2 for test",
//    2,
//    UART2_BASE_ADDR,
//    27000000,
//    IMX_INT_UART2,
//    NULL,
//    NULL
//};

int hello_world (int argc, char * const argv[]) {
	/* clear .bss */
	clean_bss();

	console_putStr("Hello world! How have you been doing?\n\r");
	console_putStr("This is a echo test?\n\r");
	console_putStr("Please type a character - x to exit:\n\r");

	char sel = 0;

	while(sel != 'x'){
		sel = console_readChar();
		if(sel > 32 && sel < 126){
			console_putChar(sel);
		}
	}

	console_putStr("\n\rEnd of echo test!\n\r");

	return 0;
}

