/**************************************************************************//**
  \file  hexSwitch.h

  \brief Interface of hex switch.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    7/10/08 A. Khromykh - Created
******************************************************************************/
#ifndef _HEXSWITCH_H
#define _HEXSWITCH_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define HEX_SWITCH_FIRST   0
#define HEX_SWITCH_SECOND  1

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Read the hex switch.

\param[in]
  nmSwitch - hex switch number

\return
  hex code.
******************************************************************************/
uint8_t BSP_ReadHexSwitch(uint8_t nmSwitch);

#endif /* _HEXSWITCH_H */
