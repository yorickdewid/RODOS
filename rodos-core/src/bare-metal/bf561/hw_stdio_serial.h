/***********************************************************************************************//**
 * \ingroup HK
 * \file hw_stdio_serial.h
 * \version 0.1
 * \date 12.03.2014
 * \author Georg Raith
 *
 * \brief 
 * \b History: \n
 *
 * 12.03.2014 rg: initial version
 */

#ifndef HW_STDIO_SERIAL_H_
#define HW_STDIO_SERIAL_H_

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

int UART_inititialize(int divisor);
int putchar(int c);


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* HW_STDIO_SERIAL_H_ */
