#include "rodos.h"

/*
 * For some reason the linker don't find the "weak" MAIN() from rodos/src/independent/main_usermain.cpp in librodos.a
 * -> so we have to define our own
 */
void MAIN() {
	xprintf("Windows specific user-MAIN.\nNecessary because MinGW linker don't find weak MAIN from rodos/independent\n");
}

