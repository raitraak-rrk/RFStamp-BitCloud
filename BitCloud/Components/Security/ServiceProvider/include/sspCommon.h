/**************************************************************************//**
  \file sspCommon.h

  \brief Declarations of common Security Service Provider's fields and types.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      06/12/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _SSPCOMMON_H
#define _SSPCOMMON_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
#define SECURITY_KEY_SIZE   16U
#define SECURITY_BLOCK_SIZE 16U
#define SECURITY_NONCE_SIZE 13U
#define SSP_SECURITY_CONTROL_FIELD_LENGTH 1U
#define SSP_FRAME_COUNTER_FIELD_LENGTH    4U
#define SSP_KEY_SEQ_NUMBER_FIELD_LENGTH   1U

/******************************************************************************
                        Types section.
******************************************************************************/
/**************************************************************************//**
  \brief Possible status values of different SSP primitives.
******************************************************************************/
typedef enum
{
  SSP_SUCCESS_STATUS,
  SSP_NOT_PERMITTED_STATUS,
  SSP_MAX_FRM_COUNTER_STATUS,
  SSP_BAD_CCM_OUTPUT_STATUS,
  SSP_INVALID_MIC_STATUS,
} SSP_Status_t;

/**************************************************************************//**
  \brief Internal service fields which give an opportunity to compose requests' queue.
******************************************************************************/
typedef struct
{
  void    *next;
  uint8_t requestId;
} SSP_Service_t;


/******************************************************************************
                      Prototypes section
 ******************************************************************************/
/*******************************************************************************
  Checks a security key.
  Parameters:
    key - valid pointer to a security key.
  Return:
    true - if key is not zero otherwise return false.
********************************************************************************/
bool SSP_IsKeyValid(const uint8_t *key);


#endif //_SSPCOMMON_H

// eof sspCommon.h
