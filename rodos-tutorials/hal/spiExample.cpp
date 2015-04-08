/**
 * @file    spiExample.cpp
 *
 * @date    03.12.2013
 * @author  Michael Ruffer
 *
 * @brief   Shows how to use RODOS HAL_SPI. This example doesn't use a "real" slave
 *          device. Use an oscilloscope instead and connect it to CS, SCK and MOSI according
 *          hal_spi.cpp and hal_gpio.cpp of your target.
 *          e.g. STM32F4 pin PA0 (CS), PA5 (SCK) and PA7 (MOSI)
 *          An example using HAL_SPI to access a spi flash can be found in:
 *          rodos-support/support_libs/flash/spiFlash_AT45DBxxx
 */
#include "rodos.h"


HAL_SPI spi1(SPI_IDX1);
HAL_GPIO cs(GPIO_000);

class SPIExample:public Thread{
    uint32_t baudrate;
public:
    SPIExample():Thread("SPI example"){
        baudrate = 1000000;
    }

    void init(){
        cs.init(true,1,1);
        spi1.init(baudrate);
    }

    void run(){
        uint8_t i = 0;

        TIME_LOOP(NOW()+1*SECONDS,1*SECONDS){
            uint8_t rdBuf[5] = {0};
            uint8_t wrBuf[2] = {0,1};
            wrBuf[0] = i++;

            PRINTF("SPI baudrate wanted: %d real: %d\n",baudrate, spi1.status(SPI_STATUS_BAUDRATE));

            /* write */
            cs.setPins(0);
            spi1.write(wrBuf,sizeof(wrBuf));
            cs.setPins(1);

            /* read */
            cs.setPins(0);
            spi1.read(rdBuf,sizeof(rdBuf));
            cs.setPins(1);

            /* write and read */
            cs.setPins(0);
            spi1.write(wrBuf,sizeof(wrBuf));
            spi1.read(rdBuf,sizeof(rdBuf));
            cs.setPins(1);

            /* writeRead full-duplex*/
            cs.setPins(0);
            spi1.writeRead(wrBuf,sizeof(wrBuf),rdBuf,sizeof(rdBuf));
            cs.setPins(1);

            baudrate += 1000000;
            if (baudrate > 50000000) baudrate = 1000000;
            spi1.config(SPI_PARAMETER_BAUDRATE,baudrate);
        }
    }
}spiExpample;

