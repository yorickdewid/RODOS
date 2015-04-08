#include <new>
#include "rodos.h"
#include "hal/hal_gpio.h"

#include "gpio_v2.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "hw_types.h"
#include "interrupt.h"
#include "pin_mux.h"


/*
 GPIO Pin Mappoing for Beaglebone

GPIO_IDX    GPIO Pin

GPIO_000	GPIO_0_0
GPIO_001	GPIO_0_1
GPIO_002	GPIO_0_2
GPIO_003	GPIO_0_3
GPIO_004	GPIO_0_4
GPIO_005	GPIO_0_5
GPIO_006	GPIO_0_6
GPIO_007	GPIO_0_7
GPIO_008	GPIO_0_8
GPIO_009	GPIO_0_9
GPIO_010	GPIO_0_10
GPIO_011	GPIO_0_11
GPIO_012	GPIO_0_12
GPIO_013	GPIO_0_13
GPIO_014	GPIO_0_14
GPIO_015	GPIO_0_15
GPIO_016	GPIO_0_16
GPIO_017	GPIO_0_17
GPIO_018	GPIO_0_18
GPIO_019	GPIO_0_19
GPIO_020	GPIO_0_20
GPIO_021	GPIO_0_21
GPIO_022	GPIO_0_22
GPIO_023	GPIO_0_23
GPIO_024	GPIO_0_24
GPIO_025	GPIO_0_25
GPIO_026	GPIO_0_26
GPIO_027	GPIO_0_27
GPIO_028	GPIO_0_28
GPIO_029	GPIO_0_29
GPIO_030	GPIO_0_30
GPIO_031	GPIO_0_31
GPIO_032	GPIO_1_0
GPIO_033	GPIO_1_1
GPIO_034	GPIO_1_2
GPIO_035	GPIO_1_3
GPIO_036	GPIO_1_4
GPIO_037	GPIO_1_5
GPIO_038	GPIO_1_6
GPIO_039	GPIO_1_7
GPIO_040	GPIO_1_8
GPIO_041	GPIO_1_9
GPIO_042	GPIO_1_10
GPIO_043	GPIO_1_11
GPIO_044	GPIO_1_12
GPIO_045	GPIO_1_13
GPIO_046	GPIO_1_14
GPIO_047	GPIO_1_15
GPIO_048	GPIO_1_16
GPIO_049	GPIO_1_17
GPIO_050	GPIO_1_18
GPIO_051	GPIO_1_19
GPIO_052	GPIO_1_20
GPIO_053	GPIO_1_21
GPIO_054	GPIO_1_22
GPIO_055	GPIO_1_23
GPIO_056	GPIO_1_24
GPIO_057	GPIO_1_25
GPIO_058	GPIO_1_26
GPIO_059	GPIO_1_27
GPIO_060	GPIO_1_28
GPIO_061	GPIO_1_29
GPIO_062	GPIO_1_30
GPIO_063	GPIO_1_31
GPIO_064	GPIO_2_0
GPIO_065	GPIO_2_1
GPIO_066	GPIO_2_2
GPIO_067	GPIO_2_3
GPIO_068	GPIO_2_4
GPIO_069	GPIO_2_5
GPIO_070	GPIO_2_6
GPIO_071	GPIO_2_7
GPIO_072	GPIO_2_8
GPIO_073	GPIO_2_9
GPIO_074	GPIO_2_10
GPIO_075	GPIO_2_11
GPIO_076	GPIO_2_12
GPIO_077	GPIO_2_13
GPIO_078	GPIO_2_14
GPIO_079	GPIO_2_15
GPIO_080	GPIO_2_16
GPIO_081	GPIO_2_17
GPIO_082	GPIO_2_18
GPIO_083	GPIO_2_19
GPIO_084	GPIO_2_20
GPIO_085	GPIO_2_21
GPIO_086	GPIO_2_22
GPIO_087	GPIO_2_23
GPIO_088	GPIO_2_24
GPIO_089	GPIO_2_25
GPIO_090	GPIO_2_26
GPIO_091	GPIO_2_27
GPIO_092	GPIO_2_28
GPIO_093	GPIO_2_29
GPIO_094	GPIO_2_30
GPIO_095	GPIO_2_31
GPIO_096	GPIO_3_0
GPIO_097	GPIO_3_1
GPIO_098	GPIO_3_2
GPIO_099	GPIO_3_3
GPIO_100	GPIO_3_4
GPIO_101	GPIO_3_5
GPIO_102	GPIO_3_6
GPIO_103	GPIO_3_7
GPIO_104	GPIO_3_8
GPIO_105	GPIO_3_9
GPIO_106	GPIO_3_10
GPIO_107	GPIO_3_11
GPIO_108	GPIO_3_12
GPIO_109	GPIO_3_13
GPIO_110	GPIO_3_14
GPIO_111	GPIO_3_15
GPIO_112	GPIO_3_16
GPIO_113	GPIO_3_17
GPIO_114	GPIO_3_18
GPIO_115	GPIO_3_19
GPIO_116	GPIO_3_20
GPIO_117	GPIO_3_21
GPIO_118	GPIO_3_21
GPIO_119	GPIO_3_21
GPIO_120	GPIO_3_21
GPIO_121	GPIO_3_21
GPIO_122	GPIO_3_21
GPIO_123	GPIO_3_21
GPIO_124	GPIO_3_21
GPIO_125	GPIO_3_21
GPIO_126	GPIO_3_21
GPIO_127	GPIO_3_21
*/



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


int getGPIOInstanceNo(uint32_t instanceAddr){
	switch(instanceAddr){
	case SOC_GPIO_0_REGS: return 0;
	case SOC_GPIO_1_REGS: return 1;
	case SOC_GPIO_2_REGS: return 2;
	case SOC_GPIO_3_REGS: return 3;
	default: return -1;
	}
}


class HW_HAL_GPIO {
    friend class HAL_GPIO;

public:
    HAL_GPIO* hal_gpio;
    uint8_t pinIdx;
    uint8_t numOfPins;
    uint32_t gpioInstance;
    volatile bool interruptEventOcured;

public:
	HW_HAL_GPIO(){}

	void IRQHandler(){
		interruptEventOcured=true;
		hal_gpio->upCallDataReady();
	}
};

void GPIO_IRQHandler(int instanceNo,bool isEvenPin);
extern "C" {
	void GPIO0_IRQHandlerA(){ GPIO_IRQHandler(0,true ); }
	void GPIO0_IRQHandlerB(){ GPIO_IRQHandler(0,false); }
	void GPIO1_IRQHandlerA(){ GPIO_IRQHandler(1,true ); }
	void GPIO1_IRQHandlerB(){ GPIO_IRQHandler(1,false); }
	void GPIO2_IRQHandlerA(){ GPIO_IRQHandler(2,true ); }
	void GPIO2_IRQHandlerB(){ GPIO_IRQHandler(2,false); }
	void GPIO3_IRQHandlerA(){ GPIO_IRQHandler(3,true ); }
	void GPIO3_IRQHandlerB(){ GPIO_IRQHandler(3,false); }
}

static struct {
	uint32_t baseAddr;
	void (*clkConfigFunc)();
	IntHandlerFunc irqhandlerA;
	IntHandlerFunc irqhandlerB;
	uint32_t irqIdA;
	uint32_t irqIdB;

}GPIOInstanceInfo[4]= {
		{SOC_GPIO_0_REGS,&GPIO0ModuleClkConfig,&GPIO0_IRQHandlerA,&GPIO0_IRQHandlerB,SYS_INT_GPIOINT0A,SYS_INT_GPIOINT0B},
		{SOC_GPIO_1_REGS,&GPIO1ModuleClkConfig,&GPIO1_IRQHandlerA,&GPIO1_IRQHandlerB,SYS_INT_GPIOINT1A,SYS_INT_GPIOINT1B},
		{SOC_GPIO_2_REGS,&GPIO2ModuleClkConfig,&GPIO2_IRQHandlerA,&GPIO2_IRQHandlerB,SYS_INT_GPIOINT2A,SYS_INT_GPIOINT2B},
		{SOC_GPIO_3_REGS,&GPIO3ModuleClkConfig,&GPIO3_IRQHandlerA,&GPIO3_IRQHandlerB,SYS_INT_GPIOINT3A,SYS_INT_GPIOINT3B}
};

uint32_t GPIOPadRegs[32][4] = {
		{ GPIO_0_0 , GPIO_1_0 , GPIO_2_0 , GPIO_3_0 },
		{ GPIO_0_1 , GPIO_1_1 , GPIO_2_1 , GPIO_3_1 },
		{ GPIO_0_2 , GPIO_1_2 , GPIO_2_2 , GPIO_3_2 },
		{ GPIO_0_3 , GPIO_1_3 , GPIO_2_3 , GPIO_3_3 },
		{ GPIO_0_4 , GPIO_1_4 , GPIO_2_4 , GPIO_3_4 },
		{ GPIO_0_5 , GPIO_1_5 , GPIO_2_5 , GPIO_3_5 },
		{ GPIO_0_6 , GPIO_1_6 , GPIO_2_6 , GPIO_3_6 },
		{ GPIO_0_7 , GPIO_1_7 , GPIO_2_7 , GPIO_3_7 },
		{ GPIO_0_8 , GPIO_1_8 , GPIO_2_8 , GPIO_3_8 },
		{ GPIO_0_9 , GPIO_1_9 , GPIO_2_9 , GPIO_3_9 },
		{ GPIO_0_10 , GPIO_1_10 , GPIO_2_10 , GPIO_3_10 },
		{ GPIO_0_11 , GPIO_1_11 , GPIO_2_11 , GPIO_3_10 },
		{ GPIO_0_12 , GPIO_1_12 , GPIO_2_12 , GPIO_3_10 },
		{ GPIO_0_13 , GPIO_1_13 , GPIO_2_13 , GPIO_3_13 },
		{ GPIO_0_14 , GPIO_1_14 , GPIO_2_14 , GPIO_3_14 },
		{ GPIO_0_15 , GPIO_1_15 , GPIO_2_15 , GPIO_3_15 },
		{ GPIO_0_16 , GPIO_1_16 , GPIO_2_16 , GPIO_3_16 },
		{ GPIO_0_17 , GPIO_1_17 , GPIO_2_17 , GPIO_3_17 },
		{ GPIO_0_18 , GPIO_1_18 , GPIO_2_18 , GPIO_3_18 },
		{ GPIO_0_19 , GPIO_1_19 , GPIO_2_19 , GPIO_3_19 },
		{ GPIO_0_20 , GPIO_1_20 , GPIO_2_20 , GPIO_3_20 },
		{ GPIO_0_21 , GPIO_1_21 , GPIO_2_21 , GPIO_3_21 },
		{ GPIO_0_22 , GPIO_1_22 , GPIO_2_22 , GPIO_3_21 },
		{ GPIO_0_23 , GPIO_1_23 , GPIO_2_23 , GPIO_3_21 },
		{ GPIO_0_23 , GPIO_1_24 , GPIO_2_24 , GPIO_3_21 },
		{ GPIO_0_23 , GPIO_1_25 , GPIO_2_25 , GPIO_3_21 },
		{ GPIO_0_26 , GPIO_1_26 , GPIO_2_26 , GPIO_3_21 },
		{ GPIO_0_27 , GPIO_1_27 , GPIO_2_27 , GPIO_3_21 },
		{ GPIO_0_28 , GPIO_1_28 , GPIO_2_28 , GPIO_3_21 },
		{ GPIO_0_29 , GPIO_1_29 , GPIO_2_29 , GPIO_3_21 },
		{ GPIO_0_30 , GPIO_1_30 , GPIO_2_30 , GPIO_3_21 },
		{ GPIO_0_31 , GPIO_1_31 , GPIO_2_31 , GPIO_3_21 }
};

HW_HAL_GPIO* GPIOs[4][32];

void GPIOInstanceInit(int instanceNo){
	static bool initialized[4];
	if(!initialized[instanceNo]){
		initialized[instanceNo]=true;

		GPIOInstanceInfo[instanceNo].clkConfigFunc();

		IntRegister(GPIOInstanceInfo[instanceNo].irqIdA, GPIOInstanceInfo[instanceNo].irqhandlerA);
		IntPrioritySet(GPIOInstanceInfo[instanceNo].irqIdA, 0, AINTC_HOSTINT_ROUTE_IRQ);
		IntRegister(GPIOInstanceInfo[instanceNo].irqIdB, GPIOInstanceInfo[instanceNo].irqhandlerB);
		IntPrioritySet(GPIOInstanceInfo[instanceNo].irqIdB, 0, AINTC_HOSTINT_ROUTE_IRQ);

		uint32_t baseAddr = GPIOInstanceInfo[instanceNo].baseAddr;
		GPIOModuleEnable(baseAddr);
		GPIOModuleReset(baseAddr);

		IntSystemEnable(GPIOInstanceInfo[instanceNo].irqIdA);
		IntSystemEnable(GPIOInstanceInfo[instanceNo].irqIdB);
	}
}

void GPIO_IRQHandler(int instanceNo,bool isEvenPin){
	//Even Pin ->Line1, odd Pin -> Line2

	uint32_t intStatus;
	uint32_t uartInstance = GPIOInstanceInfo[instanceNo].baseAddr;
	if(isEvenPin){
		intStatus = HWREG(uartInstance + GPIO_IRQSTATUS(0));
	}else{
		intStatus = HWREG(uartInstance + GPIO_IRQSTATUS(1)) >> 1;
	}

	for(int i=0;i<16;i++){
		if(intStatus & 1){
			int intNo = isEvenPin ? i*2 : (i*2)+1;
			if(GPIOs[instanceNo][intNo]){
				GPIOs[instanceNo][intNo]->IRQHandler();
				GPIOPinIntClear(uartInstance,isEvenPin ? GPIO_INT_LINE_1 : GPIO_INT_LINE_2,intNo);
			}
		}
		intStatus = intStatus >> 2;
	}

}


HAL_GPIO::HAL_GPIO(GPIO_PIN pinIdx) {
	context = (HW_HAL_GPIO*)xmalloc(sizeof(HW_HAL_GPIO)); // dynamic memory allocation with RODOS function
	context = new (context) HW_HAL_GPIO(); // placement new to call constructor
	context->hal_gpio = this;

    context->interruptEventOcured = false;
    context->numOfPins=0;

    context->pinIdx = ((int)pinIdx) % 32;
    context->gpioInstance=GPIOInstanceInfo[((int)pinIdx) / 32].baseAddr;
}


int32_t HAL_GPIO::init(bool isOutput, uint32_t numOfPins, uint32_t initVal){
	if (numOfPins != 1) return -1;

	if(Scheduler::isSchedulerRunning()){
		PRIORITY_CEILING{
			GPIOInstanceInit(getGPIOInstanceNo(context->gpioInstance));
		}
	}else{
		GPIOInstanceInit(getGPIOInstanceNo(context->gpioInstance));
	}
	GPIOs[getGPIOInstanceNo(context->gpioInstance)][context->pinIdx] = context;

	config(GPIO_CFG_PULLDOWN_ENABLE,0);
	config(GPIO_CFG_IRQ_SENSITIVITY,GPIO_IRQ_SENS_BOTH);
	setPins(initVal);
	config(GPIO_CFG_OUTPUT_ENABLE, isOutput ? 1:0);

	return 0;
}


int32_t HAL_GPIO::config(GPIO_CFG_TYPE type, uint32_t paramVal){
	switch (type){
		case GPIO_CFG_OUTPUT_ENABLE:
			if (paramVal > 0){
				if(Scheduler::isSchedulerRunning()){
					PRIORITY_CEILING{
						GPIODirModeSet(context->gpioInstance,context->pinIdx,GPIO_DIR_OUTPUT);
					}
				}else{
					GPIODirModeSet(context->gpioInstance,context->pinIdx,GPIO_DIR_OUTPUT);
				}
			}
			else {
				if(Scheduler::isSchedulerRunning()){
					PRIORITY_CEILING{
						GPIODirModeSet(context->gpioInstance,context->pinIdx,GPIO_DIR_INPUT);
					}
				}else{
					GPIODirModeSet(context->gpioInstance,context->pinIdx,GPIO_DIR_INPUT);
				}

			}
			return 0;

		case GPIO_CFG_NUM_OF_PINS:
			if (paramVal > 0 && paramVal < 2) { // numOfPins has to be 1
				//context->numOfPins = paramVal;
				return 0;
			}
			return -1;

		case GPIO_CFG_PULLUP_ENABLE:
			if (paramVal > 0){
				if (paramVal){
					GpioPinMuxSetup(GPIOPadRegs[context->pinIdx][getGPIOInstanceNo(context->gpioInstance)],PAD_SL_RXE_PU_PUPDE(7));
				}else{
					GpioPinMuxSetup(GPIOPadRegs[context->pinIdx][getGPIOInstanceNo(context->gpioInstance)],PAD_SL_RXE_NA_PUPDD(7));
				}
				return 0;
			}
			return -1;

		case GPIO_CFG_PULLDOWN_ENABLE:
			if (paramVal > 0){
				if (paramVal){
					GpioPinMuxSetup(GPIOPadRegs[context->pinIdx][getGPIOInstanceNo(context->gpioInstance)],PAD_SL_RXE_PD_PUPDE(7));
				}else{
					GpioPinMuxSetup(GPIOPadRegs[context->pinIdx][getGPIOInstanceNo(context->gpioInstance)],PAD_SL_RXE_NA_PUPDD(7));
				}

				return 0;
			}
			return -1;
		case GPIO_CFG_IRQ_SENSITIVITY:
			int intType;
			switch((RODOS::GPIO_IRQ_SENSITIVITY)paramVal){
			case GPIO_IRQ_SENS_BOTH: intType=GPIO_INT_TYPE_BOTH_EDGE; break;
			case GPIO_IRQ_SENS_RISING: intType=GPIO_INT_TYPE_RISE_EDGE; break;
			case GPIO_IRQ_SENS_FALLING: intType=GPIO_INT_TYPE_FALL_EDGE; break;
			default: return -1;
			}

			if(Scheduler::isSchedulerRunning()){
				PRIORITY_CEILING{
					GPIOIntTypeSet(context->gpioInstance,context->pinIdx,GPIO_INT_TYPE_NO_EDGE);
					GPIOIntTypeSet(context->gpioInstance,context->pinIdx,intType);
				}
			}else{
				GPIOIntTypeSet(context->gpioInstance,context->pinIdx,GPIO_INT_TYPE_NO_EDGE);
				GPIOIntTypeSet(context->gpioInstance,context->pinIdx,intType);
			}
			return 0;

		default: return -1;
	}
}


void HAL_GPIO::reset(){
	interruptEnable(false);
    config(GPIO_CFG_OUTPUT_ENABLE, 0);
    config(GPIO_CFG_PULLDOWN_ENABLE,0);
    setPins(0);
    GPIOs[getGPIOInstanceNo(context->gpioInstance)][context->pinIdx] = 0;
    context->interruptEventOcured = false;
    context->numOfPins=0;
}


void HAL_GPIO::setPins(uint32_t val) {
    if(val) {
        HWREG(context->gpioInstance + GPIO_SETDATAOUT  ) = (1 << context->pinIdx);
    } else{
    	HWREG(context->gpioInstance + GPIO_CLEARDATAOUT) = (1 << context->pinIdx);
    }
}


uint32_t HAL_GPIO::readPins(){
	return HWREG(context->gpioInstance + GPIO_DATAIN) & (1 << context->pinIdx) ? 1:0;
}


bool HAL_GPIO::isDataReady(){
	return context->interruptEventOcured;
}

//When rising and/or falling edge occures dataReady() == true
void HAL_GPIO::interruptEnable(bool enable){
	uint32_t intLine = (context->pinIdx & 1) ? GPIO_INT_LINE_2 : GPIO_INT_LINE_1; //Even Pin ->Line1, odd Pin -> Line2
	if(enable){
		GPIOPinIntEnable(context->gpioInstance,intLine,context->pinIdx);
	}else{
		GPIOPinIntDisable(context->gpioInstance,intLine,context->pinIdx);
	}
}

void HAL_GPIO::resetInterruptEventStatus(){
	context->interruptEventOcured=false;
}


#ifndef NO_RODOS_NAMESPACE
}
#endif





