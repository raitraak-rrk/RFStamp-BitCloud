/**************************************************************************//**
  \file machwdBatMon.h

  \brief Prototypes of battery monitor functions and corresponding types.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      16/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDBATMON_H
#define _MACHWDBATMON_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rfBattery.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/
//! Redefenition of battery request function name.
#define MACHWD_BatMonReq RF_BatteryMonReq

/******************************************************************************
                    Types section
******************************************************************************/
//! Redefenition of battery request structure.
typedef RF_BatteryMonReq_t MACHWD_BatMonReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Sets battery monitor voltage.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void MACHWD_BatMonReq(MACHWD_BatMonReq_t *reqParams);

#endif /*_MACHWDBATMON_H*/

// eof machwdBatMon.h
