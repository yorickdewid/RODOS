/***************************************************************************
 **
 **    This file defines the board specific definition
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2007
 **
 **    $Revision: 30870 $
 **
 ***************************************************************************/
#include "arm_comm.h"

#ifndef __BOARD_H
#define __BOARD_H

#define I_RC_OSC_FREQ   (4MHZ)
#define MAIN_OSC_FREQ   (12MHZ)
#define RTC_OSC_FREQ    (32768UL)


// USB Link LED 1
#define USB_LINK_LED1_MASK  (1UL<<18)
#define USB_LINK_LED1_DIR   IO1DIR
#define USB_LINK_LED1_FDIR  FIO1DIR
#define USB_LINK_LED1_SET   IO1SET
#define USB_LINK_LED1_FSET  FIO1SET
#define USB_LINK_LED1_CLR   IO1CLR
#define USB_LINK_LED1_FCLR  FIO1CLR
#define USB_LINK_LED1_IO    IO1PIN
#define USB_LINK_LED1_FIO   FIO1PIN

// USB Link LED 2
#define USB_LINK_LED2_MASK  (1UL<<13)
#define USB_LINK_LED2_DIR   IO0DIR
#define USB_LINK_LED2_FDIR  FIO0DIR
#define USB_LINK_LED2_SET   IO0SET
#define USB_LINK_LED2_FSET  FIO0SET
#define USB_LINK_LED2_CLR   IO0CLR
#define USB_LINK_LED2_FCLR  FIO0CLR
#define USB_LINK_LED2_IO    IO0PIN
#define USB_LINK_LED2_FIO   FIO0PIN

// LCD
// LCD Backlight
#define LCD_BL_MASK         (1UL<<21)
#define LCD_BL_DIR          IO0DIR
#define LCD_BL_FDIR         FIO0DIR
#define LCD_BL_SET          IO0SET
#define LCD_BL_FSET         FIO0SET
#define LCD_BL_CLR          IO0CLR
#define LCD_BL_FCLR         FIO0CLR
#define LCD_BL_IO           IO0PIN
#define LCD_BL_FIO          FIO0PIN

#define LCD_LIGHT_ON()      LCD_BL_FSET = LCD_BL_MASK
#define LCD_LIGHT_OFF()     LCD_BL_FCLR = LCD_BL_MASK

// LCD E
#define LCD_E_MASK          (1UL<<26)
#define LCD_E_DIR           IO0DIR
#define LCD_E_FDIR          FIO0DIR
#define LCD_E_SET           IO0SET
#define LCD_E_FSET          FIO0SET
#define LCD_E_CLR           IO0CLR
#define LCD_E_FCLR          FIO0CLR
#define LCD_E_IO            IO0PIN
#define LCD_E_FIO           FIO0PIN

// LCD RS
#define LCD_RS_MASK         (1UL<<24)
#define LCD_RS_DIR          IO0DIR
#define LCD_RS_FDIR         FIO0DIR
#define LCD_RS_SET          IO0SET
#define LCD_RS_FSET         FIO0SET
#define LCD_RS_CLR          IO0CLR
#define LCD_RS_FCLR         FIO0CLR
#define LCD_RS_IO           IO0PIN
#define LCD_RS_FIO          FIO0PIN

// LCD RW
#define LCD_RW_MASK         (1UL<<25)
#define LCD_RW_DIR          IO0DIR
#define LCD_RW_FDIR         FIO0DIR
#define LCD_RW_SET          IO0SET
#define LCD_RW_FSET         FIO0SET
#define LCD_RW_CLR          IO0CLR
#define LCD_RW_FCLR         FIO0CLR
#define LCD_RW_IO           IO0PIN
#define LCD_RW_FIO          FIO0PIN

// LCD DATA
#define LCD_DATA_SHIFT       17
#define LCD_DATA_MASK       (0xFUL<<LCD_DATA_SHIFT)
#define LCD_DATA_DIR        IO0DIR
#define LCD_DATA_FDIR       FIO0DIR
#define LCD_DATA_SET        IO0SET
#define LCD_DATA_FSET       FIO0SET
#define LCD_DATA_CLR        IO0CLR
#define LCD_DATA_FCLR       FIO0CLR
#define LCD_DATA_IO         IO0PIN
#define LCD_DATA_FIO        FIO0PIN

// LEDs array
#define ARRAY_LED_SHIFT     (0)
#define ARRAY_LED_MASK      (0xFFUL << ARRAY_LED_SHIFT)
#define ARRAY_LED_FDIR      FIO2DIR
#define ARRAY_LED_FSET      FIO2SET
#define ARRAY_LED_FCLR      FIO2CLR
#define ARRAY_LED_FIO       FIO2PIN

// Buttons
#define K1_MASK             (1UL<<10)
#define K1_FDIR             FIO2DIR
#define K1_FIO              FIO2PIN

#define K2_MASK             (1UL<<12)
#define K2_FDIR             FIO2DIR
#define K2_FIO              FIO2PIN

#define K3_MASK             (1UL<<13)
#define K3_FDIR             FIO2DIR
#define K3_FIO              FIO2PIN

// MMC/SD
#define SD_CMD_MODE         PINMODE2_bit.P1_3

// card switches
// Card present
#define MMC_CP_MASK         (1UL << 11)
#define MMC_CP_FDIR         FIO2DIR
#define MMC_CP_FIO          FIO2PIN
#define MMC_CP_MODE         PINMODE0_bit.P0_11

// Analog trim
#define ANALOG_TRIM_CHANNEL 0
#define ANALOG_TRIM_CHANNEL_SEL   PINSEL1_bit.P0_23



// PCLK offset
#define WDT_PCLK_OFFSET     0
#define TIMER0_PCLK_OFFSET  2
#define TIMER1_PCLK_OFFSET  4
#define UART0_PCLK_OFFSET   6
#define UART1_PCLK_OFFSET   8
#define PWM0_PCLK_OFFSET    10
#define PWM1_PCLK_OFFSET    12
#define I2C0_PCLK_OFFSET    14
#define SPI_PCLK_OFFSET     16
#define RTC_PCLK_OFFSET     18
#define SSP1_PCLK_OFFSET    20
#define DAC_PCLK_OFFSET     22
#define ADC_PCLK_OFFSET     24
#define CAN1_PCLK_OFFSET    26
#define CAN2_PCLK_OFFSET    28
#define ACF_PCLK_OFFSET     30
#define BAT_RAM_PCLK_OFFSET 32
#define GPIO_PCLK_OFFSET    34
#define PCB_PCLK_OFFSET     36
#define I2C1_PCLK_OFFSET    38
//#define                   40
#define SSP0_PCLK_OFFSET    42
#define TIMER2_PCLK_OFFSET  44
#define TIMER3_PCLK_OFFSET  46
#define UART2_PCLK_OFFSET   48
#define UART3_PCLK_OFFSET   50
#define I2C2_PCLK_OFFSET    52
#define I2S_PCLK_OFFSET     54
#define MCI_PCLK_OFFSET     56
//#define                   58
#define PCLK_PCLK_OFFSET    60
//#define                   62

#endif /* __BOARD_H */
