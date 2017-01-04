/******************************************************************************
  \file zsiHalSerialization.h

  \brief ZAppSI HAL primitives serialization interface.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiApsSerialization.h 24441 2013-02-07 12:02:29Z Viswanadham Kotla $
******************************************************************************/

#ifndef _ZSIHALSERIALIZATION_H_
#define _ZSIHALSERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for HAL commands deserialization and processing.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiHalFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief HAL Reset request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeHAL_ResetReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief HAL-Reset confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeHAL_ResetConf(void *memory, ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief HAL-Reset request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return HAL-Reset request frame size.
 ******************************************************************************/
INLINE uint16_t zsiHAL_ResetReqLength()
{
  return ZSI_COMMAND_FRAME_OVERHEAD ;
}

/**************************************************************************//**
  \brief ZSI_ResetNP handler to send the serialized HAL reset req command 
   to reset the NP in case of Host - ZAPPSINP

  \return None.
 ******************************************************************************/
void ZSI_ResetNP(void);

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief HAL-Reset Confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return HAL-Reset Confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiHAL_ResetConfLength(const void *const confirm)
{
  (void)confirm;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t);  /* status */
}

/**************************************************************************//**
  \brief HAL-Reset request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeHAL_ResetReq(void *memory, ZsiCommandFrame_t *const cmdFrame);

#endif // ZAPPSI_NP


#ifdef ZAPPSI_HOST
#define zsiDeserializeHAL_ResetReq     NULL
#elif defined(ZAPPSI_NP)
#define zsiDeserializeHAL_ResetConf    NULL
#endif


#endif  //_ZSIHALSERIALIZATION_H_
