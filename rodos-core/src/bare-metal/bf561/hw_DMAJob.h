/**
 * \brief Declaration of DMA Jobs.
 * \details
 * The Blackfin DMA can be used to copy large blocks of data between the various memory sections.
 * For Details read the Processor Handbook Chapter 9.
 *
 * From a user point of view several DMAJobs of given length can be chained together to form
 * a single job. Each one might declare its own callback function which is called when the
 * DMA hardware finishes the last job of the chain.
 *
 * The DMAJob is just a transfer object, able to keep the meta-information for the DMA hardware.
 * The application will provide the payload data for this transfer.
 * It has to be linked to the start- / destination-address, depending on the direction of transfer.
 *
 * For usage of DMA the application has to keep
 * -# a number of DMAJobs statically,
 * -# has to call the init method of all Jobs to prepare the transfer and link the chain,
 * -# prepare the message itself,
 * -# call the addJob() method of the channel to add the job to the channel queue.
 *
 * The channel will process the jobs and executes the callback at the end.
 * The callback is of type
 * \code u32 myCallback(isr_callback_event event, void* arg) \endcode
 * The event should be evaluated to detect errors during the DMA process -
 * most likely due to programming errors of missaligned addresses, etc.
 *
 * The argument to the callback is the parameter arg of the init method.
 * It is recommenced to generate a static callback function which is registered with init.
 * Use the processing thread for the thread-argument and and the DMAJob as argument.
 * The static function might call some kind of void process(event, DMAJob) - function
 * to resume the thread and copy data to variables in the thread stack.
 *
 * \attention Be careful with data located in cachable memory regions!
 *
 * The DMA controller is bypassing the Memory Managing Unit and accessing those areas directly.
 * If your data is located in such an area you have to ensure the data is
 * synchronised with the original location which is given with init!
 *
 * If your thread wants to read data which is copied to a cachable area you also have
 * to synchronise the data to the cache in L1 before using it.
 *
 * To synchronise L1-Data down to SDRAM use the mfc_FlushCacheArea function.
 * The way back is done by declaring the cache line to be invalid by using mfc_FlushInvalidateCacheArea.
 *
 * \file hw_DMAJob.h
 * \author mw
 * \date 17.03.2014
 *
 */

#ifndef HW_DMAJOB_H_
#define HW_DMAJOB_H_

#include "hw_dma.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * define the transfer-width as power of 2
 * Don't change, software depends on it!
 */
typedef enum {
	system_dma_transfer_1_byte = 0,
	system_dma_transfer_2_byte = 1,
	system_dma_transfer_4_byte = 2
} system_dma_transferwidth;

struct  __attribute__((packed)) dma_descriptor {
	struct dma_descriptor *next_desriptor;
	void* start_address;
	u16 dma_config;
	u16 xcount;
	s16 xmodify;
	u16 ycount;
	s16 ymodify;
};

class DMAJob {

protected:
	Thread* owner;
	void* callbackArg;
	dma_descriptor descSource;
	dma_descriptor descDest;
	DMAJob* nextInLine;
	DMAJob* nextInQueue;
	bool processed;
	bool error;
	isr_callback* appCallback;
public:
	DMAJob();
	/**
	 * Constructor for a Memory-DMA job to copy 1D or 2D buffers.
	 * @param next pointer to the next job
	 * @param appCallback callback of the application
	 * @param arg application specific information
	 * @param start_addr_source address of the source buffer to copy
	 * @param start_addr_destination address of the dma destination
	 * @param xcount count of elements in x-direction (columns)
	 * @param ycount count of elements in y-direction (rows) - left empty or set to 0 for 1D copy
	 * @param width transfer width for this job
	 * @param owner thread who is responsible for this job
	 */
	virtual void init(
			Thread* owner,
			DMAJob* next,
			isr_callback* appCallback,
			void* arg,
			void* start_addr_source,
			void* start_addr_destination,
			system_dma_transferwidth width,
			u16 xcount,
			u16 ycount = 1
			);


	/**
	 * Constructor for a Memory-DMA job to copy 1D or 2D buffers.
	 * @param next pointer to the next job
	 * @param appCallback callback of the application
	 * @param arg application specific information
	 * @param start_addr_source address of the source buffer to copy
	 * @param start_addr_destination address of the dma destination
	 * @param xcount count of elements in x-direction (columns)
	 * @param ycount count of elements in y-direction (rows) - left empty or set to 0 for 1D copy
	 * @param width transfer width for this job
	 * @param owner thread who is responsible for this job
	 * @param xsourcemodify x source pointer is incremented by this value
	 * @param xdestmodify   x dest pointer is incremented by this value
	 * @param ysourcemodify y source pointer is incremented by this value
	 * @param ydestmodify   y dest pointer is incremented by this value
	 */
	virtual void init(
			Thread* owner,
			DMAJob* next,
			isr_callback* appCallback,
			void* arg,
			void* start_addr_source,
			void* start_addr_destination,
			system_dma_transferwidth width,
			u16 xcount,
			u16 ycount,
			s16 xsourcemodify,
			s16 xdestmodify,
			s16 ysourcemodify,
			s16 ydestmodify
			);


	/**
	 * Get the internal source DMA descriptor.
	 * @return the internal descriptor
	 */
	dma_descriptor* getSourceDescriptor() const;
	volatile void* getSourceDescriptorAddr() const;
	/**
	 * Get the internal destination DMA descriptor.
	 * @return the internal descriptor
	 */
	dma_descriptor* getDestinationDescriptor() const;
	volatile void* getDestinationDescriptorAddr() const;
	Thread* getOwner() const;
	/**
	 * get the state of the object - processed or not
	 * @return the current processed state
	 */
	bool isProcessed() const;
	/**
	 * Set the object to processed state.
	 */
	void setProcessed();
	/**
	 * Reset the processed state of the object.
	 */
	void resetProcessed();
	/**
	 * get the error state of the object.
	 * \note the error state is if an error occurred during DMA processing, usually due to wrong descriptor settings.
	 * @return the error state
	 */
	bool isError() const;
	/**
	 * set to error state
	 */
	void setError();
	/**
	 * reset the error state
	 */
	void resetError();
	/**
	 * append another DMAJob to this one to create a linked list of DMA descriptors.
	 * @param next dma job to append to the list
	 */
	virtual void append(DMAJob* next);
	/**
	 * get the next Object in queue
	 * @return the next Object in queue
	 */
	virtual DMAJob* getNextInQueue() const;
	/**
	 * get the next Object in line
	 * @return the next Object in line
	 */
	virtual DMAJob* getNextInLine() const;
	/**
	 * set the arguement object to be the next object in queue
	 * @param queue
	 */
	virtual void setNextInQueue(DMAJob* queue);
	/**
	 * execute Application Callback
	 */
	virtual void executeCallback();

	virtual int getXCount() const;
	virtual int getYCount() const;
	virtual int getSize() const;

	virtual void flushDescriptor();
	virtual void flushDescriptorLine();
	virtual void flushSource();
	virtual void flushSourceLine();
	virtual void flushDestination();
	virtual void flushDestinationLine();
	virtual void flushInvalidateSource();
	virtual void flushInvalidateSourceLine();
	virtual void flushInvalidateDestination();
	virtual void flushInvalidateDestinationLine();


	/**
	 * standard destructor
	 */
	virtual ~DMAJob();
};

#ifndef NO_RODOS_NAMESPACE
}
#endif
#endif
