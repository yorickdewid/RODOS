/**
 * @file hal_uart.cpp
 * @author Tobias Mikschl <tobias.mikschl@uni-wuerzburg.de>
 *
 * @brief C Hardware Abstraction Layer for UART
 */


/* IDX mapping:
UART_IDX0 = UART_Lite

// only on Biros:
UART_IDX1 = TC0 from PPU Module
UART_IDX2 = TC1 from PPU Module
*/


#include "rodos.h"
#include "hal/hal_uart.h"
#include "bsp.h"
#include "addresses.h"



#ifdef USE_DEV_BOARD_BIROS

/* Defintions of control bits in 
   tc ctrl */

#define SBC1_EN       0
#define TX1_RST       1
#define RX1_RST       2
#define TX1_EMPTY     4
#define TX1_AFULL     5
#define RX1_EMPTY     6
#define RX1_OVERFLOW  7
#define TX1_IRQ       8
#define RX1_IRQ       9
#define SBC2_EN       16
#define TX2_RST       17
#define RX2_RST       18
#define TX2_EMPTY     20
#define TX2_AFULL     21
#define RX2_EMPTY     22
#define RX2_OVERFLOW  24
#define TX2_IRQ       25
#define RX2_IRQ       26
#endif

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern "C"{
  void UART0_IRQHandler();
#ifdef USE_DEV_BOARD_BIROS
	void TC1_IRQHandler();
	void TC2_IRQHandler();
#endif
}

class HW_HAL_UART {
	friend class ReceiveTrigger;
	friend class HAL_UART;

	friend void UART0_IRQHandler();
#ifdef USE_DEV_BOARD_BIROS
	friend void TC1_IRQHandler();
	friend void TC2_IRQHandler();
#endif

	UART_IDX idx;
	int baudrate;
	HAL_UART* hal_uart;
	int uartRxError;

	void UARTIRQHandler();

};

HW_HAL_UART UART_contextArray[3];

/* Interrupt Handlers direct called from interrupt.cpp.
 * redirect corresponding class interrupt handler*/
extern "C"
{
  void UART0_IRQHandler() {
    UART_contextArray[0].UARTIRQHandler();
  }
#ifdef USE_DEV_BOARD_BIROS
	void TC1_IRQHandler() {
    UART_contextArray[1].UARTIRQHandler();
  }
	void TC2_IRQHandler() {
    UART_contextArray[2].UARTIRQHandler();
  }
#endif
}

/* specific Interrupt handler */
void HW_HAL_UART::UARTIRQHandler(){
    int value;
    if(idx == UART_IDX0) {
      value = *(UART0_BASEADDR + uart_statOffset);
      if ((value & 0x01) != 0x00) hal_uart->upCallDataReady();
      if ((value & 0x04) != 0x00) hal_uart->upCallWriteFinished();
    } 
#ifdef USE_DEV_BOARD_BIROS
    else if (idx == UART_IDX1) {
      value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
      if ((value & (1<<RX1_EMPTY))==0x00) 
    	  hal_uart->upCallDataReady();
      if ((value & (1<<TX1_EMPTY))!=0x00) 
    	  hal_uart->upCallWriteFinished();
    }
    else if (idx == UART_IDX2) {
      value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
      if((value & (1<<RX2_EMPTY))==0x00) 
    	  hal_uart->upCallDataReady();
      if((value & (1<<TX2_EMPTY))!=0x00) 
    	  hal_uart->upCallWriteFinished();
    }
#endif

}

HAL_UART::HAL_UART(UART_IDX uartIdx) {
	context = &UART_contextArray[uartIdx];
	context->idx = uartIdx;
	context->baudrate = 115200;
	context->hal_uart = this;
}


int uart0_init(unsigned int baudrate) {
  *(UART0_BASEADDR + uart_ctrlOffset) = 0x13;
  return 0;
}
#ifdef USE_DEV_BOARD_BIROS
int tc1_init(unsigned int baudrate) {
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<TX1_RST);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<RX1_RST);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<SBC1_EN);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<RX1_IRQ);
  return 0;
}
int tc2_init(unsigned int baudrate) {
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<TX2_RST);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<RX2_RST);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<SBC2_EN);
  *(PPU_BASEADDR + ppu_tcCtrlOffset) |= (1<<RX2_IRQ);
  return 0;
}
#endif
int HAL_UART::init(unsigned int baudrate){
  UART_IDX idx = context->idx;

	if ((idx < 0) || (idx > 2)) {return -1;}

	context->baudrate = baudrate;

	switch (idx)
		{
			case 0: return uart0_init(baudrate);
#ifdef USE_DEV_BOARD_BIROS
			case 1: return tc1_init(baudrate);
			case 2: return tc2_init(baudrate);
#endif
			default: return -1;
		}
}




void HAL_UART::reset(){

}

int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal){
  return 0;
}

int HAL_UART::status(UART_STATUS_TYPE type){
  return 0;
}


bool HAL_UART::isWriteFinished(){
  int value;
  if(context->idx == 0)
  { value = *(UART0_BASEADDR + uart_statOffset);
    return (value & 0x04) != 0; 
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1)
  { value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
	return (value & (1<<TX1_EMPTY)) != 0x00;
  }
  else if (context->idx == 2)
  { value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
    return (value & (1<<TX2_EMPTY)) != 0x00;
  }
#endif
  else
    return true;
}

bool HAL_UART::isDataReady(){
  int value;
  if(context->idx == 0)
  { value = *(UART0_BASEADDR + uart_statOffset);
    return (value & 0x01) != 0X00; 
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1)
  { value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
    return (value & (1<<RX1_EMPTY)) == 0x00;
  }
  else if (context->idx == 2)
  { value = *(PPU_BASEADDR + ppu_tcCtrlOffset);
    return (value & (1<<RX2_EMPTY)) == 0x00;
  }
#endif
  else 
    return false;
}


int HAL_UART::write(const char* sendBuf, int len){
  int i=-1;
  int value;
  if(context->idx == 0) {
    for(i=0;i<len && ((*(UART0_BASEADDR + uart_statOffset) & (1 << 3)) == 0x00);i++) {
      *(UART0_BASEADDR + uart_txOffset) = sendBuf[i];
    }
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1){
	  for (i = 0; i < len && ((value = *(PPU_BASEADDR + ppu_tcCtrlOffset)) & (1<<TX1_AFULL))==0x00; i++ )
	  {
		  *(PPU_BASEADDR + ppu_tc1DataOffset) = sendBuf[i];
	  }	  
  }
  else if (context->idx == 2) {
	  for (i = 0; i < len && ((value = *(PPU_BASEADDR + ppu_tcCtrlOffset)) & (1<<TX2_AFULL))==0x00; i++ )
	  {
		  *(PPU_BASEADDR + ppu_tc2DataOffset) = sendBuf[i];
	  }	  
  }
#endif
  return i;
}

int HAL_UART::read(char* recBuf, int maxLen){
  int i=-1;
  if(context->idx == 0) {
    for(i=0;i<maxLen && isDataReady();i++) {
      recBuf[i] = *(UART0_BASEADDR + uart_rxOffset);
    }
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1){
	  for (i = 0; i < maxLen &&  isDataReady(); i++ )
	  {
		  recBuf[i] = *(PPU_BASEADDR + ppu_tc1DataOffset) & 0xff;
	  }	  
  }
  else if (context->idx == 2) {
	  for (i = 0; i < maxLen &&  isDataReady(); i++ )
	  {
		  recBuf[i] = *(PPU_BASEADDR + ppu_tc2DataOffset) & 0xff;
	  }	  
  }
#endif
  return i;
}

int HAL_UART::getcharNoWait(){
  uint8_t c = 0;
  
  if(context->idx == 0) {
	  if( isDataReady() ) {
      c = *(UART0_BASEADDR + uart_rxOffset) & 0xff;
      return c;
    } 
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1) {
    if( (*(PPU_BASEADDR + ppu_tcCtrlOffset) & (1<<RX1_EMPTY))==0x00 ) {
      c = *(PPU_BASEADDR + ppu_tc1DataOffset) & 0xff;
      return c;
    } 
  }
  else if (context->idx == 2) {
   if( (*(PPU_BASEADDR + ppu_tcCtrlOffset) & (1<<RX2_EMPTY))==0x00 ) {
      c=*(PPU_BASEADDR + ppu_tc2DataOffset);
      return c ;
    } 
  }
#endif
  return -1;
}

int HAL_UART::putcharNoWait(char c){
  if(context->idx == 0) {
     if((*(UART0_BASEADDR + uart_statOffset) & (1 << 3)) == 0x00) {
      *(UART0_BASEADDR + uart_txOffset) = c;
      return c;
    } 
  }
#ifdef USE_DEV_BOARD_BIROS
  else if (context->idx == 1) {
    if((*(PPU_BASEADDR + ppu_tcCtrlOffset) & (1<<TX1_AFULL))==0x00) {
      *(PPU_BASEADDR + ppu_tc1DataOffset) = c;
      return c;
    } 
  }
  else if (context->idx == 2) {
    if((*(PPU_BASEADDR + ppu_tcCtrlOffset) & (1<<TX2_AFULL))==0x00) {
      *(PPU_BASEADDR + ppu_tc2DataOffset) = c;
      return c;
    } 
  }
#endif
  return -1;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif
