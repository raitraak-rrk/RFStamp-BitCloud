/**************************************************************************//**
  \file sspAuthenticHandler.h

  \brief Mutual Symmetric-Key Entity Authentication routine handler's header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      17/12/07 ALuzhetsky - Created.
******************************************************************************/
#ifndef _SSPAUTHENTICHANDLER_H
#define _SSPAUTHENTICHANDLER_H

#ifdef _LINK_SECURITY_
/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <sspManager.h>
#include <sspHashHandler.h>
#include <sspAesHandler.h>

/******************************************************************************
                        Types section.
******************************************************************************/
/******************************************************************************
  TBD
******************************************************************************/
typedef struct
{
  SspKeyedHashMacReq_t keyedHashMacParams;
  BEGIN_PACK
  struct PACK
  {
    volatile uint8_t keyExpander[SECURITY_KEY_SIZE];
    uint8_t macData[MAX_HASH_TEXT_SIZE];
    volatile uint8_t bitsExpander[16];
  };
  END_PACK
} SspAuthenticHandlerMem_t;
/******************************************************************************
                        Prototypes section.
******************************************************************************/
/*******************************************************************************
  Calculates parameters (MacTag1) for Mutual Symmetric-Key Entity Authentication
  procedure.
  Parameters:
    none.
  Return:
    none.
********************************************************************************/
void sspCalculateAuthenticMacTag1ReqHandler(void);

/*******************************************************************************
  Calculates parameters (MacTag2) for Mutual Symmetric-Key Entity Authentication
  procedure.
  Parameters:
    none.
  Return:
    none.
********************************************************************************/
void sspCalculateAuthenticMacTag2ReqHandler(void);

#endif // _LINK_SECURITY_

#endif //_SSPAUTHENTICHANDLER_H

// eof sspAuthenticHandler.h

