/******************************************************************************
  \file zsiSysSerialization.h

  \brief ZAppSI system configuration primitives serialization interface.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiSysSerialization.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/

#ifndef _ZSISYSSERIALIZATION_H_
#define _ZSISYSSERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <appFramework.h>
#include <configServer.h>
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZsiCsParameter_t
{
  CS_MemoryItemId_t parameterId;
  uint16_t          size;
  uint8_t           payload[CS_MAX_PARAMETER_SIZE];
} ZsiCsParameter_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for SYS command deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiSysFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief CS_ReadParameter request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeCS_ReadParameterReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief CS_WriteParameter request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeCS_WriteParameterReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief CS_ReadParameter request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return CS_WriteParameter frame size.
 ******************************************************************************/
INLINE uint16_t
zsiCS_ReadParameterReqLength(const CS_MemoryItemId_t *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t);  /* parameterId */
}

/**************************************************************************//**
  \brief CS_WriteParameter request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return CS_WriteParameter frame size.
 ******************************************************************************/
INLINE uint16_t
zsiCS_WriteParameterReqLength(const ZsiCsParameter_t *const req)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) + /* parameterId */
         sizeof(uint16_t) + /* size */
         req->size;         /* payload */
}

/**************************************************************************//**
  \brief CS_WriteParameter confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_WriteParameterConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief CS_ReadParameter confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_ReadParameterConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#elif defined(ZAPPSI_NP)
;/**************************************************************************//**
  \brief CS_ReadParameter request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_ReadParameterReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief CS_ReadParameter confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return CS_WriteParameter frame size.
 ******************************************************************************/
INLINE uint16_t
zsiCS_ReadParameterConfLength(const ZsiCsParameter_t *const req)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +  /* parameterId */
         sizeof(uint16_t) +  /* size */
         req->size;          /* payload */
}

/**************************************************************************//**
  \brief CS_WriteParameter request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeCS_WriteParameterReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief CS_WriteParameter confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] conf - confirm parameters.

  \return CS_WriteParameter frame size.
 ******************************************************************************/
INLINE uint16_t
zsiCS_WriteParameterConfLength(const void *const conf)
{
  (void)conf;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

#endif /* ZAPPSI_NP */

#ifdef ZAPPSI_HOST
#define zsiDeserializeCS_WriteParameterReq NULL
#define zsiDeserializeCS_ReadParameterReq  NULL

#elif defined(ZAPPSI_NP)
#define zsiDeserializeCS_WriteParameterConf NULL
#define zsiDeserializeCS_ReadParameterConf  NULL

#endif /* ZAPPSI_NP */

#endif /* _ZSISYSSERIALIZATION_H_ */
