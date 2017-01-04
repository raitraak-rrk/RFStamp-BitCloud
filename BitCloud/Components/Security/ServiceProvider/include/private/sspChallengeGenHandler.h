/**************************************************************************//**
  \file sspChallengeGenHandler.h

  \brief Challenge generator handler header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      17/12/07 ALuzhetsky - Created.
******************************************************************************/
#ifndef _SSPCHALLENGEGENHANDLER_H
#define _SSPCHALLENGEGENHANDLER_H

/******************************************************************************
                        Includes section.
******************************************************************************/

/******************************************************************************
                        Definition section.
******************************************************************************/

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/*******************************************************************************
  Resets Security Service Provider entity.
  Parameters:
    challenge - pointer to the memory for saving generated challenge.
  Return:
    none.
********************************************************************************/
void sspGenerateChallengeHandler(uint8_t challenge[/*CHALLENGE_SEQUENCE_SIZE*/]);
#endif //_SSPCHALLENGEGENHANDLER_H

// eof sspChallengeGenHandler.h

