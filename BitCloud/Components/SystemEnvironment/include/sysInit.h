/**************************************************************************//**
\file  sysInit.h

\brief Stack initialization interface

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    16/10/12 N. Fomin - Created
*******************************************************************************/
#ifndef _SYS_INIT_H
#define _SYS_INIT_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <sysUtils.h>

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Wrapper to initializes ZSI layer.
******************************************************************************/
void SYS_InitZsiLayer(void);

/**************************************************************************//**
\brief Wrapper to initializes ZCL layer.
******************************************************************************/
void SYS_InitZclLayer(void);

/**************************************************************************//**
\brief Wrapper to initialize zsiMem to execute ZsiInit layer.
******************************************************************************/
void SYS_InitZsiMem(void);

#endif  // _SYS_INIT_H
//eof sysinit.h
