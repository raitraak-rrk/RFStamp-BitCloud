/************************************************************************//**
  \file bspDbg.h

  \brief
    The header file describes BSP Debug Module

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/04/14 karthik.p_u - Created
******************************************************************************/

#ifndef _BSPDBD_H
#define _BSPDBD_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <dbg.h>

/**************************************************************************//**
                    Types section
******************************************************************************/
typedef enum
{
  BSPSENSORS_TEMPERATURENULLCALLBACK_0 = 0x9000,
  BSPSENSORS_LIGHTNULLCALLBACK_0       = 0x9001,
  BSPSENSORS_BATTERYNULLCALLBACK_0     = 0x9002,
  BSPRGBLED_INVALID_BRIGHTNESS_VALUE_0 = 0x9100,
  BSPRGBLED_INVALID_LED_NUMBER_0,
} BspDbgCodeId_t;

#endif// _BSPDBD_H

//eof bspDbg.h
