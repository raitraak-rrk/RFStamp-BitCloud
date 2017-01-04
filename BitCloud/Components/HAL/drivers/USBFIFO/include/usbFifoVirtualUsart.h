/**************************************************************************//**
\file   usbFifoVirtualUsart.h

\brief  The declaration of interfunction interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    15.07.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _USBFIFOVIRTUALUSART_H
#define _USBFIFOVIRTUALUSART_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
#if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)

// the macros for the manipulation by PC6 for binary decoder
HAL_ASSIGN_PIN(PC6, D, 6);
// the macros for the manipulation by PC7 for binary decoder
HAL_ASSIGN_PIN(PC7, D, 7);
// the macros for the manipulation by RD
HAL_ASSIGN_PIN(RD, E, 5);
// the macros for the manipulation by WR
HAL_ASSIGN_PIN(WR, E, 4);

#elif defined(ATMEGA1281) || defined(ATMEGA2561)

// the macros for the manipulation by PC6 for binary decoder
HAL_ASSIGN_PIN(PC6, C, 6);
// the macros for the manipulation by PC7 for binary decoder
HAL_ASSIGN_PIN(PC7, C, 7);
// the macros for the manipulation by RD
HAL_ASSIGN_PIN(RD, G, 1);
// the macros for the manipulation by WR
HAL_ASSIGN_PIN(WR, G, 0);

#else
  #error 'USB FIFO is not supported for this mcu.'
#endif

/**************************************************************************//**
\brief Startup initialization.
******************************************************************************/
int usbfifoInit(void);

/**************************************************************************//**
\brief Clear startup initialization parameters
******************************************************************************/
void usbfifoUnInit(void);

/**************************************************************************//**
\brief  USB FIFO driver task handler.
******************************************************************************/
void usbfifoHandler(void);

#endif /* _USBFIFOVIRTUALUSART_H */
