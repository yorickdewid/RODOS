/*!
 * \file    i2cExample.cpp
 *
 * \date    03. 06. 2013
 * \author  Michael Ruffer
 *
 * \brief   To run this example you must connect the pololu module "imu01a"
 *          to I2C bus 1 according to the hal_i2c.cpp of your target
 *          -> e.g. STM32F4 pin PD6 & PD9
 */

#include "rodos.h"
#include "hal/hal_i2c.h"

static Application APP_I2CTest("APP_I2CTest", 2000);

#define L3G4200D_TEST       1 // pololu module imu01a - gyro
#define LSM303_ACC_TEST     0 // pololu module imu01a - accelerometer
#define LSM303_MAG_TEST     0 // pololu module imu01a - magnetometer


HAL_I2C i2c1(I2C_IDX1);


/* addresses of I2C slave devices */
#define LSM303DLH_ACC_ADDR  0x18
#define LSM303DLH_MAG_ADDR  0x1E
#define L3G4200D_GYR_ADDR   0x69


class I2CTest: public Thread {
public:
	I2CTest(const char* name) :
			Thread(name) {
	}


	void init() {
        uint8_t txBuf[2]={0};

		i2c1.init();

#if L3G4200D_TEST
        txBuf[0] = 0x20; // address of CTRL_REG1
        txBuf[1] = 0x0F; // value of CTRL_REG1 -> normal mode, x,y,z enable
        if (i2c1.write(L3G4200D_GYR_ADDR,txBuf,2) < 0) i2c1.init();
        txBuf[0] = 0x23; // address of CTRL_REG4_A
        txBuf[1] = 0x40; // value of CTRL_REG4_A -> MSB first
        //txBuf[1] = 0x00; // value of CTRL_REG4_A -> LSB first
        if (i2c1.write(L3G4200D_GYR_ADDR,txBuf,2) < 0) i2c1.init();
#endif
#if LSM303_ACC_TEST
        txBuf[0] = 0x20; // address of CTRL_REG1_A
        txBuf[1] = 0x27; // value of CTRL_REG1_A -> normal mode, x,y,z enable
        if (i2c1.write(LSM303DLH_ACC_ADDR,txBuf,2) < 0) i2c1.init();
        txBuf[0] = 0x23; // address of CTRL_REG4_A
        txBuf[1] = 0x40; // value of CTRL_REG4_A -> MSB first
        //txBuf[1] = 0x00; // value of CTRL_REG4_A -> LSB first
        if (i2c1.write(LSM303DLH_ACC_ADDR,txBuf,2) < 0) i2c1.init();
#endif
#if LSM303_MAG_TEST
        txBuf[0] = 0x02; // address of MR_REG
        txBuf[1] = 0x00; // value of MR_REG -> continuous mode
        if (i2c1.write(LSM303DLH_MAG_ADDR,txBuf,2) < 0) i2c1.init();
#endif
	}



	void run() {
		uint8_t rxBuf[6]={0};
		uint8_t txBuf[3]={0};
		int32_t err[10] = {0};

		while (1) {
			/** check out L3G4200D (extern) **/
#if L3G4200D_TEST
            memset(rxBuf,0,sizeof(rxBuf));
            memset(txBuf,0,sizeof(txBuf));
			txBuf[0] = 0x28 | 0x80; // start reading with x-low register, read multiple register
			err[0] = i2c1.writeRead(L3G4200D_GYR_ADDR,txBuf,1,rxBuf,6);
            txBuf[0] = 0x0f; // WHO_AM_I Register
            err[1] = i2c1.writeRead(L3G4200D_GYR_ADDR,txBuf,1,&rxBuf[6],1);
			if (printError("L3G4200D GYRO", err, 2) > 0){
                PRINTF("Init I2C and all slaves ...\n\n");
                init(); // init i2c1/i2c2 and all slaves
            } else {
                PRINTF("L3G4200D \"Who am I\" reg (0xD3):0x%x\n",rxBuf[6]);
                //PRINTF("L3G4200D GYRO x,y,z: %d, %d, %d\n",rxBuf[0]|(rxBuf[1]<<8), rxBuf[2]|(rxBuf[3]<<8), rxBuf[4]|(rxBuf[5]<<8)); // LSB first
                PRINTF("L3G4200D GYRO x,y,z: %d, %d, %d\n\n",rxBuf[1]|(rxBuf[0]<<8), rxBuf[3]|(rxBuf[2]<<8), rxBuf[5]|(rxBuf[4]<<8)); // MSB first
            }

#endif

            /** check out LSM303 (extern) **/
#if LSM303_ACC_TEST
            // ACC
            memset(rxBuf,0,sizeof(rxBuf));
            memset(txBuf,0,sizeof(txBuf));
            txBuf[0] = 0x28 | 0x80; // start reading with x-low register, read multiple register
            err[0] = i2c1.writeRead(LSM303DLH_ACC_ADDR,txBuf,1,rxBuf,6);
            if (printError("LSM303 ACC", err, 1) > 0){
                PRINTF("Init I2C and all slaves ...\n\n");
                init(); // init i2c1/i2c2 and all slaves
           }else{
                //PRINTF("LSM303 ACC x,y,z: %d, %d, %d\n\n",rxBuf[0]|(rxBuf[1]<<8), rxBuf[2]|(rxBuf[3]<<8), rxBuf[4]|(rxBuf[5]<<8)); // LSB first
                PRINTF("LSM303 ACC x,y,z: %d, %d, %d\n\n",rxBuf[1]|(rxBuf[0]<<8), rxBuf[3]|(rxBuf[2]<<8), rxBuf[5]|(rxBuf[4]<<8)); // MSB first
            }
#endif
            // MAG
#if LSM303_MAG_TEST
            memset(rxBuf,0,sizeof(rxBuf));
            memset(txBuf,0,sizeof(txBuf));
            txBuf[0] = 0x03; // start reading with x-high register, read multiple register
            err[0] = i2c1.writeRead(LSM303DLH_MAG_ADDR,txBuf,1,rxBuf,6);
            if (printError("LSM303 MAG", err, 1) > 0){
                PRINTF("Init I2C and all slaves ...\n\n");
                init(); // init i2c1/i2c2 and all slaves
            }else{
                PRINTF("LSM303 MAG x,y,z: %d, %d, %d\n\n",rxBuf[1]|rxBuf[0]<<8,rxBuf[3]|rxBuf[2]<<8,rxBuf[5]|rxBuf[4]<<8); // MSB first
            }
#endif

			suspendCallerUntil(NOW()+1*SECONDS);
		}
	}


	int32_t printError(const char* dev, int32_t *err, uint32_t size){
        uint32_t i=0;
        uint32_t errCnt = 0;

        for (i=0;i<size;i++){
            if (err[i] < 0){
                PRINTF("%s I2C error:\n",dev);
                break;
            }
        }

        for (i=0;i<size;i++){
            if (err[i] < 0){
                errCnt++;
                PRINTF(" %d. %d\n",(int)i+1,(int)err[i]);
            }
        }

        return errCnt;
    }
};

I2CTest i2cTest("I2CTest");
