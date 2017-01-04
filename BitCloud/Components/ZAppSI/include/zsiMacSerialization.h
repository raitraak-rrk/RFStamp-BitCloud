/******************************************************************************
  \file zsiMacSerialization.h

  \brief ZAppSI Mac primitives serialization interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
  14.01.2013  A. Khromykh - Created.
  Last change:
  $Id: zsiMacSerialization.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/
#ifndef _ZSIMACSERIALIZATION_H
#define _ZSIMACSERIALIZATION_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <rfRandom.h>
#include <macEnvironment.h>
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for MAC commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiMacFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief MAC_IsOwnExtAddrReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return MAC_IsOwnExtAddrReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiMAC_IsOwnExtAddrReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(ExtAddr_t);
}

/**************************************************************************//**
  \brief MAC_GetExtAddrReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return MAC_GetExtAddrReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiMAC_GetExtAddrReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief RF_RandomReq frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return RF_RandomReq request frame size.
 ******************************************************************************/
INLINE uint16_t zsiRF_RandomReq(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief MAC_IsOwnExtAddrReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeMAC_IsOwnExtAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief MAC_IsOwnExtAddrConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_IsOwnExtAddrConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief MAC_GetExtAddrReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeMAC_GetExtAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief MAC_GetExtAddrConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_GetExtAddrConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief RF_RandomReq primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeRF_RandomReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief RF_RandomConf confirm primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeRF_RandomConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#elif defined(ZAPPSI_NP)

/**************************************************************************//**
  \brief MAC_IsOwnExtAddrConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return MAC_IsOwnExtAddrConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiMAC_IsOwnExtAddrConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(bool);
}

/**************************************************************************//**
  \brief MAC_GetExtAddrConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return MAC_GetExtAddrConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiMAC_GetExtAddrConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(ExtAddr_t);
}

/**************************************************************************//**
  \brief RF_RandomConf frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return MAC_GetExtAddrConf request frame size.
 ******************************************************************************/
INLINE uint16_t zsiRF_RandomConf(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + sizeof(uint16_t);
}

/**************************************************************************//**
  \brief MAC_IsOwnExtAddrReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_IsOwnExtAddrReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief MAC_GetExtAddrReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeMAC_GetExtAddrReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief RF_RandomReq request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
*******************************************************************************/
ZsiProcessingResult_t zsiDeserializeRF_RandomReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief RF_RandomConf primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeRF_RandomConf(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
\brief Confirm for RF_RandomReq.

\param conf - pointer to confirm with result.
******************************************************************************/
void zsiRfRandomConf(RF_RandomConf_t *conf);

#endif /* ZAPPSI_NP */


#ifdef ZAPPSI_HOST
#define zsiDeserializeMAC_IsOwnExtAddrReq            NULL
#define zsiDeserializeMAC_GetExtAddrReq              NULL
#define zsiDeserializeRF_RandomReq                   NULL

#elif defined(ZAPPSI_NP)
#define zsiDeserializeMAC_IsOwnExtAddrConf           NULL
#define zsiDeserializeMAC_GetExtAddrConf             NULL
#define zsiDeserializeRF_RandomConf                  NULL

#endif /* ZAPPSI_NP */

#endif /* _ZSIMACSERIALIZATION_H */
