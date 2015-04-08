/*
 * Copyright (c) 2014 Uni Wuerzburg, Wuerzburg

 * All rights reserved.
 *
 * @author Erik Dilger
 */


#include "rodos.h"
#include "hal/hal_uart.h"
#include <new>

#include "uart_irda_cir.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "hw_types.h"
#include "interrupt.h"


#define UART_IDX_MIN        UART_IDX0
#define UART_IDX_MAX        UART_IDX5

#define UART_MODULE_INPUT_CLK                (48000000)


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern "C"{
	void UART0_IRQHandler();
	void UART1_IRQHandler();
	void UART2_IRQHandler();
	void UART3_IRQHandler();
	void UART4_IRQHandler();
	void UART5_IRQHandler();
}

class HW_HAL_UART {
	friend class HAL_UART;
	friend void UART0_IRQHandler();
	friend void UART1_IRQHandler();
	friend void UART2_IRQHandler();
	friend void UART3_IRQHandler();
	friend void UART4_IRQHandler();
	friend void UART5_IRQHandler();

	UART_IDX idx;
	uint32_t uartInstance;
	int baudrate;
	HAL_UART* hal_uart;
	int uartRxError;

	BlockFifo<uint8_t,UART_BUF_SIZE> receiveBuffer;
	BlockFifo<uint8_t,UART_BUF_SIZE> transmittBuffer;

	//volatile bool transmittRunning;

	void UARTIRQHandler();
	int init(unsigned int baudrate);
};


HW_HAL_UART* UART_contextArray[UART_IDX_MAX+1];
const uint32_t uartidxTouartinstanceMap[] = {SOC_UART_0_REGS,SOC_UART_1_REGS,SOC_UART_2_REGS,SOC_UART_3_REGS,SOC_UART_4_REGS,SOC_UART_5_REGS};



HAL_UART::HAL_UART(UART_IDX uartIdx){

    if ((uartIdx < UART_IDX_MIN) || (uartIdx > UART_IDX_MAX)) {return;}

	context =  new(xmalloc(sizeof(HW_HAL_UART))) HW_HAL_UART();
	UART_contextArray[uartIdx]= context;
	context->idx = uartIdx;
	context->baudrate = 115200;
	context->hal_uart = this;
	context->uartInstance = uartidxTouartinstanceMap[uartIdx];
};


extern "C"
{
	void UART0_IRQHandler() { UART_contextArray[UART_IDX0]->UARTIRQHandler(); }
	void UART1_IRQHandler() { UART_contextArray[UART_IDX1]->UARTIRQHandler(); }
	void UART2_IRQHandler() { UART_contextArray[UART_IDX2]->UARTIRQHandler(); }
	void UART3_IRQHandler() { UART_contextArray[UART_IDX3]->UARTIRQHandler(); }
	void UART4_IRQHandler() { UART_contextArray[UART_IDX4]->UARTIRQHandler(); }
	void UART5_IRQHandler() { UART_contextArray[UART_IDX5]->UARTIRQHandler(); }
} // end extern "C"



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

	UARTModuleReset(context->uartInstance);
	context->receiveBuffer.clear();
	context->transmittBuffer.clear();
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

        HWREG(context->uartInstance + UART_SCR) &= ~UART_SCR_TX_EMPTY_CTL_IT;
        HWREG(context->uartInstance + UART_IER) |= UART_INT_THR;
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

		HWREG(context->uartInstance + UART_SCR) &= ~UART_SCR_TX_EMPTY_CTL_IT;
		HWREG(context->uartInstance + UART_IER) |= UART_INT_THR;
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

		default: return -1;
	}
}

bool HAL_UART::isWriteFinished() {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return false;}
	return context->transmittBuffer.isEmpty() && (HWREG(context->uartInstance + UART_LSR) & UART_LSR_TX_SR_E);
}

bool HAL_UART::isDataReady() {
    if ((context->idx < UART_IDX_MIN) || (context->idx > UART_IDX_MAX)) {return false;}
	return !context->receiveBuffer.isEmpty();
}


void HW_HAL_UART::UARTIRQHandler() {

    uint8_t c;
    uint32_t intStatus;

    while((intStatus=UARTIntPendingStatusGet(uartInstance)) == UART_INT_PENDING){
    	bool transmitBufferEmpty=false;
    	uint32_t intPending = UARTIntIdentityGet(uartInstance);

    	switch(intPending){

    	case UART_INTID_CHAR_TIMEOUT:
       	case UART_INTID_RX_THRES_REACH:
    		while(HWREG(uartInstance + UART_LSR) & UART_LSR_RX_FIFO_E){
    			uint32_t errorStatus = HWREG(uartInstance + UART_LSR);
				c = (uint8_t)HWREG(uartInstance + UART_RHR);
				if(errorStatus & UART_LSR_RX_OE){
					uartRxError++;
				}
				if(errorStatus & (UART_LSR_RX_BI | UART_LSR_RX_FE | UART_LSR_RX_PE)){
					uartRxError++;
				}else{
					if(!receiveBuffer.put(c)){
						uartRxError++;
					}
				}
    		}
    		hal_uart->upCallDataReady();
    		break;

    	case UART_INTID_TX_THRES_REACH:
    		while((!(HWREG(uartInstance + UART_SSR) & UART_SSR_TX_FIFO_FULL)) && !transmitBufferEmpty){
    			if(transmittBuffer.get(c)){
     				HWREG(uartInstance + UART_THR) = c;
    			}else{//Buffer Empty
    				transmitBufferEmpty=true;
    			}
    		}

    		if(transmitBufferEmpty){
    			HWREG(uartInstance + UART_SCR) |= UART_SCR_TX_EMPTY_CTL_IT;
    			if(HWREG(uartInstance + UART_LSR) & UART_LSR_TX_SR_E){
    				HWREG(uartInstance + UART_IER) &= ~UART_INT_THR;
    				hal_uart->upCallWriteFinished();
    			}
    		}
			break;

    	default:
    		break;
    	}

    }
}


int HW_HAL_UART::init(unsigned int baudrate) {
    if ((idx < UART_IDX_MIN) || (idx > UART_IDX_MAX)) {return -1;}

	IntHandlerFunc handler;
	uint32_t irqid;
	switch (idx){
	case UART_IDX0:
		irqid=SYS_INT_UART0INT;
		handler=&UART0_IRQHandler;
	    break;
	case UART_IDX1:
		irqid=SYS_INT_UART1INT;
		handler=&UART1_IRQHandler;
		break;
	case UART_IDX2:
		irqid=SYS_INT_UART2INT;
		handler=&UART2_IRQHandler;
		break;
	case UART_IDX3:
		irqid=SYS_INT_UART3INT;
		handler=&UART3_IRQHandler;
		break;
	case UART_IDX4:
		irqid=SYS_INT_UART4INT;
		handler=&UART4_IRQHandler;
		break;
	case UART_IDX5:
		irqid=SYS_INT_UART5INT;
		handler=&UART5_IRQHandler;
		break;
    default:
    	break;
	}

	if(idx != UART_IDX0){
		UART12345ModuleClkConfig((int)idx);
		UARTPinMuxSetup((int)idx);
	}
	hal_uart->reset();

	IntRegister(irqid, handler);
	IntPrioritySet(irqid, 0, AINTC_HOSTINT_ROUTE_IRQ);

    this->baudrate = baudrate;

    /* Performing FIFO configurations. */
    unsigned int fifoConfig = 0;
     /* Setting the TX and RX FIFO Trigger levels as 1. No DMA enabled. */
     fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_4,
                                   UART_TRIG_LVL_GRANULARITY_4,
                                   (UART_FCR_TX_TRIG_LVL_32),
                                   (UART_FCR_RX_TRIG_LVL_16),
                                   1,
                                   1,
                                   UART_DMA_EN_PATH_SCR,
                                   UART_DMA_MODE_0_ENABLE);
     /* Configuring the FIFO settings. */
     UARTFIFOConfig(uartInstance, fifoConfig);
     /* Performing Baud Rate settings. */
     unsigned int divisorValue = 0;
     /* Computing the Divisor Value. */
     divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                          baudrate,
                                          UART16x_OPER_MODE,
                                          UART_MIR_OVERSAMPLING_RATE_42);

     double bauderror = ((double)UART_MODULE_INPUT_CLK / (16.0 * (double)divisorValue)) - (double)baudrate;
     bauderror = bauderror>0 ? bauderror : -bauderror;
     //xprintf("baudrateerror is %f%%\n",(bauderror/baudrate)*100.0);
     /* Programming the Divisor Latches. */
     UARTDivisorLatchWrite(uartInstance, divisorValue);

     /* Switching to Configuration Mode B. */
     //UARTRegConfigModeEnable(uartInstance, UART_REG_CONFIG_MODE_B);

     /* Programming the Line Characteristics. */
     UARTLineCharacConfig(uartInstance,
                          (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1),
                          UART_PARITY_NONE);

     /* Disabling write access to Divisor Latches. */
     UARTDivisorLatchDisable(uartInstance);
     /* Disabling Break Control. */
     UARTBreakCtl(uartInstance, UART_BREAK_COND_DISABLE);
     /* Switching to UART16x operating mode. */
     UARTOperatingModeSelect(uartInstance, UART16x_OPER_MODE);

     UARTIntEnable(uartInstance,UART_INT_RHR_CTI);

 	IntSystemEnable(irqid);
    return 0;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

