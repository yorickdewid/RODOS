#include <new>
#include "hal/hal_i2c.h"
#include "timemodel.h"

#include "registers/regsi2c.h"
#include "i2c/imx_i2c.h"

#include "workspace.h"
#include "rodos.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define I2C_TIMEOUT_IN_NS  			(1*MILLISECONDS)
#define I2C_LONG_TIMEOUT_IN_NS  	(10*MILLISECONDS)


class HW_HAL_I2C {
public:
	HW_HAL_I2C(int i2cinstance){
		instance = i2cinstance;
		currentState = STATE_IDLE;
	}

	int instance;
	HAL_I2C* hal_i2c;

	uint16_t I2C_SLAVE_ADDRESS7;
	uint32_t I2C_SPEED; /*!< Specifies the SCL frequency in Hz.
	 This parameter must be set to a value lower than 400kHz */

	volatile enum {STATE_IDLE,STATE_WRITEREAD,STATE_READ,STATE_READFIRST,STATE_WRITE} currentState;
	volatile uint8_t currentAdrdess;
	uint8_t* volatile currentReadBuffer;
	const uint8_t* volatile currentWriteBuffer;
	volatile uint32_t currentWriteLength;
	volatile uint32_t currentReadLength;
	volatile uint32_t bytesTransmitted;
	volatile int32_t currentError;


	int32_t beginTransfer(uint8_t addr,bool read); //waits for bus free, gen start, wait busy, send addr
	void IRQHandler();

private:
	void sendAddr(const uint8_t addr, bool read); //Send addr and prepare for data transmitt/receive
	int32_t start();
	void  stop();
};


HW_HAL_I2C I2C_contextArray[] = {HW_HAL_I2C(1),HW_HAL_I2C(2),HW_HAL_I2C(3)};

extern "C" {
	void I2C1_IRQHandler(){ I2C_contextArray[0].IRQHandler(); }
	void I2C2_IRQHandler(){ I2C_contextArray[1].IRQHandler(); }
	void I2C3_IRQHandler(){ I2C_contextArray[2].IRQHandler(); }
}


HAL_I2C::HAL_I2C(I2C_IDX idx) {
    if(idx < I2C_IDX1) idx = I2C_IDX1;
    if(idx > I2C_IDX3) idx = I2C_IDX3;
	context = &I2C_contextArray[idx-1];
	context->hal_i2c = this;
}

int32_t HAL_I2C::init(uint32_t speed) {
    /* Reset I2C module */
    reset();
	context->I2C_SPEED = speed;
	i2c_init(context->instance,speed);
	HW_I2C_I2CR_SET(context->instance,BM_I2C_I2CR_IEN);

	//configure irq
	irq_hdlr_t handler;
	uint32_t irqid;
	switch(context->instance){
	case 1:
		irqid=IMX_INT_I2C1;
		handler=&I2C1_IRQHandler;
		break;
	case 2:
		irqid=IMX_INT_I2C2;
		handler=&I2C2_IRQHandler;
		break;
	case 3:
		irqid=IMX_INT_I2C3;
		handler=&I2C3_IRQHandler;
		break;
	default:
		break;
	}
	register_interrupt_routine(irqid,handler);
	enable_interrupt(irqid,cpu_get_current(),128);
	gic_set_irq_security(irqid,false);
	//HW_I2C_I2CR_SET(context->instance,BM_I2C_I2CR_IIEN);

	return 0;
}

void HAL_I2C::reset() {
	HW_I2C_I2CR_CLR(context->instance,BM_I2C_I2CR_IEN);
	context->currentState = HW_HAL_I2C::STATE_IDLE;
}

bool HAL_I2C::isWriteFinished(){
    return context->currentState==HW_HAL_I2C::STATE_IDLE;
}
bool HAL_I2C::isReadFinished(){
    return context->currentState==HW_HAL_I2C::STATE_IDLE;
}

int32_t HAL_I2C::write(const uint8_t addr, const uint8_t* txBuf, uint32_t txBufSize) {
	if(context->currentState != HW_HAL_I2C::STATE_IDLE){
		return HAL_I2C_ERR_START;
	}
	if (txBufSize == 0) return 0;
	int32_t retVal = 0;

	context->currentAdrdess=addr;
	context->currentReadBuffer=0;
	context->currentReadLength=0;
	context->currentWriteBuffer=txBuf;
	context->currentWriteLength=txBufSize;
	context->bytesTransmitted=0;
	context->currentError=0;
	context->currentState=HW_HAL_I2C::STATE_WRITE;

	retVal = context->beginTransfer(addr,false);
	if(retVal < 0){
		return retVal;
	}

	while(!isWriteFinished()){
		suspendUntilWriteFinished();
	}

	if(context->currentError < 0){
		return context->currentError;
	}
	return context->bytesTransmitted;
}

int32_t HAL_I2C::read(const uint8_t addr, uint8_t* rxBuf, uint32_t rxBufSize) {
	if(context->currentState != HW_HAL_I2C::STATE_IDLE){
		return HAL_I2C_ERR_START;
	}
	if (rxBufSize == 0) return 0;
	int32_t retVal = 0;

	context->currentAdrdess=addr;
	context->currentReadBuffer=rxBuf;
	context->currentReadLength=rxBufSize;
	context->currentWriteBuffer=0;
	context->currentWriteLength=0;
	context->bytesTransmitted=0;
	context->currentError=0;
	context->currentState=HW_HAL_I2C::STATE_READFIRST;

	retVal = context->beginTransfer(addr,true);
	if(retVal < 0){
		return retVal;
	}

	while(!isReadFinished()){
		suspendUntilReadFinished();
	}

	if(context->currentError < 0){
		return context->currentError;
	}
	return context->bytesTransmitted;

}

int32_t HAL_I2C::writeRead(const uint8_t addr, const uint8_t* txBuf, uint32_t txBufSize, uint8_t* rxBuf, uint32_t rxBufSize) {
	if(context->currentState != HW_HAL_I2C::STATE_IDLE){
		return HAL_I2C_ERR_START;
	}
	if (rxBufSize == 0 || rxBufSize == 0) return 0;
	int32_t retVal = 0;

	context->currentAdrdess=addr;
	context->currentReadBuffer=rxBuf;
	context->currentReadLength=rxBufSize;
	context->currentWriteBuffer=txBuf;
	context->currentWriteLength=txBufSize;
	context->bytesTransmitted=0;
	context->currentError=0;
	context->currentState=HW_HAL_I2C::STATE_WRITEREAD;

	retVal = context->beginTransfer(addr,false);
	if(retVal < 0){
		return retVal;
	}

	while(!isReadFinished()){
		suspendUntilReadFinished();
	}

	if(context->currentError < 0){
		return context->currentError;
	}
	return context->bytesTransmitted;
}



/*****************************************************************************************************************/
/*
 * class HW_HAL_I2C
 */

void HW_HAL_I2C::sendAddr(const uint8_t addr, bool read){
	HW_I2C_I2CR_SET(instance,BM_I2C_I2CR_IIEN);
	HW_I2C_I2DR_WR(instance, (addr<<1) | (read ? 1:0));
}

int32_t HW_HAL_I2C::beginTransfer(uint8_t addr,bool read){

	int64_t timeoutTime;
	do{
		//Wait for Bus free
		timeoutTime = NOW()+ I2C_LONG_TIMEOUT_IN_NS;
		while(HW_I2C_I2SR(instance).B.IBB !=0){
			if(NOW() > timeoutTime){
				stop();
				return HAL_I2C_ERR_START;
			}
			Thread::yield();
		}

		// Select master mode, assert START signal and also indicate TX mode and wait for bus bus busy
		HW_I2C_I2SR_WR(instance, 0); // clear both IAL and IIF bits
		HW_I2C_I2CR_SET(instance, BM_I2C_I2CR_MSTA | BM_I2C_I2CR_MTX);
	}while(HW_I2C_I2SR(instance).B.IAL != 0);

	//Wait for Bus busy to be set
	timeoutTime = NOW()+ I2C_TIMEOUT_IN_NS;
	while(HW_I2C_I2SR(instance).B.IBB ==0){
		if(NOW() > timeoutTime){
			stop();
			return HAL_I2C_ERR_START;
		}
	}

	sendAddr(addr,read);
	return 0;
}

void HW_HAL_I2C::IRQHandler(){
	uint16_t dummy;

	uint16_t status = HW_I2C_I2SR_RD(instance);
	HW_I2C_I2SR_WR(instance,0); //Ack interrupt and arbitration lost

	if((status & BM_I2C_I2SR_IAL) ==1){ //Arrbitration lost, just stop
		currentError = HAL_I2C_ERR_ADDR;
		stop();
		return;
	}

	switch(currentState){
	case STATE_READFIRST:
		HW_I2C_I2CR_CLR(instance,BM_I2C_I2CR_MTX);
		if(bytesTransmitted == currentReadLength-1){
			HW_I2C_I2CR_SET(instance,BM_I2C_I2CR_TXAK);
		}
		dummy = HW_I2C_I2DR_RD(instance);dummy++;//Suppress unsued warning
		bytesTransmitted=0;
		currentState = STATE_READ;
		break;

	case STATE_READ:
		bytesTransmitted++;
		if(bytesTransmitted == currentReadLength-1){
			HW_I2C_I2CR_SET(instance,BM_I2C_I2CR_TXAK);
		}
		if(bytesTransmitted == currentReadLength){
			stop();
		}
		currentReadBuffer[bytesTransmitted-1] = HW_I2C_I2DR_RD(instance);
		break;

	case STATE_WRITEREAD:
	case STATE_WRITE:
		if((status & BM_I2C_I2SR_RXAK) != 0){
			//No ACK
			if(bytesTransmitted==0){
				currentError = HAL_I2C_ERR_WRITE;
			}
			stop();
			return;
		}

		bytesTransmitted++;
		if(bytesTransmitted-1 >= currentWriteLength){ //address byte also counts
			if(currentState==STATE_WRITEREAD){

				HW_I2C_I2CR_SET(instance,BM_I2C_I2CR_RSTA);
				int64_t waitUntil = NOW() + 200;
				while(NOW() < waitUntil) {}

				currentState = STATE_READFIRST;
				sendAddr(currentAdrdess,true);
				return;
			}else{
				currentError=0;
				stop();
				bytesTransmitted--;
				return;
			}
		}
		HW_I2C_I2DR_WR(instance,currentWriteBuffer[bytesTransmitted-1]);
		break;

	case STATE_IDLE:
		break;
	default:
		break;
	}
}

void HW_HAL_I2C::stop(){
	if(currentState==STATE_WRITE){
		currentState = STATE_IDLE;
		hal_i2c->upCallWriteFinished();
	}else{
		currentState = STATE_IDLE;
		hal_i2c->upCallReadFinished();
	}
	HW_I2C_I2CR_CLR(instance,BM_I2C_I2CR_IIEN | BM_I2C_I2CR_MSTA | BM_I2C_I2CR_MTX | BM_I2C_I2CR_TXAK);
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

