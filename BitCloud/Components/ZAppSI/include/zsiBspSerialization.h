/******************************************************************************
  \file zsiBspSerialization.h

  \brief ZAppSI BSP APIs serialization interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiApsSerialization.h 24441 2013-02-07 12:02:29Z Unithra C $
******************************************************************************/
#ifndef _ZSIBSPSERIALIZATION_H_
#define _ZSIBSPSERIALIZATION_H_
/******************************************************************************
                    Includes section
******************************************************************************/
#include <zsiFrames.h>
#include <zsiDriver.h>
#include <macAddr.h>

/**************************************************************************//**
  \brief Finds routine for BSP commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiBspFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief BSP read UID frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return BSP-ReadUID frame size.
 ******************************************************************************/
INLINE uint16_t zsiBSP_ReadUidReqLength()
{
  return ZSI_COMMAND_FRAME_OVERHEAD ;
}

/**************************************************************************//**
  \brief BSP_ReadUid confirm deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeBSP_ReadUidConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief BSP read uid request serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeBSP_ReadUidReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief BSP_ReadUid Request deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeBSP_ReadUidReq(void *memory, ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief BSP_ReadUidConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return BSP_ReadUidConf confirm frame size.
 ******************************************************************************/
INLINE uint16_t zsiBSP_ReadUidConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(ExtAddr_t);
}
#endif //ZAPPSI_HOST

#ifdef ZAPPSI_HOST
#define zsiDeserializeBSP_ReadUidReq                  NULL
#elif defined(ZAPPSI_NP)
#define zsiDeserializeBSP_ReadUidConf                 NULL
#endif /*ZAPPSI_HOST*/

#endif//_ZSIBSPSERIALIZATION_H_