/**
 * Protected version of the RODOS FIFO.
 *
 * The critical region functions are used for protection.
 * \file protectedFifo.h
 *
 *  \date 11.04.2014
 *  \author wittkampm
 */

#ifndef PROTECTEDFIFO_H_
#define PROTECTEDFIFO_H_

#include <rodos.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

// redeclaration of the critical region functions. The original one is in hw_interrupts.h but that wasn't accepted by the compiler
void system_enter_critical_region(void *pArg);
void system_exit_critical_region(void *pArg);

template <typename T, int len > class ProtectedFifo : Fifo < T, len > {
public:
	/**  returns true == ok, false == fifo full */
	bool putProtected(const T& val) {
		bool res;
		void* criticalArg;
		system_enter_critical_region(criticalArg);
		res = this->put(val);
		system_exit_critical_region(criticalArg);
		return res;
	}
	/** return true == ok, false = fifo empty, val not modified */
	bool getProtected(T& val) {
		bool res;
		void* criticalArg;
		system_enter_critical_region(criticalArg);
		res = this->get(val);
		system_exit_critical_region(criticalArg);
		return res;
	}
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* PROTECTEDFIFO_H_ */
