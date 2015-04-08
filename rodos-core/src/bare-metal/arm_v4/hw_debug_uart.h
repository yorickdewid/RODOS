
#ifndef ARMv4_DEBUG_UART_H
#define ARMv4_DEBUG_UART_H
namespace RODOS {
/**
 * @brief	Simple Implementation of UART0
 * 
 * Not interrupt driven, can be used inside an abort handler etc.
 * Used for debugging purposes (xprintf() etc.).
 * 
 * @date	May 2011
 * @author	Fabian Greif
 */
class DebugUart
{
	bool charReady;
	char lastChar;
	
public:
	static void
	initialize();
	
	/**
	 * @brief	Send a single character
	 * 
	 * Blocks until there is enough space in the send buffer.
	 */
	static void
	writechar(const char c);
	
	/// wait till all characters are send
	static void
	flush();
	
	static bool
	isCharacterAvailable();
	
	static bool
	getcharNoWait(char &c);
};
}

#endif /* ARMv4_DEBUG_UART_H */
