
/***************************************************************************//**
  \file zsiHalSerialization.c

  \brief ZAppSI HAL primitives serialization.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiApsSerialization.c 24641 2013-03-22 14:26:58Z ViswanadhamK $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiHalSerialization.h>
#include <resetReason.h>
#include <zsiFrames.h>
#include <zsiDriver.h>
#include <zsiSerializer.h>
#include <sysUtils.h>
#include <zsiDbg.h>

#if defined (ZAPPSI_NP)
static void resetTimerFired(void);
static HAL_AppTimer_t resetTimer =
{
  .interval = 50,
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = resetTimerFired,
};
#endif



/******************************************************************************
                               Implementation section
 ******************************************************************************/

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief HAL Reset request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeHAL_ResetReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = true
  };
  uint16_t length = zsiHAL_ResetReqLength();
  uint8_t sequenceNumber = zsiGetSequenceNumber();

  zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SREQ_CMD, ZSI_CMD_HAL,
    ZSI_HAL_RESET_REQUEST);
  (void)req;
  return result;

}
/**************************************************************************//**
  \brief HAL-Reset confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeHAL_ResetConf(void *memory, ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = false,
    .keepMemory = true
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  return result;

}
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief HAL-Reset request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/

ZsiProcessingResult_t zsiDeserializeHAL_ResetReq(void *memory, ZsiCommandFrame_t *const cmdFrame)
{
  ZsiProcessingResult_t result =
  {
    .keepCmdFrame = true,
    .keepMemory = false
  };
  ZsiSerializer_t serializer =
  {
    .pow = cmdFrame->payload,
    .por = cmdFrame->payload
  };

  zsiDeserializeUint8(&serializer, memory);

  /*Hal function to reset the MCU */

  /* Prepare and send confirm at the same buffer as request */
  {
    uint16_t length = zsiHAL_ResetConfLength(NULL);
    uint8_t sequenceNumber = cmdFrame->sequenceNumber;
    uint8_t status = 0;
    zsiPrepareCommand(cmdFrame, length, sequenceNumber, ZSI_SRSP_CMD, ZSI_CMD_HAL,
                      ZSI_HAL_RESET_CONFIRM);

    zsiSerializeUint8(&serializer, (uint8_t)status);
  }
  HAL_StartAppTimer(&resetTimer);

  return result;
}
/**************************************************************************//**
  \brief ResetTimerFired routine to reset teh MCU
         routine.
 ******************************************************************************/
static void resetTimerFired(void)
{
  HAL_WarmReset();
}


#endif





