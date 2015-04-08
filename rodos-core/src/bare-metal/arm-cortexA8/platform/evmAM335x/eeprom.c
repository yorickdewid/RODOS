/**
 * \file   eeprom.c
 *
 * \brief  This file contains functions for accessing eeprom
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "soc_AM335x.h"
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "evmAM335x.h"
#include "hsi2c.h"

/******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define EEPROM_I2C_BASE                       (SOC_I2C_0_REGS)

/******************************************************************************
**                          FUNCTION DEFINITIONS
******************************************************************************/
/**
 * \brief   Sets up the EEPROM I2C interface
 *          
 * \param   slaveAddr   Slave Address of the EEPROM
 *
 * \return  None.
 */
void EEPROMI2CSetUp(unsigned int slaveAddr)
{
    /* Put i2c in reset/disabled state */
    I2CMasterDisable(EEPROM_I2C_BASE);

    /* Configure i2c bus speed to 100khz */
    I2CMasterInitExpClk(EEPROM_I2C_BASE, 48000000, 24000000, 100000);

    /* Set i2c slave address */
    I2CMasterSlaveAddrSet(EEPROM_I2C_BASE, slaveAddr);

    /* Disable all I2C interrupts */
    I2CMasterIntDisableEx(EEPROM_I2C_BASE, 0xFFFFFFFF);

    /* Bring I2C module out of reset */
    I2CMasterEnable(EEPROM_I2C_BASE);
}

/**
 * \brief   This function reads data from EEPROM.
 *          
 * \param   data    Address where data is to be read.
 * \param   length  Length of data to be read
 * \param   offset  Address of the byte from which data to be read.
 *
 * \return  None.
 *
 * \note    This muxing depends on the profile in which the EVM is configured.
 *          EEPROMI2CSetUp Shall be called Before this API is used
 */
void EEPROMI2CRead(unsigned char *data, unsigned int length,
                   unsigned short offset)
{
    unsigned int idx = 0;

    /* First send the register offset - TX operation */
    I2CSetDataCount(EEPROM_I2C_BASE, 2);

    I2CMasterControl(EEPROM_I2C_BASE, I2C_CFG_MST_TX);

    I2CMasterStart(EEPROM_I2C_BASE);

    /* Wait for the START to actually occir on the bus */
    while (0 == I2CMasterBusBusy(EEPROM_I2C_BASE));

    /* Wait for the Tx register to be empty */
    while (0 == I2CMasterIntRawStatusEx(EEPROM_I2C_BASE, 
                                        I2C_INT_TRANSMIT_READY));

    I2CMasterDataPut(EEPROM_I2C_BASE, (unsigned char)((offset >> 8) & 0xFF));

    /* Wait for the Tx register to be empty */
    while (0 == I2CMasterIntRawStatusEx(EEPROM_I2C_BASE, 
                                        I2C_INT_TRANSMIT_READY));

    /* Push offset out and tell CPLD from where we intend to read the data */
    I2CMasterDataPut(EEPROM_I2C_BASE, (unsigned char)(offset & 0xFF));

    /* Now that we have sent the register offset, start a RX operation*/
    I2CMasterControl(EEPROM_I2C_BASE, I2C_CFG_MST_RX);

    /* Repeated start condition */
    I2CMasterStart(EEPROM_I2C_BASE);

    while (length--)
    {
        while (0 == I2CMasterIntRawStatusEx(EEPROM_I2C_BASE,
                                            I2C_INT_RECV_READY));
        data[idx] = (unsigned char)I2CMasterDataGet(EEPROM_I2C_BASE);
    }

    I2CMasterStop(EEPROM_I2C_BASE);
}

/****************************** End Of File *********************************/
