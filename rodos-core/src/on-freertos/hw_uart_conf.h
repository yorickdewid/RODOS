
/**
 * @file hw_uart_conf.cc
 * @date 2008/12/03 10:03
 * @author Olaf Frauenberger
 *
 * Copyright 2011 DLR
 *
 * @brief UART communication settings for serial port
 *
 */

#include "FreeRTOS.h"  // Contains type specifications of FreeRTOS
//#include "portmacro.h"    // FreeRTOS basic definitions needed by the following includes
#include "serial.h"       // FreeRTOS definitions for serial port


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
 * Definitions for the serial port.  
 * TODO This is only a prototype.
 *
 * FreeRTOS provides two examples on how to bind the serial port. The simple
 * example works on Posix, while the other version is ought to support a
 * specific hardware and uses threrefore more parameter.
 */
class UARTConf {

 public:

  static const eCOMPort ePort = serCOM1; //eCOMPort ePort, TODO may be something configurable
  static const eBaud eWantedBaud = ser9600;
  static const eParity eWantedParity = serNO_PARITY; 
  static const eDataBits eWantedDataBits = serBITS_8; 
  static const eStopBits eWantedStopBits = serSTOP_1; 
  static const unsigned portBASE_TYPE uxBufferLength = sizeof(char);

};

#ifndef NO_RODOS_NAMESPACE
}
#endif

