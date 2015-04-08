/*
 * sd_tests.h
 *
 *  Created on: 10.09.2012
 *      Author: ruffer
 */

#ifndef SD_TESTS_H_
#define SD_TESTS_H_


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      10  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)

#define SD_OPERATION_ERASE          0
#define SD_OPERATION_BLOCK          1
#define SD_OPERATION_MULTI_BLOCK    2
#define SD_OPERATION_END            3


void NVIC_Configuration(void);
void SD_EraseTest(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* SD_TESTS_H_ */
