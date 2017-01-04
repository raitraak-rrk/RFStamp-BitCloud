/******************************************************************************
  \file phyRxTxIndicator.h

  \brief Prototype of radio transceiver state control functions.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      03/07/09 A. Khromykh - Created.
******************************************************************************/
#ifndef _PHYRXTXINDICATOR_H
#define _PHYRXTXINDICATOR_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <phyCommon.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
#define RF_FRONT_END_ENABLED    0x80
#define RF_FRONT_END_DISABLED   0x00

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief  Turn on pin 1 (DIG3) and pin 2 (DIG4) to indicate the transmit state of
the radio transceiver.
******************************************************************************/
void phyRxTxSwitcherOn(void);

/**************************************************************************//**
\brief  Turn off pin 1 (DIG3) and pin 2 (DIG4) to indicate the transmit state of
the radio transceiver.
******************************************************************************/
void phyRxTxSwitcherOff(void);

#endif /* _PHYRXTXINDICATOR_H */

// eof phyRxTxIndicator.h
