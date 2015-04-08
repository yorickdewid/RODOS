#include "lpc24xx_registers.h"
#include "debug.h"

#include "hw_debug_uart.h"
namespace RODOS {
extern "C" {
int puts(const char* s) {
	if (s == 0) {
		return 0;
	}
	while (*s != 0) {
		putchar(*s);
		s++;
	}
	putchar('\n');
	return 0;
}

void flush() {
	DebugUart::flush();
}

int putchar(const int c) {
	DebugUart::writechar(c);
	return c;
}

/*void ui2hex(char* buf, unsigned int i) {
	static const char* HEXCHARS = "0123456789ABCDEF";
	for (int j = 0; j < 8; ++j) {
		buf[j] = HEXCHARS[((i >> (28 - 4 * j)) & 0xF)];
	}
}

void debug_ui(unsigned int i) {
	for (int j = 0; j < 8; ++j) {
		int k = ((i >> (28 - 4 * j)) & 0xF);
		if (k > 9) {
			putchar('7' + k);
		} else {
			putchar('0' + k);
		}
	}
}

void debug_char(unsigned char i) {
	for (int j = 0; j < 2; ++j) {
		int k = ((i >> (4 - 4 * j)) & 0xF);
		if (k > 9) {
			putchar('7' + k);
		} else {
			putchar('0' + k);
		}
	}
}*/
}

void FFLUSH() {
}
}
