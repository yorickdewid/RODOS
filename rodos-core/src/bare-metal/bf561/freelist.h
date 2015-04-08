/***********************************************************************************************//**
 * \ingroup system
 * \file freelist.h
 * \version 0.1
 * \date 08.04.2014
 * \author Georg Raith
 *
 * \brief Implementation of a FreeList.
 * \b History: \n
 *
 * 08.04.2014 rg: initial version
 */

#ifndef BUFFERMANAGEMENT_H_
#define BUFFERMANAGEMENT_H_

#include <new>
#include <rodos.h>
#include <hw_interrupt.h>

enum FreeListError{
	BufferNoError,
	BufferFull,
	BufferAddressNotAligned,
	BufferFreeFailed,
	BufferAlreadyFree,
};




template <typename T, const int MemFragments>
class FreeList {
	u32 unused[MemFragments];
	s32 free;
	const T * StartAddress;

protected:
	virtual T* BuildObject(const T* address) {
		return new ((void *) address) T();
	}

public:
	FreeList(T *start) :
		StartAddress(start)
	{
		u32 i;

		this->free = MemFragments-1;
		for (i=0; i< MemFragments ; i++) {
			this->unused[i] = i;
		}

	}
	virtual ~FreeList() {}

	T * Get() {
		u32 BufferNumber;

		system_enter_critical_region(0);
			if ((this->free < 0) || (this->free >= MemFragments)) {
				system_exit_critical_region(0);
				return NULL;
			}
			BufferNumber = this->unused[this->free];
			this->free--;
		system_exit_critical_region(0);

		return this->BuildObject(&(this->StartAddress[BufferNumber]));
	}

	FreeListError Put(T *address) {

		u32 addr;
		u32 bufferNumber;
		s32 i;
		addr = (u32) address;
		addr -= (u32)this->StartAddress;
		bufferNumber = addr/sizeof(T);

		if (addr%sizeof(T)) return BufferAddressNotAligned;


		if (bufferNumber < 0 || bufferNumber > MemFragments) {
			return BufferFreeFailed;
		}

		system_enter_critical_region(0);

		for (i = 0; i <= this->free; i++) {
			if (this->unused[i] == bufferNumber) {
				system_exit_critical_region(0);
				return BufferAlreadyFree;
			}
		}
		if ((this->free < -1) || (this->free >= MemFragments-1)) {
			system_exit_critical_region(0);
			return BufferFreeFailed;
		}
		this->free++;
		this->unused[this->free] = bufferNumber;
#warning find way to remove it from critical region
		address->~T();
		system_exit_critical_region(0);

		return BufferNoError;
	}

	const u32 getFragmentSize() const {
		return sizeof(T);
	}

	const u32 getMemFragments() const {
		return MemFragments;
	}
};


#endif /* BUFFERMANAGEMENT_H_ */
