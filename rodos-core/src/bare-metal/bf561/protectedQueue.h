/**
 * \brief Queue management, safe for ISR and Threading.
 * \details The queue template might be used to move elements from interrupt to thread context and vice versa.
 * It uses an integrated free-list for management of the linked list.
 *
 * The typename and the maximum length is need in order to build the object.
 *
 * \file protectedQueue.h
 *
 * \date 16.04.2014
 * \author wittkampm
 */

#ifndef PROTECTEDQUEUE_H_
#define PROTECTEDQUEUE_H_

#include <rodos.h>
#include <hw_interrupt.h>
#include <freelist.h>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 *
 */
template <typename T, const int maxLen>
class ProtectedQueue {
protected:
	/**
	 * Structure of meta information.
	 */
	struct Tl {
		T* dat;	/// data element
		Tl* next;	/// pointer to the next element
	};

	Tl contextBuffer[maxLen];			/// buffer to store the meta information
	FreeList <Tl, maxLen> contextFreelist;	/// freelist for buffer managment
	int count;	/// count of elements currently stored in the queue
	Tl* head;	/// pointer to the head-end of the queue
	Tl* tail;	/// pointer to the last element of the queue

public:
	/**
	 * Standard constructor for the queue.
	 */
	ProtectedQueue () : contextFreelist(contextBuffer)
	{
		this->count = 0;
		this->head = NULL;
		this->tail = NULL;
	}
	/**
	 * Standard destructor for the queue.
	 */
	virtual ~ProtectedQueue() {}

	/**
	 * Append another element to the queue.
	 * Since the queue is using a free-list for storing meta-information the maximum size of the queue is limited to maxLen given during construction.
	 * @param e element to add to the queue
	 * @return true if the operation was successful, false if the queue is full
	 */
	virtual bool addToQueue(T* e) {
		void* criticalArg;
		Tl* ce = contextFreelist.Get();
		if (ce != NULL) {
			ce->dat = e;
			ce->next = NULL;
			system_enter_critical_region(criticalArg);
			if (this->head == NULL) {
				this->head = ce;
			} else {
				this->tail->next = ce;
			}
			this->tail = ce;
			this->count++;
			system_exit_critical_region(criticalArg);
			return true;
		} else return false;
	}

	/**
	 * @return the oldest element of the queue or NULL if the queue is empty
	 */
	virtual T* getNext() {
		void* criticalArg;
		T* e = NULL;
		Tl* ce = NULL;
		system_enter_critical_region(criticalArg);
		if (this->count > 0) {
			ce = this->head;
			this->head = ce->next;
			this->count--;
			system_exit_critical_region(criticalArg);
			e = ce->dat;
			this->contextFreelist.Put(ce);
		} else {
			system_exit_critical_region(criticalArg);
		}
		return e;
	}

	/**
	 * Search through the list and remove an object somewhere in the queue.
	 * \note This is a O(n) operation!
	 * @param e the object to search
	 * @return true if the object was found, false if the queue doesn't contain the object.
	 */
	virtual bool remove(const T* e) {
		Tl* ce = this->head;
		Tl* ced = this->head;
		bool ret = false;
		void* criticalArg;
		system_enter_critical_region(criticalArg);
		if (this->count > 0) {
			// queue contains at least one element
			// ce is pointing to the element to check,
			// ced to the previous element
			while ((ce != NULL) && (ce->dat != e)) {
				ced = ce;
				ce = ce->next;
			}
			if (ce != NULL) {
				if (this->count > 1) {
					// the element was found and ce is pointing to it
					// remove the element from the list
					ced->next = ce->next;
					ce->next = NULL;
					this->count--;
				} else {
					this->head = NULL;
					this->tail = NULL;
					this->count = 0;
				}
				ret = true;
			}
		}
		system_exit_critical_region(criticalArg);
		if (ce != NULL) this->contextFreelist.Put(ce);
		return ret;
	}
	/**
	 * Read the contents of the head element but don't remove it.
	 * @return pointer to the head element of the queue
	 */
	virtual T* readNext() {
		return this->head->dat;
	}

	/**
	 * flush the queue, remove all elements
	 */
	virtual void flush() {
		while (this->getNext() != NULL);
	}

	/**
	 * @return the length of the queue, the count of elements
	 */
	virtual int getCount() const { return this->count; }

};


#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif /* PROTECTEDQUEUE_H_ */
