/*
 * Copyright (c) 2013 Uni Wuerzburg, Wuerzburg

 * All rights reserved.
 *
 * @author Erik Dilger
 */


#include "rodos.h"
#include "hal/hal_uart.h"



#include "uart/uart.h"
#include "registers/regsuart.h"
#include "core/ccm_pll.h"
#include "iomux_config.h"
#include <new>

#define UART_IDX_MIN        UART_IDX1
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
#define UART_IDX_MAX        UART_IDX8
#else
#define UART_IDX_MAX        UART_IDX5
#endif


#define RX_FIFO_THRESH 24
#define TX_FIFO_THRESH 8


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern "C"{
	void UART1_IRQHandler();
	void UART2_IRQHandler();
	void UART3_IRQHandler();
	void UART4_IRQHandler();
	void UART5_IRQHandler();
}



class HW_HAL_UART {

	friend class HAL_UART;

	friend void UART1_IRQHandler();
	friend void UART2_IRQHandler();
	friend void UART3_IRQHandler();
	friend void UART4_IRQHandler();
	friend void UART5_IRQHandler();

	UART_IDX idx;
	int uartInstance;
	int baudrate;
	HAL_UART* hal_uart;
	int uartRxError;

	BlockFifo<uint8_t,UART_BUF_SIZE> receiveBuffer;
	BlockFifo<uint8_t,UART_BUF_SIZE> transmittBuffer;

	volatile bool transmittRunning;

	void UARTIRQHandler();

	int init(unsigned int baudrate);
};


HW_HAL_UART* UART_contextArray[UART_IDX_MAX+1];
const int uartidxTouartinstanceMap[] = {0,HW_UART1,HW_UART2,HW_UART3,HW_UART4,HW_UART5};


#define UART_UFCR_RFDIV    BF_UART_UFCR_RFDIV(4)

uint32_t uart_get_reffreq(uint32_t instance)
{
    uint32_t div = UART_UFCR_RFDIV;
    uint32_t ret = 0;
    uint32_t freq = get_peri_clock(UART1_BAUD); // clock is same for all uarts

    if (div == BF_UART_UFCR_RFDIV(4))
        ret = freq / 2;
    else if (div == BF_UART_UFCR_RFDIV(2))
        ret = freq / 4;
    else if (div == BF_UART_UFCR_RFDIV(6))
        ret = freq / 7;

    return ret;
}




HAL_UART::HAL_UART(UART_IDX uartIdx){

    if ((uartIdx < UART_IDX_MIN) || (uartIdx > UART_IDX_MAX)) {
        context = UART_contextArray[UART_IDX0]; // UART_IDX0 is not used in this implementation
                                                 // -> so we can use this contextArray to save wrong idx
                                                 // -> with this saved idx all HAL_UART-methods will return correctly with -1
        context->idx = uartIdx;
        return;
    }

	context =  new(xmalloc(sizeof(HW_HAL_UART))) HW_HAL_UART();
	UART_contextArray[uartIdx]= context;
	context->idx = uartIdx;
	context->baudrate = 115200;
	context->hal_uart = this;
	context->uartInstance = uartidxTouartinstanceMap[uartIdx];
	context->transmittRunning=false;
};


extern "C"
{

void UART1_IRQHandler() {
	UART_contextArray[UART_IDX1]->UARTIRQHandler();

}


void UART2_IRQHandler() {
	UART_contextArray[UART_IDX2]->UARTIRQHandler();

}

void UART3_IRQHandler() {
	UART_contextArray[UART_IDX3]->UARTIRQHandler();

}


void UART4_IRQHandler() {
	UART_contextArray[UART_IDX4]->UARTIRQHandler();

}


void UART5_IRQHandler() {
	UART_contextArray[UART_IDX5]->UARTIRQHandler();

}



} // end extern "C"




/*
 * USART
 * - all USART will be initialized in 8N1 mode
 */
int HAL_UART::init(unsigned int iBaudrate) {

	if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}

	return context->init(iBaudrate);
}


int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {

	UART_IDX idx = context->idx;

	if ((idx < UART_IDX_MIN) || (idx > UART_IDX_MAX)) {return -1;}


	switch (type) {
		case UART_PARAMETER_BAUDRATE:
			if (paramVal > 0)
			{
				context->baudrate = paramVal;
			}
			else {return -1;}
			break;

		case UART_PARAMETER_HW_FLOW_CONTROL:
			return -1;
			break;

		case UART_PARAMETER_ENABLE_DMA:
		    return -1;
		    break; // end case UART_PARAMETER_HW_FLOW_CONTROL

		default: return -1;
	}

	return 0;
}


void HAL_UART::reset(){
	if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return;}

	HW_UART_UCR2_CLR(context->uartInstance, BM_UART_UCR2_TXEN | BM_UART_UCR2_RXEN | BM_UART_UCR2_SRST);
	HW_UART_UCR1_CLR(context->uartInstance, BM_UART_UCR1_UARTEN);
}


int HAL_UART::read(char* buf, int size) {

    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}
    if(size <=0) return 0;

    int readCnt = 0;
    int i = 0;

    uint8_t* p = context->receiveBuffer.getBufferToRead(readCnt);

    if (p) {
        if (readCnt > size) {readCnt = size;}

        for (i = 0; i < readCnt; i++ ) {
            buf[i] = p[i];
        }
        context->receiveBuffer.readConcluded(readCnt);

        return readCnt;
    } else {
        return 0;
    }
}


int HAL_UART::write(const char* buf, int size) {

    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}
    if(size <=0) return 0;

    int i = 0;

    int spaceinbuffer;

    uint8_t* p = context->transmittBuffer.getBufferToWrite(spaceinbuffer);

    if(p){
        if(spaceinbuffer < size){
            size=spaceinbuffer;
        }

        for (i=0;i<size;i++){
            p[i]=buf[i];
        }
        context->transmittBuffer.writeConcluded(size);

        hwDisableInterrupts();
        context->transmittRunning=true;
        HW_UART_UCR4_CLR(context->uartInstance, BM_UART_UCR4_TCEN);
		HW_UART_UCR1_SET(context->uartInstance, BM_UART_UCR1_TRDYEN);
		hwEnableInterrupts();

        return size;
    }else{
        return 0;
    }
}


int HAL_UART::getcharNoWait() {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}

	uint8_t c = 0;
	bool dataAvailible = context->receiveBuffer.get(c);

	if (dataAvailible)
	{
		return (int)c;
	}else	{
		return -1;
	}
}


int HAL_UART::putcharNoWait(char c) {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}

	if(context->transmittBuffer.put(c)){

		hwDisableInterrupts();
		context->transmittRunning=true;
		HW_UART_UCR4_CLR(context->uartInstance, BM_UART_UCR4_TCEN);
		HW_UART_UCR1_SET(context->uartInstance, BM_UART_UCR1_TRDYEN);
		hwEnableInterrupts();

		return c;
	}else{
		return -1;
	}

}


int HAL_UART::status(UART_STATUS_TYPE type) {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return -1;}

	switch (type)
	{
		case UART_STATUS_RX_BUF_LEVEL:
			return context->receiveBuffer.getElementCount();

		case UART_STATUS_RX_ERROR:
			int temp;
			temp=context->uartRxError;
			context->uartRxError=0;
			return temp;

//		case UART_STATUS_TX_COMPLETE:
//		    return (USART_GetFlagStatus(context->UARTx,USART_FLAG_TC) == SET);

		//...
		default: return -1;
	}
}


bool HAL_UART::isWriteFinished() {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return false;}
	return context->transmittBuffer.isEmpty() && ((HW_UART_USR2_RD(context->uartInstance) & BM_UART_USR2_TXDC) != 0);
}


bool HAL_UART::isDataReady() {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return false;}
	return !context->receiveBuffer.isEmpty();
}





void HW_HAL_UART::UARTIRQHandler() {

    uint8_t c;
    uint32_t usr1 = HW_UART_USR1_RD(uartInstance);
    uint32_t data;

    if((BM_UART_USR1_RRDY | BM_UART_USR1_AGTIM) & usr1){ // REcv FIFO above threshold or timeout
    	//xprintf("RECV IRQ\n");

    	bool receivedSomething=false;
    	while(  (data = HW_UART_URXD_RD(uartInstance))   & BM_UART_URXD_CHARRDY  ){ //loop while UART has chars
    		if(data & BM_UART_URXD_ERR){
    			uartRxError++;
    		}else{
    			c= data & BM_UART_URXD_RX_DATA;
    			//xprintf("%c",c);
    			if(!receiveBuffer.put(c)){
    				uartRxError++;
    			}else{
    				receivedSomething=true;
    			}

    		}

    	}
    	HW_UART_USR1_WR(uartInstance,BM_UART_USR1_AGTIM);
    	//xprintf("read upcall...\n");
    	if (receivedSomething){
    		hal_uart->upCallDataReady();
    	}
    	//xprintf("done\n");
    }

    if((BM_UART_USR1_TRDY & usr1)!=0 && transmittRunning){ // transmitt fifo below thrsh
    	//xprintf("TRANSMIT IRQ\n");
    	while(!(HW_UART_UTS_RD(uartInstance) & BM_UART_UTS_TXFULL)){
			if(transmittBuffer.get(c)){
				//xprintf("sendchar IRQ\n");
				HW_UART_UTXD_WR(uartInstance,c);
			}else{
				//xprintf("buffer empty IRQ\n");
				HW_UART_UCR4_SET(uartInstance, BM_UART_UCR4_TCEN);
				HW_UART_UCR1_CLR(uartInstance, BM_UART_UCR1_TRDYEN);
				break;
			}
    	}

    }
    if((BM_UART_USR2_TXDC & HW_UART_USR2_RD(uartInstance))!=0 && transmittRunning){ //transfer complete

    	HW_UART_UCR4_CLR(uartInstance, BM_UART_UCR4_TCEN);
    	HW_UART_UCR1_CLR(uartInstance, BM_UART_UCR1_TRDYEN);
    	transmittRunning=false;
    	//xprintf("C");
    	hal_uart->upCallWriteFinished();
    }

}



int HW_HAL_UART::init(unsigned int baudrate) {

    if ((idx < UART_IDX_MIN) || (idx > UART_IDX_MAX)) {return -1;}



	irq_hdlr_t handler;
	uint32_t irqid;

	switch (idx){
	case UART_IDX1:
		irqid=IMX_INT_UART1;
		handler=&UART1_IRQHandler;
	    break;

    case UART_IDX2:
    	irqid=IMX_INT_UART2;
		handler=&UART2_IRQHandler;
        break;

    case UART_IDX3:
    	irqid=IMX_INT_UART3;
		handler=&UART3_IRQHandler;
        break;

    case UART_IDX4:
    	irqid=IMX_INT_UART4;
		handler=&UART4_IRQHandler;
        break;

    case UART_IDX5:
    	irqid=IMX_INT_UART5;
		handler=&UART5_IRQHandler;
        break;



    default:
    	break;
	}

	register_interrupt_routine(irqid,handler);
	enable_interrupt(irqid,cpu_get_current(),128);
	gic_set_irq_security(irqid,false);

    this->baudrate = baudrate;
    uint32_t base = REGS_UART_BASE(uartInstance);
    	//int instance = HW_UART2;

    	/* configure the I/O for the port */
    	uart_iomux_config(uartInstance);

    	/* enable the source clocks to the UART port */
    	clock_gating_config(base, CLOCK_ON);

    	/* Wait for UART to finish transmitting before changing the configuration */
    	//while (!(HW_UART_UTS(instance).B.TXEMPTY)) ;

    	/* Disable UART */
    	HW_UART_UCR1_CLR(uartInstance, BM_UART_UCR1_UARTEN);

    	/* Configure FIFOs trigger level to half-full and half-empty */
    	HW_UART_UFCR_WR(uartInstance, BF_UART_UFCR_RXTL(RX_FIFO_THRESH) | UART_UFCR_RFDIV | BF_UART_UFCR_TXTL(TX_FIFO_THRESH));

    	/* Setup One Millisecond timer */
    	//HW_UART_ONEMS_WR(instance, uart_get_reffreq(instance) / 1000);

    	/* Set parity = none */
    	HW_UART_UCR2_CLR(uartInstance,(BM_UART_UCR2_PREN| BM_UART_UCR2_PROE));

    	/* Set stop bit = 1 bit */
    	HW_UART_UCR2_CLR(uartInstance, BM_UART_UCR2_STPB);

    	/* Set data size = 8 bits */
    	HW_UART_UCR2_SET(uartInstance, BM_UART_UCR2_WS);

    	/* Configure the flow control = off */
    	/* Ignore RTS */
    	HW_UART_UCR2_SET(uartInstance,  BM_UART_UCR2_IRTS);
    	/* CTS controlled by the CTS bit */
    	HW_UART_UCR2_CLR(uartInstance,  BM_UART_UCR2_CTSC);

    	/* the reference manual says that this bit must always be set */
    	HW_UART_UCR3_SET(uartInstance,  BM_UART_UCR3_RXDMUXSEL);

    	/* Enable UART */
    	HW_UART_UCR1_SET(uartInstance, BM_UART_UCR1_UARTEN);

    	/* Enable FIFOs and does software reset to clear status flags, reset
    	       the transmit and receive state machine, and reset the FIFOs */
    	HW_UART_UCR2_SET(uartInstance, BM_UART_UCR2_TXEN | BM_UART_UCR2_RXEN);
    	HW_UART_UCR2_CLR(uartInstance, BM_UART_UCR2_SRST);

    	/* Set the numerator value minus one of the BRM ratio */
    	HW_UART_UBIR_WR(uartInstance, (baudrate / 100) - 1);

    	/* Set the denominator value minus one of the BRM ratio */
    	HW_UART_UBMR_WR(uartInstance,  ((uart_get_reffreq(uartInstance) / 1600) - 1));

    	/* Optional: prevent the UART to enter debug state. Useful when debugging
    	       the code with a JTAG and without active IRQ */
    	HW_UART_UTS_SET(uartInstance, BM_UART_UTS_DBGEN);

    	// Enable Interrupts

    	HW_UART_UCR2_SET(uartInstance, BM_UART_UCR2_ATEN);
    	HW_UART_UCR1_SET(uartInstance, BM_UART_UCR1_RRDYEN);
    	//HW_UART_UCR1_SET(uartInstance, BM_UART_UCR1_TRDYEN);
    	//HW_UART_UCR4_SET(uartInstance, BM_UART_UCR4_TCEN); // Transfer Complete gets enabeld later when needed

    	return 0;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

