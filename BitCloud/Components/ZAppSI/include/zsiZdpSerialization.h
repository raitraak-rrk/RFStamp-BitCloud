/******************************************************************************
  \file zsiZdpSerialization.h

  \brief ZAppSI ZDP primitives serialization interface.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiZdpSerialization.h 28222 2015-07-24 11:38:43Z unithra.c $
******************************************************************************/

#ifndef _ZSIZDPSERIALIZATION_H_
#define _ZSIZDPSERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zdo.h>
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for ZDP commands serialization.

  \param[in] clusterId - ZDP cluster identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiSerializeRoutine_t zsiZdpFindSerializationRoutine(uint16_t clusterId);

/**************************************************************************//**
  \brief Finds routine for ZDP commands deserialization and processing.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiZdpFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief ZDP-IeeeAddress.Reqest primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpIeeeAddressReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDP-IeeeAddressReq frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpIeeeAddressReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +            /* nwkAddrOfInterest */
         sizeof(uint8_t)  +            /* reqType */
         sizeof(uint8_t);              /* startIndex */
}

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpIeeeAddressResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-PermitJoining.Reqest primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpPermitJoiningReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-PermitJoining.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDP-PermitJoiningReq frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpPermitJoiningReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t) +             /* permitDuration */
         sizeof(uint8_t);              /* tcSignificance */
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpPermitJoiningResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpSimpleDescriptorReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDP-SimpleDescriptorReq request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpSimpleDescriptorReqLength(void)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +                       /* nwkAddrOfInterest */
         sizeof(uint8_t);                        /* endpoint */
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpSimpleDescriptorResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#ifdef _BINDING_
/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpMatchDescriptorReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDP-MatchDescriptorReq request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpMatchDescriptorReqLength(const ZDO_MatchDescReq_t *const req)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +                       /* nwkAddrOfInterest */
         sizeof(uint16_t) +                       /* profileId */
         sizeof(uint8_t)  +                       /* numInClusters */
         req->numInClusters * sizeof(uint16_t)  + /* inClusterList */
         sizeof(uint8_t)  +                       /* numOutClusters */
         req->numOutClusters * sizeof(uint16_t);  /* inClusterList */
}

/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpMatchDescriptorResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-Bind.Reqest primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpBindReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-Bind.Request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return ZDP-Bind.Request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpBindReqLength(const ZDO_BindReq_t *const req)
{
  uint16_t length = ZSI_COMMAND_FRAME_OVERHEAD +
                    sizeof(uint64_t) + /* srcAddr */
                    sizeof(uint8_t) +  /* srcEndpoint */
                    sizeof(uint16_t) + /* clusterId */
                    sizeof(uint8_t);   /* dstAddrMode */

  if (APS_EXT_ADDRESS == req->dstAddrMode)
  {
    length += sizeof(uint64_t) + /* dstExtAddr */
              sizeof(uint8_t);   /* dstEndpoint */
  }
  else
    length += sizeof(uint16_t);  /* dstGroupAddr */

  return length;
}

/**************************************************************************//**
  \brief ZDP-Bind.Response primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpBindResp(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

#endif /* _BINDING_ */

#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief ZDP-IeeeAddress.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpIeeeAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpIeeeAddressResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-IeeeAddress.Response frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] resp - response parameters.

  \return ZDP-IeeeAddress.Response request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpIeeeAddressRespLength(const ZDO_IeeeAddrResp_t *const resp)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint64_t) +                    /* ieeeAddrRemote */
         sizeof(uint16_t)  +                   /* nwkAddrRemote */
         sizeof(uint8_t)  +                    /* numAssocDev */
         sizeof(uint8_t)  +                    /* startIndex */
         resp->numAssocDev * sizeof(uint16_t); /* nwkAddrAssocDevList */
}

/**************************************************************************//**
  \brief ZDP-PermitJoining.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpPermitJoiningReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-PermitJoining.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpPermitJoiningResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-PermitJoining.Response frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] resp - response parameters.

  \return ZDP-PermitJoining.Response request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpPermitJoiningRespLength(const void *const resp)
{
  (void)resp;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpSimpleDescriptorReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpSimpleDescriptorResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-SimpleDescriptor.Response frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] resp - response parameters.

  \return ZDP-SimpleDescriptor.Response request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpSimpleDescriptorRespLength(const ZDO_SimpleDescResp_t *const resp)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +                   /* nwkAddrOfInterest */
         sizeof(uint8_t)  +                   /* simpleDescLength */
         resp->length * sizeof(uint8_t);      /* simple desc response payload */
}

#ifdef _BINDING_
/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpMatchDescriptorReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpMatchDescriptorResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-MatchDescriptor.Response frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] resp - response parameters.

  \return ZDP-MatchDescriptor.Response request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpMatchDescriptorRespLength(const ZDO_MatchDescResp_t *const resp)
{
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t) +                   /* nwkAddrOfInterest */
         sizeof(uint8_t)  +                   /* matchLength */
         resp->matchLength * sizeof(uint8_t); /* matchList */
}

/**************************************************************************//**
  \brief ZDP-Bind.Request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeZdpBindReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-Bind.Response primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeZdpBindResp(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief ZDP-Bind.Response frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] resp - response parameters.

  \return ZDP-Bind.Response request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiZdpBindRespLength(const void *const resp)
{
  (void)resp;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint8_t); /* status */

}

#endif /* _BINDING_ */

#endif /* ZAPPSI_NP */

#ifdef ZAPPSI_HOST
#define zsiDeserializeZdpMatchDescriptorReq  NULL
#define zsiDeserializeZdpSimpleDescriptorReq NULL
#define zsiDeserializeZdpIeeeAddressReq      NULL
#define zsiDeserializeZdpPermitJoiningReq    NULL
#define zsiDeserializeZdpBindReq             NULL
#ifndef _BINDING_
#define zsiSerializeZdpMatchDescriptorReq    NULL
#define zsiSerializeZdpBindReq               NULL
#define zsiDeserializeZdpMatchDescriptorResp NULL
#define zsiDeserializeZdpBindResp            NULL
#endif /* _BINDING_ */

#elif defined(ZAPPSI_NP)
#define zsiDeserializeZdpMatchDescriptorResp NULL
#define zsiDeserializeZdpSimpleDescriptorResp NULL
#define zsiDeserializeZdpIeeeAddressResp     NULL
#define zsiDeserializeZdpPermitJoiningResp   NULL
#define zsiDeserializeZdpBindResp            NULL
#ifndef _BINDING_
#define zsiDeserializeZdpMatchDescriptorReq  NULL
#define zsiDeserializeZdpBindReq             NULL
#define zsiSerializeZdpMatchDescriptorResp   NULL
#define zsiSerializeZdpBindResp              NULL
#endif /* _BINDING_ */

#endif /* ZAPPSI_NP */

#endif /* _ZSIZDPSERIALIZATION_H_ */
