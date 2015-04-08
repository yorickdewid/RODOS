#ifndef _FLASH_STM32F4_H_
#define _FLASH_STM32F4_H_



class INTFlash_STM32F4 : public Flash {
public: 
    static const uint32_t NUMBER_OF_PAGES  = 12;

private:

public:
    INTFlash_STM32F4();
    int32_t getNumberOfPages();
    int32_t getPageSize(uint32_t pageIdx);

    void * getMemoryPointer(uint32_t pageIdx);
    void * getMemoryPointerOffset(uint32_t pageIdx,uint32_t offsetInPage);
    int32_t erasePage(uint32_t pageIdx);
    int32_t readPageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* destination, uint32_t maxLen);
    int32_t writePageOffset(uint32_t pageIdx, uint32_t offsetInPage, void* source, uint32_t maxLen);
    int32_t eraseAndWritePage(uint32_t pageIdx, void* source, uint32_t maxLen);

    int32_t read(uint32_t address, void* destination, uint32_t maxLen);
    int32_t write(uint32_t address, void* source, uint32_t maxLen);

    void init();
};


const uint32_t stm32f4_flash_startAddress[INTFlash_STM32F4::NUMBER_OF_PAGES]  = {0x08000000 + 0      ,0x08000000 + 16384  ,0x08000000 + 32768  ,0x08000000 + 49152
                                                  ,0x08000000 + 65536  ,0x08000000 + 131072 ,0x08000000 + 262144 ,0x08000000 + 393216
                                                  ,0x08000000 + 524288 ,0x08000000 + 655360 ,0x08000000 + 786432 ,0x08000000 + 917504 };
const uint32_t stm32f4_flash_pageSize[INTFlash_STM32F4::NUMBER_OF_PAGES]      = {16384 ,16384  ,16384  ,16384  ,65536  ,131072 ,131072 ,131072 ,131072 ,131072 ,131072 ,131072};



#endif /* _FLASH_STM32F4_H_ */
