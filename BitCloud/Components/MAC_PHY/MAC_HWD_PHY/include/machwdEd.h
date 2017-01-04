/**************************************************************************//**
  \file machwdEd.h

  \brief MACHWD energy detection types and function prototypes.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      24/05/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWDED_H
#define _MACHWDED_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <machwdService.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! MACHWD energy detection confirm structure.
typedef struct
{
  PHY_EnergyLevel_t energyLevel;
} MACHWD_EdConf_t;

//! MACHWD energy detection request structure.
typedef struct
{
  MACHWD_Service_t service;
  // Callback pointer.
  void (*MACHWD_EdConf)(MACHWD_EdConf_t *confParams);
  // Confirm parameters.
  MACHWD_EdConf_t confirm;
} MACHWD_EdReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Detects energy level on channel.
  \param reqParams - MACHWD energy detection request structure pointer.
  \return none.
******************************************************************************/
void MACHWD_EdReq(MACHWD_EdReq_t *reqParams);

#endif /* _MACHWDED_H */

// eof machwdEd.h
