/***********************************************************************************************//**
 * \ingroup HK
 * \file MBox.h
 * \version 0.1
 * \date 16.04.2014
 * \author Georg Raith
 *
 * \brief 
 * \b History: \n
 *
 * 16.04.2014 rg: initial version
 */

#ifndef MBOX_H_
#define MBOX_H_

#include <rodos_system_init.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


enum RetMbox {
	MBoxNoErr,
	MBoxFull,
	MBoxEmpty,
	MBoxTimeOut,
	MBoxErrArg,
	MBoxErrRegion,
};

template <typename T, int count>
class MBox {
	T *data[count];
	u32 ReadCounter;
	u32 WriteCounter;
	u32 NumberOfElements;

	void wakup(bool isWriteCounter) {
		void *signaler = isWriteCounter ?  &this->WriteCounter : &this->ReadCounter;
		Thread *waiter;

		waiter = Thread::findNextWaitingFor(signaler);
		if (waiter != NULL) {
			if (isInterrupt() || isCritRegion() ) waiter->resume();
			else waiter->resumeAndYield();
		}
	}

public:
	RetMbox Post(T *pdata){
		RetMbox ret = MBoxNoErr;
		Thread *waiter;

		if (pdata == NULL) return MBoxErrArg;

		system_enter_critical_region(0);
		if (this->NumberOfElements >= count) {
			ret = MBoxFull;
		}
		else {
			this->data[this->WriteCounter] = pdata;
			this->NumberOfElements++;
			this->WriteCounter = (this->WriteCounter+1)%count;
		}
		system_exit_critical_region(0);

		if (ret == MBoxNoErr) {
			this->wakup(true);
		}
		return ret;
	}
	RetMbox PostWait(TTime timeout, T *data ) {
		Thread *caller;
		if (isCritRegion() || isInterrupt()) {
			xprintf("Suspend in Crit Region Post Waith\n");
			return MBoxErrRegion;
		}
		caller = Thread::getCurrentThread();
		do {
			if (this->Post(data) == MBoxNoErr) {
				if (this->NumberOfElements < count) this->wakup(true);
				return MBoxNoErr;
			}
		} while (caller->suspendCallerUntil(timeout, &this->ReadCounter)) ;
		return MBoxTimeOut;
	}

	RetMbox Accept(T** pdata) {
		RetMbox ret = MBoxNoErr;
		Thread *waiter;

		//check the argument;
		if (pdata == NULL) return MBoxErrArg;


		system_enter_critical_region(0);
		if (this->NumberOfElements == 0) {
			ret = MBoxEmpty;
		}
		else {
			*pdata = this->data[this->ReadCounter];
			this->ReadCounter = (this->ReadCounter+1)%count;
			this->NumberOfElements--;

		}
		system_exit_critical_region(0);

		if (ret == MBoxNoErr) {
			this->wakup(false);
		}
		return ret;
	}

	RetMbox Pend(TTime timeout, T**pdata){
		Thread *caller;
		if (pdata == NULL) return MBoxErrArg;
		if (isCritRegion() || isInterrupt()) {
			xprintf("Suspend in Crit Region Pend Waith\n");
			return MBoxErrRegion;
		}
		caller = Thread::getCurrentThread();

		do {
			if (this->Accept(pdata) == MBoxNoErr) {
				if (this->NumberOfElements > 0) this->wakup(false);
				return MBoxNoErr;//Mbox Was Full and is Now Processed
			}
		} while (caller->suspendCallerUntil(timeout, &this->WriteCounter)); //false if timeout

		return MBoxTimeOut;
	}//blocking

	RetMbox Query(){
		switch(this->NumberOfElements) {
		case 0:
			return MBoxEmpty;
		case count-1:
			return MBoxFull;
		default:
			return MBoxNoErr;
		}
		return MBoxNoErr;
	}

	MBox():
	ReadCounter(0), WriteCounter(0), NumberOfElements(0)
	{
		for (u32 i= 0; i < count; i++)
			this->data[i] = NULL;
	}
	virtual ~MBox() {}

	u8 getNumberOfElements() const {
		return NumberOfElements;
	}
	u8 getNumberOfFreeElements() const {
		return count - NumberOfElements;
	}
};

#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* MBOX_H_ */
