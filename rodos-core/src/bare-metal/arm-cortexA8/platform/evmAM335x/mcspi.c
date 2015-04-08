/**
 * \file   mcspi.c
 *
 * \brief  This file contains functions which configure the pin muxing and
 *         module clock configurations for mcspi peripheral.
 *
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

#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "evmAM335x.h"
#include "hw_cm_per.h"
#include "hw_types.h"
#include "mcspi.h"

/**
 * \brief   This function selects the McSPI pins for use. The McSPI pins
 *          are multiplexed with pins of other peripherals in the SoC
 *          
 * \param   instanceNum       The instance number of the McSPI instance to be
 *                            used.
 * \return  TRUE/FALSE
 *
 * \note    This muxing depends on the profile in which the EVM is configured.
 */
unsigned int McSPIPinMuxSetup(unsigned int instanceNum)
{
    unsigned int profile = 2;
    unsigned int status = FALSE;

    if (0 != instanceNum)
    {
        return FALSE;
    }

    profile = EVMProfileGet(); 

    switch (profile)
    {
        case 2:
            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_SCLK) = 
                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL | 
                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);
            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D0) = 
                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL | 
                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);
            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D1) = 
                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL | 
                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);
            status = TRUE;
        break;
        default:
        break;
    }
    return status;
}

/**
 * \brief   This function selects the McSPI CS's for use. The McSPI CS pins
 *          are multiplexed with pins of other peripherals in the SoC
 *          
 * \param   instanceNum       The instance number of the McSPI instance to be
 *                            used.
 * \return  TRUE/FALSE
 *
 * \note    This muxing depends on the profile in which the EVM is configured.
 */
unsigned int McSPI0CSPinMuxSetup(unsigned int csPinNum)
{
    unsigned int profile = 2;

    if (0 != csPinNum)
    {
        return FALSE;
    }

    profile = EVMProfileGet(); 

    switch (profile)
    {
        case 2:
            HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS0) = 
                 (CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_PUTYPESEL | 
                      CONTROL_CONF_SPI0_SCLK_CONF_SPI0_SCLK_RXACTIVE);
            return TRUE;
        default:
        break;
    }
    return FALSE;
}

/**
 * \brief   This function will configure the required clocks for McSPI0 instance.
 *
 * \return  None.
 *
 */
void McSPI0ModuleClkConfig(void)
{
    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) = 
                             CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) & 
     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) = 
                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) & 
     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) = 
                             CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) & 
                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) != 
                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) = 
                             CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) & 
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) = 
                             CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) & 
                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) != 
                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) = 
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) & 
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) != 
                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) = 
                             CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) & 
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) &= ~CM_PER_SPI0_CLKCTRL_MODULEMODE;

    HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) |= 
                             CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_CM_PER_REGS + CM_PER_SPI0_CLKCTRL) & 
      CM_PER_SPI0_CLKCTRL_MODULEMODE) != CM_PER_SPI0_CLKCTRL_MODULEMODE_ENABLE);


    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) & 
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) & 
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) & 
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK | 
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));

    while(!(HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) & 
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK | 
            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_SPI_GCLK)));
    
}

