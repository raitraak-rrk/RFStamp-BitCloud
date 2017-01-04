/*************************************************************************//**
  \file apsSmSsp.c

  \brief APS SSP features header file

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    07/11/07 A. Potashov - Created.
*****************************************************************************/

#ifndef _APSSMSSP_H
#define _APSSMSSP_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <aps.h>
#include <apsMemoryManager.h>

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  APS_SM_SSPE_SUCCESS_STATUS  = 0x00,
  APS_SM_SSPE_START_STATUS,
  APS_SM_SSPE_FAIL_STATUS,
} ApsSspeStatus_t;


#if defined _SECURITY_ && defined _LINK_SECURITY_
typedef enum
{
  APS_SM_SSPD_SUCCESS_STATUS  = 0x00,
  APS_SM_SSPD_START_STATUS,
  APS_SM_SSPD_FAIL_STATUS,
} ApsSspdStatus_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/*****************************************************************************
  APS Frame Encrypt Function
  Parameters: buffer      - APS Security Manager buffer pointer
              dstExtAddr  - destination extended address pointer
              apduLength  - APDU length
  Returns:    APS_SM_SSPE_START_STATUS - encryption process started
              APS_SM_SSPE_FAIL_STATUS - encryption failed
              APS_SM_SSPE_SUCCESS_STATUS - encryption not required
                                                   (not needed)
*****************************************************************************/
/**
 * \brief request for encryption of data buffer
 */
APS_PRIVATE void apsEncryptDataFrame(ApsDataBuffer_t *buffer, uint8_t headerLength);

APS_PRIVATE void apsSecurityTaskHandler(void);
APS_PRIVATE  void apsSecuredNwkDataIndHandler(NWK_DataInd_t *ind);

/*****************************************************************************
  Make a search of key-pair descriptor for extended address specified
  Parameters: address - 64-bit IEEE device address pointer
  Returns:    NULL - key-pair descriptor not found
              valid key-pair descriptor pointer - in other case
*****************************************************************************/
APS_PRIVATE void apsDecryptSentFrame(ApsDataBuffer_t *buffer);

APS_PRIVATE void apsResetSmSsp(void);
#else
#define apsSecurityTaskHandler NULL
#endif //_SECURITY_ and _LINK_SECURITY_
#endif  //#ifndef _APSSMSSP_H
//eof apsSmSsp.h
