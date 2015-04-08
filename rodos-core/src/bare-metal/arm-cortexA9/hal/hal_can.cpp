/*
 * hal_can.cpp
 *
 *  Created on: 12.08.2013
 *      Author: Erik Dilger
 */

#include "hal/hal_can.h"
#include "rodos.h"

//extern "C"{
	#include "workspace.h"
//}
#include <new>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define MAX_FILTERS 32
#define TX_MAILBOX 63

#define SRR_BIT (1 << 22)
#define IDE_BIT (1 << 21)
#define RTR_BIT (1 << 20)

#define CODE_SHIFT 24
#define DLC_SHIFT 16

#define CODE_INACTIVERX 0x0
#define CODE_EMPTY    0x4
#define CODE_OVERRUN  0x6
#define CODE_RANSWER  0xA
#define CODE_TANSWER  0xE
#define CODE_BUSY     0x1
#define CODE_INACTIVETX 0x8
#define CODE_ABORT    0x9
#define CODE_DATA     0xC

#define REG_CTRL2(base) ((base)+ 0x34)
#define REG_RXFGMASK(base) ((base)+ 0x48)
#define REG_RXFIR(base) ((base)+ 0x4C)
#define REG_RXIMR0(base) ((base)+ 0x880)
#define REG_RXIMR_SET(base,x,v) (((volatile uint32_t*)REG_RXIMR0(base))[x] = (v))

#define REG_SET(addr,v) (*((volatile uint32_t*)addr) |=  (v))
#define REG_CLR(addr,v) (*((volatile uint32_t*)addr) &= ~(v))
#define REG_RD(addr)    (*((volatile uint32_t*)addr))

#define REG_CTRL2_RFFN_SHIFT 24
#define REG_CTRL2_RFFN_MASK (0xF << REG_CTRL2_RFEN_SHIFT)
#define REG_CTRL2_RRS_MASK (0x1 << 17)

#define REG_RXFIR_IDHIT_MASK 0x1FF

#define FIFO_FLAG_WRNFULL 7
#define FIFO_FLAG_OVERFLOW 6
#define FIFO_FLAG_DATARDY 5

#define FIFO_FILTER_ENTRY(base,x,v) ((volatile uint32_t*)((base)+ 0xE0))[x] = (v)


static hw_module_t can1_port = {
    "CAN1",          // name
    1,               // instance number
    CAN1_BASE_ADDR,  // base_address
    30000000,        // frequency CAN_CLK_ROOT aka can_PE_clock
    IMX_INT_FLEXCAN1,    // interrut source ID
    // void (*irq_subroutine)(void); // IRQ subroutine
    // void (*iomux_config) (void); // module iomux configuration fucntion
};

static hw_module_t can2_port = {
    "CAN2",           // name
    2,                // instance number
    CAN2_BASE_ADDR,   // base_address
    30000000,         // frequency CAN_CLK_ROOT aka can_PE_clock
    IMX_INT_FLEXCAN2,     // interrut source ID
    //&can2_rx_handler, // IRQ subroutine
    // void (*iomux_config) (void); // module iomux configuration fucntion
};

struct CAN_Filter{
	uint32_t canID;
	uint32_t canIDmask;
	HAL_CAN* owner;
	HW_HAL_CAN* owner_context;
};

struct CanMsg {
	uint32_t id;
	uint32_t cs;
	union {
		struct {
			uint32_t firstDataWord;
			uint32_t secondDataWord;
		};
		uint8_t data[8];
	};
};

extern "C"{
	void CAN1_IRQHandler();
	void CAN2_IRQHandler();
}


class CAN_Ctrl{
public:
	bool initialized;
	hw_module_t* can;
	Semaphore CANCtrlProtector;

	CAN_Filter filters[MAX_FILTERS];
	int numFilters;

	Fifo<CanMsg,16> txFifo;
	volatile bool txInProgrss;

	CAN_Ctrl(hw_module_t* _can);
	void init(uint32_t baudrate);

	void putIntoTxMailbox(CanMsg& msg);
	void getFromRxFifo(CanMsg& msg);
	bool setupFilters();

	void IRQHandler();
};

class HW_HAL_CAN {
	public:
	CAN_Ctrl* ctrl;
	Fifo<CanMsg,64> RxFifo;

	HW_HAL_CAN();
};


CAN_Ctrl CANs[2] = { CAN_Ctrl(&can1_port),CAN_Ctrl(&can2_port)};


CAN_Ctrl::CAN_Ctrl(hw_module_t* _can){
	initialized = false;
	can=_can;
	numFilters=0;
	txInProgrss=false;
}

enum can_bitrate mapToFlexcanBitrate(uint32_t baudrate){
	switch(baudrate){
		case 1000000: return MBPS_1;
		case  800000: return KBPS_800;
		case  500000: return KBPS_500;
		case  250000: return KBPS_250;
		case  125000: return KBPS_125;
		case   62500: return KBPS_62; //62.5kps
		case   20000: return KBPS_20;
		case   10000: return KBPS_10;
		default:
			PRINTF("CAN Baudrate not supported: %d\n",(int)baudrate);
			return MBPS_1;
	}
}

void CAN_Ctrl::init(uint32_t baudrate){
	if(!initialized){
		initialized=true;

		can_init(can, CAN_LAST_MB);

		imx_flexcan canmodule;
		can_set_can_attributes(&canmodule,mapToFlexcanBitrate(baudrate),can);
		can_update_bitrate(&canmodule);

		for(int i=0;i<CAN_NUMBER_OF_BUFFERS;i++){
			can_mb_int_ack(can,i);
		}

		//configure fifo
		HW_FLEXCAN_MCR_SET(can->instance,BM_FLEXCAN_MCR_FEN); //set RFEN
		HW_FLEXCAN_MCR_SET(can->instance,BM_FLEXCAN_MCR_BCC);//set IRMQ
		HW_FLEXCAN_MCR_CLR(can->instance,BM_FLEXCAN_MCR_IDAM); // filter format A
		REG_SET(REG_RXFGMASK(can->base),~0);
		//conif RFFN
		REG_SET(REG_CTRL2(can->base),0xF <<REG_CTRL2_RFFN_SHIFT);

		//configure other flags
		HW_FLEXCAN_MCR_SET(can->instance,BM_FLEXCAN_MCR_SRX_DIS); //No self recv
		REG_SET(REG_CTRL2(can->base),REG_CTRL2_RRS_MASK);

		//configure irq
		irq_hdlr_t handler;
		uint32_t irqid;

		if(this==&CANs[0]){
			irqid=IMX_INT_FLEXCAN1;
			handler=&CAN1_IRQHandler;
		}else if(this==&CANs[1]){
			irqid=IMX_INT_FLEXCAN2;
			handler=&CAN2_IRQHandler;
		}
		register_interrupt_routine(irqid,handler);
		enable_interrupt(irqid,cpu_get_current(),128);
		gic_set_irq_security(irqid,false);

		setupFilters();

		can_exit_freeze(can);
		can_enable_mb_interrupt(can,FIFO_FLAG_DATARDY);
	}
}

void CAN_Ctrl::putIntoTxMailbox(CanMsg& msg){
	set_can_mb(can,TX_MAILBOX,msg.cs | (CODE_DATA << CODE_SHIFT),msg.id,msg.firstDataWord,msg.secondDataWord);
	txInProgrss=true;
}

void CAN_Ctrl::getFromRxFifo(CanMsg& msg){
	 volatile struct can_message_buffers *can_mb =
	        (volatile struct can_message_buffers *)(can->base + CAN_MB_OFFSET); //FIFO output is the first mailbox

	 msg.cs=can_mb->MB[0].cs;
	 msg.id=can_mb->MB[0].id;

	 msg.firstDataWord =can_mb->MB[0].data0;
	 msg.secondDataWord=can_mb->MB[0].data1;
}

bool CAN_Ctrl::setupFilters(){
	uint32_t lastFilter=0;

	for(int i=0;i<MAX_FILTERS;i++){
		if(i<numFilters){
			lastFilter=filters[i].canID;
			REG_RXIMR_SET(can->base,i,filters[i].canIDmask);
		}else{
			REG_RXIMR_SET(can->base,i,~0);
		}
		FIFO_FILTER_ENTRY(can->base,i,lastFilter);
	}
	for(int i=MAX_FILTERS;i<128;i++){
		FIFO_FILTER_ENTRY(can->base,i,lastFilter);
	}

	return true;
}

char buf[20];
void CAN_Ctrl::IRQHandler(){
	CanMsg msg;
	uint64_t iflags= can_mb_int_flag(can);
#define CHECK_IFLAG(n) ((iflags & ((uint64_t)1<<(n))) != 0)

	if(CHECK_IFLAG(TX_MAILBOX)){

		if(txFifo.get(msg)){
			putIntoTxMailbox(msg);
		}else{
			txInProgrss=false;
			//This may call upCall multiple times for one hal, but it is simpleset solution
			for (int i=0;i<numFilters;i++){
				filters[i].owner->upCallWriteFinished();
			}
		}
		can_mb_int_ack(can,TX_MAILBOX);
	}

	while(CHECK_IFLAG(FIFO_FLAG_DATARDY)){
		if(numFilters>0){
			int32_t matchedFilter = REG_RD(REG_RXFIR(can->base)) & 0x1FF;
			if(matchedFilter >= numFilters){
				matchedFilter =numFilters-1;
			}

			getFromRxFifo(msg);
			if(filters[matchedFilter].owner_context->RxFifo.put(msg)){
				filters[matchedFilter].owner->upCallDataReady();
			}
			//Silently ignore FIFO overflow
		}

		can_mb_int_ack(can,FIFO_FLAG_DATARDY);
		iflags= can_mb_int_flag(can);

	}

}

HW_HAL_CAN::HW_HAL_CAN(){
}

HAL_CAN::HAL_CAN(CAN_IDX canIdx, GPIO_PIN rxPin, GPIO_PIN txPin){
	context= new(xmalloc(sizeof(HW_HAL_CAN))) HW_HAL_CAN();
	context->ctrl=&CANs[canIdx-1];
}

int HAL_CAN::init(unsigned int baudrate){
	context->ctrl->init(baudrate);
	return 0;
}

void HAL_CAN::reset(){
	context->ctrl->CANCtrlProtector.enter();
	context->ctrl->initialized=false;
	can_freeze(context->ctrl->can);
	while((HW_FLEXCAN_MCR_RD(context->ctrl->can->instance) & BM_FLEXCAN_MCR_FRZ_ACK)==0) { }
	can_sw_reset(context->ctrl->can);

	context->ctrl->txInProgrss=false;

	context->ctrl->CANCtrlProtector.leave();
}

int HAL_CAN::config(CAN_PARAMETER_TYPE type, int paramVal){
	switch(type){
	case CAN_PARAMETER_BAUDRATE:
		context->ctrl->CANCtrlProtector.enter();
			reset();
			init(paramVal);
		context->ctrl->CANCtrlProtector.leave();
		return 0;
	}
	return -1;
}


int  HAL_CAN::status(CAN_STATUS_TYPE type){
	switch(type){
	case CAN_STATUS_RX_ERROR:
		return (HW_FLEXCAN_ECR_RD(context->ctrl->can->instance)>>8) & 0xFF;
	case CAN_STATUS_TX_ERROR:
		return  HW_FLEXCAN_ECR_RD(context->ctrl->can->instance)     & 0xFF;
	case CAN_STATUS_RX_LEVEL:
		return context->RxFifo.getElementCount();
	}
	return -1;
}

bool  HAL_CAN::isWriteFinished(){
    return !context->ctrl->txInProgrss;
}


bool  HAL_CAN::isDataReady(){
	return !context->RxFifo.isEmpty();
}

bool  HAL_CAN::addIncomingFilter(uint32_t ID, uint32_t IDMask, bool extID, bool rtr){
	if(context->ctrl->numFilters >= MAX_FILTERS)
		return false;

	context->ctrl->CANCtrlProtector.enter();
	can_freeze(context->ctrl->can);
	while((HW_FLEXCAN_MCR_RD(context->ctrl->can->instance) & BM_FLEXCAN_MCR_FRZ_ACK)==0) { }

	context->ctrl->filters[context->ctrl->numFilters].canID=     (extID ?   ID     <<1 :   ID      << 19) & 0x3FFFFFFE;
	context->ctrl->filters[context->ctrl->numFilters].canIDmask= (extID ? (~IDMask)<<1 : (~IDMask) << 19);
	context->ctrl->filters[context->ctrl->numFilters].canID |=   extID ? (1<<30) : 0;
	context->ctrl->filters[context->ctrl->numFilters].canID |=   rtr   ? (1<<31) : 0;
	context->ctrl->filters[context->ctrl->numFilters].canIDmask |= 0x3<<30;
	context->ctrl->filters[context->ctrl->numFilters].owner=this;
	context->ctrl->filters[context->ctrl->numFilters].owner_context=this->context;
	context->ctrl->numFilters++;

	bool result;
	result = context->ctrl->setupFilters();

	can_exit_freeze(context->ctrl->can);
	context->ctrl->CANCtrlProtector.leave();
	return result;
}

int HAL_CAN::write(const char* sendBuf, int len,uint32_t canID,  bool extID, bool rtr){
	CanMsg msg;
	int result=0;
	if(len > 8) return -1;

	msg.cs=SRR_BIT;
	if(extID){
		msg.id=canID;
		msg.cs |= IDE_BIT;
	}else{
		msg.id= canID << 18;
	}
	if (rtr){
		PRINTF("WARNING: Sending Remote frames not supoorted by this HAL\n");
	}
	msg.id &= 0x1FFFFFFF; //Mask ID to 29 bits
	msg.cs |= (len&0xF) << DLC_SHIFT;
	memcpy(msg.data,sendBuf,len);


	context->ctrl->CANCtrlProtector.enter();
	can_disable_mb_interrupt(context->ctrl->can,TX_MAILBOX);

	if(!context->ctrl->txInProgrss){
		context->ctrl->putIntoTxMailbox(msg);
	}else{
		result=context->ctrl->txFifo.put(msg) ? 0 : -1;
	}

	can_enable_mb_interrupt(context->ctrl->can,TX_MAILBOX);
	context->ctrl->CANCtrlProtector.leave();

	return result;
}

int HAL_CAN::read(char* recBuf, uint32_t* canID, bool* isExtID,bool* rtr){

	CanMsg msg;
	if(context->RxFifo.get(msg)){
		uint32_t len = (msg.cs >> DLC_SHIFT) & 0xF;
		if((msg.cs & IDE_BIT) != 0){
			if(canID){
				*canID=msg.id & 0x1FFFFFFF;
			}
			if(isExtID){
				*isExtID=true;
			}
		}else{
			if(canID){
				*canID=(msg.id >> 18) & 0x7FF;
			}
			if(isExtID){
				*isExtID=false;
			}
		}
		if(rtr){
			*rtr= ((msg.cs & RTR_BIT) != 0);
		}
		memcpy(recBuf,msg.data,len);
		return len;
	}

	return -1;
}

extern "C"{
	void CAN1_IRQHandler(){ CANs[0].IRQHandler(); }
	void CAN2_IRQHandler(){	CANs[1].IRQHandler(); }
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
