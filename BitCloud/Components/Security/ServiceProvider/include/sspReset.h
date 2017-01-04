/**************************************************************************//**
  \file sspReset.h

  \brief Security Frame Processor reset routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      07/12/2007 - ALuzhetsky created.
******************************************************************************/

#ifndef _SSPRESET_H
#define _SSPRESET_H

/******************************************************************************
                        Functions' prototypes section.
******************************************************************************/
void SSP_Init(void);

/**************************************************************************//**
  \brief Resets Security service provider
 ******************************************************************************/
void SSP_Reset(void);

#endif //_SSPRESET_H

// eof sspReset.h
