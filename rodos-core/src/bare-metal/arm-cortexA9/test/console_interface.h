#ifndef __CONSOLE_INTERFACE_H__
#define __CONSOLE_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

//void console_initialize(uint32_t baudrate, uint8_t parity, uint8_t stopbits, uint8_t datasize, uint8_t flowcontrol);
char console_putChar(char c);
int console_putStr(char *c);
char console_readChar();
void console_flush();

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_INTERFACE_H__ */
