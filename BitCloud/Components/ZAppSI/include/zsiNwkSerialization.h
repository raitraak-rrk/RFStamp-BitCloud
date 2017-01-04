/******************************************************************************
  \file zsiNwkSerialization.h

  \brief ZAppSI NWK primitives serialization interface.
         This file was automatically generated.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   Last change:
    $Id: zsiNwkSerialization.h 27088 2014-08-19 05:54:45Z mukesh.basrani $
******************************************************************************/

#ifndef _ZSINWKSERIALIZATION_H_
#define _ZSINWKSERIALIZATION_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <nwk.h>
#include <zsiFrames.h>
#include <zsiDriver.h>

/******************************************************************************
                    Types section
******************************************************************************/
#ifdef _SECURITY_
typedef struct _ZsiSetKey_t
{
  uint8_t key[SECURITY_KEY_SIZE];
  NWK_KeySeqNum_t keySeqNum;
} ZsiSetKey_t;
#endif
/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for NWK commands deserialization and processing.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiNwkFindProcessingRoutine(uint8_t commandId);

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief NWK-GetNextHop request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetNextHopReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetNextHop confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetNextHopConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetNextHop request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetNextHop request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetNextHopReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* shortAddr */
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetExtByShortAddressReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetExtByShortAddress request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetExtByShortAddressReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* shortAddr */
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetExtByShortAddressConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress request primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

      \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetShortByExtAddressReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress request frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetShortByExtAddress request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetShortByExtAddressReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint64_t); /* extAddr */
}

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortByExtAddressConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetUnicastDeliveryTimeReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return NWK-GetUnicastDeliveryTime request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetUnicastDeliveryTimeReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetUnicastDeliveryTimeConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetShortAddr primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_GetShortAddrReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetShortAddr frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return NWK-GetShortAddr request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiSerializeNWK_GetShortAddrReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}

/**************************************************************************//**
  \brief NWK-GetShortAddr confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortAddrConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
#ifdef _SECURITY_
/**************************************************************************//**
  \brief NWK-SetKey frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return NWK-SetKey request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiSerializeNWK_SetKeyReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + 
         SECURITY_KEY_SIZE +
         sizeof(uint8_t);   /* KeySeqNum*/
}
/**************************************************************************//**
  \brief NWK-SetKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_SetKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief NWK-SetKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_SetKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief NWK-ActivateKey primitive serialization routine.

  \param[in] req - request parameters.
  \param[out] cmdFrame - frame, which keeps serialized data.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiSerializeNWK_ActivateKeyReq(const void *const req,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief NWK-ActivateKey frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] ind - indication parameters.

  \return NWK-ActivateKey request frame size.
 ******************************************************************************/
INLINE uint16_t
zsiSerializeNWK_ActivateKeyReqLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD + 
         sizeof(uint8_t);   /* KeySeqNum*/;
}
/**************************************************************************//**
  \brief NWK-ActivateKey confirm primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_ActivateKeyConf(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
#endif /* _SECURITY_ */
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief NWK-GetNextHop request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetNextHopReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetNextHop confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetNextHop confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetNextHopConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* shortAddr */
}

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetExtByShortAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetExtByShortAddress confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetExtByShortAddress confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetExtByShortAddressConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint64_t); /* extAddr */
}

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress request primitive deserialization and processing
         routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortByExtAddressReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetShortByExtAddress confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetShortByExtAddress confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetShortByExtAddressConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* shortAddr */
}

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetUnicastDeliveryTimeReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-GetUnicastDeliveryTime confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-GetUnicastDeliveryTime confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetUnicastDeliveryTimeConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint32_t); /* deliveryTime */
}

/**************************************************************************//**
  \brief NWK-NWK_GetShortAdd request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

    \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_GetShortAddReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);

/**************************************************************************//**
  \brief NWK-NWK_GetShortAdd confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-NWK_GetShortAdd confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_GetShortAddConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD +
         sizeof(uint16_t); /* shortAddress */
}
#ifdef _SECURITY_
/**************************************************************************//**
  \brief NWK-SetKey request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_SetKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief NWK-SetKey confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-SetKey confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_SetKeyConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}
/**************************************************************************//**
  \brief NWK-SetKey request primitive deserialization and
         processing routine.

  \param[out] memory - memory allocated for command processing.
  \param[in] cmdFrame - frame which keeps serialized command.

  \return Processing result.
 ******************************************************************************/
ZsiProcessingResult_t zsiDeserializeNWK_ActivateKeyReq(void *memory,
  ZsiCommandFrame_t *const cmdFrame);
/**************************************************************************//**
  \brief NWK-ActivateKey confirm frame size calculation routine.
         SOF and LENGTH fields are dismissed.

  \param[in] req - request parameters.

  \return NWK-ActivateKey confirm frame size.
 ******************************************************************************/
INLINE uint16_t
zsiNWK_ActivateKeyConfLength(const void *const req)
{
  (void)req;
  return ZSI_COMMAND_FRAME_OVERHEAD;
}
#endif /* _SECURITY_ */
#endif /* ZAPPSI_NP */

#if defined(ZAPPSI_HOST)
#define zsiDeserializeNWK_GetShortByExtAddressReq   NULL
#define zsiDeserializeNWK_GetNextHopReq             NULL
#define zsiDeserializeNWK_GetUnicastDeliveryTimeReq NULL
#define zsiDeserializeNWK_GetExtByShortAddressReq   NULL
#define zsiDeserializeNWK_GetShortAddReq            NULL
#define zsiDeserializeNWK_SetKeyReq                 NULL
#define zsiDeserializeNWK_ActivateKeyReq            NULL
#elif defined(ZAPPSI_NP)
#define zsiDeserializeNWK_GetShortByExtAddressConf   NULL
#define zsiDeserializeNWK_GetNextHopConf             NULL
#define zsiDeserializeNWK_GetUnicastDeliveryTimeConf NULL
#define zsiDeserializeNWK_GetExtByShortAddressConf   NULL
#define zsiDeserializeNWK_GetShortAddrConf           NULL
#define zsiDeserializeNWK_SetKeyConf                 NULL
#define zsiDeserializeNWK_ActivateKeyConf            NULL

#endif /* ZAPPSI_NP */

#endif /* _ZSIAPSSERIALIZATION_H_ */
