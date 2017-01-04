/******************************************************************************
  \file phyTxPowerConverter.h

  \brief
    PHY TX power converter interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      19/09/13 N. Fomin - Created.
******************************************************************************/

#ifndef _PHYTXPOWERCONVERTER_H
#define _PHYTXPOWERCONVERTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <at86rf2xx.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Converts tx power from dBm to predefined value of particular RF chip

\param[in] txPower - power of transmitter in dBm

\returns power of transmitter in predefined value
******************************************************************************/
uint8_t PHY_ConvertTxPowerFromDbm(int8_t txPower);

/**************************************************************************//**
\brief Converts tx power from predefined value of particular RF chip to dBm

\param[in] txPower - power of transmitter in predefined value

\returns power of transmitter in dBm
******************************************************************************/
int8_t PHY_ConvertTxPowerToDbm(uint8_t txPower);

#endif /* _PHYTXPOWERCONVERTER_H */

// eof phyTxPowerConverter.h
