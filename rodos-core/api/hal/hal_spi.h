/**
 * @file    hal_spi.h
 *
 * @date    24.04.2013
 * @author  Michael Ruffer, Claudio S...
 *
 * @brief   Simple API to configure and use SPI interfaces
 */

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

#include <stdint.h>
#include "genericIO.h"
#include "hal_gpio.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

enum SPI_IDX{   //  STM32F4
	SPI_IDX0,   //  not available
	SPI_IDX1,   //  SPI1
	SPI_IDX2,   //  SPI2
	SPI_IDX3,    //   SPI3
	SPI_IDX4,   //  SPI4 (STM32F429 only)
	SPI_IDX5,   //  SPI5 (STM32F429 only)
	SPI_IDX6    //  SPI6 (STM32F429 only)
};

enum SPI_PARAMETER_TYPE {
    SPI_PARAMETER_BAUDRATE,
    SPI_PARAMETER_MODE      // MODE:CPOL/CPHA  0:0/0   1:0/1   2:1/0   3:1/1
};

enum SPI_STATUS_TYPE {
    SPI_STATUS_BAUDRATE
};


class HW_HAL_SPI;

class HAL_SPI : public GenericIOInterface {
	HW_HAL_SPI *context;
public:
    HAL_SPI(SPI_IDX spiIdx);
    HAL_SPI(SPI_IDX idx, GPIO_PIN sckPin, GPIO_PIN misoPin, GPIO_PIN mosiPin);

    /**
     * @brief   Initialization of SPI interface & pins
     *          Baudrate is set to the closest possible.
     *          Request configured baudrate with status().
     * @return  0 on success, -1 on failure
     */
    int32_t init(uint32_t baudrate = 1000000);

    /**
     *  @brief  Disable interface and set all its registers and pins to its reset state.
     *          Before using the interface again you must call init().
     */
    void reset();

    /**
     * @brief   Configuration of SPI interface AFTER initialization
     * @return  0 on success, -1 on failure
     */
    int32_t config(SPI_PARAMETER_TYPE type, int32_t value);

    /**
     * @brief   request status of interface e.g. baudrate
     * @return  requested status value, -1 on failure
     */
    int32_t status(SPI_STATUS_TYPE type);

    /**
     * @brief   request interface sent status
     * @return  true -> new transmit can be started, false -> busy
     */
    bool isWriteFinished();

    /**
     * @brief   request interface receive status
     * @return  true -> previously started receive request is finished, false -> busy
     */
    bool isReadFinished();

    /**
     * @brief   Send data to a spi slave device.
     * @param   sendBuf pointer to transmit buffer
     * @param   len     size of transmit buffer
     * @retval  int32_t number of sent bytes, value < 0 on failure
     */
    int32_t write(const uint8_t* sendBuf, uint32_t len);

    /**
     * @brief   Request data from a spi slave device.
     *          To get data from slave the master must generate the SPI clock and dummy
     *          data must be sent. For it recBuf will be used.
     * @param   recBuf  pointer to receive buffer
     * @param   maxLen  size of receive buffer
     * @retval  int32_t number of received bytes, value < 0 on failure
     */
    int32_t read( uint8_t* recBuf, uint32_t maxLen);

	/**
	 * @brief   Send and receive data FULL-DUPLEX.
	 *          With every sent byte one byte is received and put in recBuf.
	 *          If recBuf is bigger than sendBuf dummy bytes will be sent to
	 *          get requested data.
	 *          If sendBuf is bigger than recBuf you will only get the first
	 *          received bytes until recBuf is full.
	 * @param   sendBuf pointer to transmit buffer
	 * @param   len     size of transmit buffer
	 * @param   recBuf  pointer to receive buffer
	 * @param   maxLen  size of receive buffer
	 * @retval  int32_t number of received bytes, value < 0 on failure
	 */
    int32_t writeRead(const uint8_t* sendBuf, uint32_t len, uint8_t* recBuf, uint32_t maxLen);

};
#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* HAL_SPI_H_ */
