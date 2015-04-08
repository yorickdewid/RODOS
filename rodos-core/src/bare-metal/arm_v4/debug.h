#ifndef ARMv4_DEBUG_H
#define ARMv4_DEBUG_H

#include "lpc24xx_registers.h"

#undef putchar
namespace RODOS {
extern "C"
{
int puts(const char* s);
void ui2hex(char* buf, unsigned int i);
void debug_ui(unsigned int i);
void debug_char(unsigned char i);
void flush();
int putchar(const int c);
}


} //namespace

/* blink STAT_LED1 P4[17] and STAT_LED2 P4[16] */
static inline void led_green_on() { FIO4CLR = 1 << 17; }
static inline void led_green_off() { FIO4SET = 1 << 17; }

static inline void led_red_on() { FIO4CLR = 1 << 16; }
static inline void led_red_off() { FIO4SET = 1 << 16; }

#endif	// ARMv4_DEBUG_H
