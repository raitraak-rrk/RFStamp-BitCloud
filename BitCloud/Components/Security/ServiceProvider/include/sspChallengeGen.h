/**************************************************************************//**
  \file sspChallengeGen.h

  \brief Challenge genrator header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      07/12/07 ALuzhetsky - Created.
******************************************************************************/
#ifndef _SSPCHALLENGEGEN_H
#define _SSPCHALLENGEGEN_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <sspCommon.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
#define CHALLENGE_SEQUENCE_SIZE 16

/******************************************************************************
                        Types' definition section.
******************************************************************************/

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/*******************************************************************************
  Generates challenge.
  Parameters:
    challenge - space to store generated challenge.
  Return:
    none.
********************************************************************************/
void SSP_GenerateChallenge(uint8_t challenge[/*CHALLENGE_SEQUENCE_SIZE*/]);

#endif //_SSPCHALLENGEGEN_H

// eof sspChellenge.h

