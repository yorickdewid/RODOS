
#include "hal/hal_can.h"

#include "rodos.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


HAL_CAN::HAL_CAN(CAN_IDX canIdx, GPIO_PIN rxPin , GPIO_PIN txPin){

}


int HAL_CAN::init(unsigned int baudrate){
return 0;
}

void HAL_CAN::reset(){
}

int HAL_CAN::config(CAN_PARAMETER_TYPE type, int paramVal){
	return -1;
}


int  HAL_CAN::status(CAN_STATUS_TYPE type){
	return -1;

}


bool  HAL_CAN::isWriteFinished(){
	return true;

}

bool  HAL_CAN::isDataReady(){
	return false;
}

bool  HAL_CAN::addIncomingFilter(uint32_t ID, uint32_t IDMask, bool extID, bool rtr){
		return false;
}




int HAL_CAN::write(const char* sendBuf, int len,uint32_t canID,  bool extID, bool rtr){

	return 0;
}



int HAL_CAN::read(char* recBuf, uint32_t* canID, bool* isExtID,bool* rtr){
	return -1;
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
