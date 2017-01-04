/************************************************************//**
  \file rs232Controller.h

  \brief Functions for RS232 level converter interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    28.09.09 A. Taradov - Created
*****************************************************************/
#ifndef _RS232CONTROLLER_H
#define _RS232CONTROLLER_H

/*****************************************************************
                   Includes section
*****************************************************************/
#include <gpio.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#ifndef BSP_ENABLE_RS232_CONTROL
#if defined(BOARD_MEGARF) || defined(BOARD_STK600)
  #if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
    #if BSP_SUPPORT == BOARD_RCB_KEY_REMOTE || (BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USART)
      #define BSP_ENABLE_RS232_CONTROL 1
    #else
      #define BSP_ENABLE_RS232_CONTROL 0
    #endif /* BSP_SUPPORT == BOARD_RCB_KEY_REMOTE || (BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USART) */
  #elif defined(RCB230_V31) || defined(RCB230_V32) || defined(RCB230_V331) || defined(RCB231_V402) || \
        defined(RCB231_V411) || defined(RCB212_V532)
      #define BSP_ENABLE_RS232_CONTROL 1
  #else
    #define BSP_ENABLE_RS232_CONTROL 0
  #endif /* defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2) */
#elif defined(BOARD_SAM3S_EK)
  #define BSP_ENABLE_RS232_CONTROL 1
#else
  #define BSP_ENABLE_RS232_CONTROL 0
#endif /* defined(BOARD_MEGARF) || defined(BOARD_STK600) */
#endif /* BSP_ENABLE_RS232_CONTROL */
/************************************************************//**
\brief Enable RS232 level converter.
****************************************************************/
static inline void BSP_EnableRs232(void)
{
#if BSP_ENABLE_RS232_CONTROL == 1
  #if defined(BOARD_MEGARF) || defined(BOARD_STK600)
    #if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
      #if BSP_SUPPORT == BOARD_RCB_KEY_REMOTE
        GPIO_E5_make_out();
        GPIO_E5_clr();

        GPIO_B6_make_out();
        GPIO_B6_clr();

        GPIO_E5_set();
        GPIO_E5_clr();
      #elif BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USART
        // ~EN
        GPIO_D4_make_out();
        GPIO_D4_clr();

        // FORCEON
        GPIO_D6_make_out();
        GPIO_D6_set();

        // ~FORCEOFF
        GPIO_D7_make_out();
        GPIO_D7_set();
      #elif (BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USBFIFO) || \
            (BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO)
        #warning "It should be used only with breakout boards."
      #endif //
    #elif defined(RCB230_V31) || defined(RCB230_V32) || defined(RCB230_V331) || defined(RCB231_V402) || \
          defined(RCB231_V411) || defined(RCB212_V532)
      // ~EN
      GPIO_13_make_out();
      GPIO_13_clr();

      // FORCEON
      GPIO_15_make_out();
      GPIO_15_set();

      // ~FORCEOFF
      GPIO_16_make_out();
      GPIO_16_set();
    #endif // RCB board revisions
  #elif defined(BOARD_SAM3S_EK)
    // ~EN
    GPIO_PA23_make_out();
    GPIO_PA23_clr();
  #else
    #error 'Unsupported board.'
  #endif // Boards
#endif
};

/************************************************************//**
\brief Disable RS232 level converter.
****************************************************************/
static inline void BSP_DisableRs232(void)
{
#if BSP_ENABLE_RS232_CONTROL == 1
  #if defined(BOARD_MEGARF) || defined(BOARD_STK600)
    #if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
      #if BSP_SUPPORT == BOARD_RCB_KEY_REMOTE
        GPIO_E5_make_in();
        GPIO_B6_make_in();
      #elif BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USART
        GPIO_D4_make_in();
        GPIO_D6_make_in();
        GPIO_D7_make_in();
      #elif (BSP_SUPPORT == BOARD_RCB && APP_INTERFACE == APP_INTERFACE_USBFIFO) || \
            (BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO)
        #warning "It should be used only with breakout boards."
      #endif // BOARD_RCB_KEY_REMOTE
    #elif defined(RCB230_V31) || defined(RCB230_V32) || defined(RCB230_V331) || defined(RCB231_V402) || \
          defined(RCB231_V411) || defined(RCB212_V532)
      GPIO_13_make_in();
      GPIO_15_make_in();
      GPIO_16_make_in();
    #endif // RCB board revisions
  #elif defined(BOARD_SAM3S_EK)
    GPIO_PA23_make_in();
  #else
    #error 'Unsupported board.'
  #endif
#endif
};

#endif /* _RS232CONTROLLER_H */
