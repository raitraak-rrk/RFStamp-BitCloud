/***************************************************************************//**
  \file zsiZdp.c

  \brief ZAppSI ZDP API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-27  A. Razinkov - Created.
   Last change:
    $Id: zsiZdp.c 28222 2015-07-24 11:38:43Z unithra.c $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiZdpSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiZdpProcessingRoutines[]) =
{
  [ZSI_ZDP_MATCH_DESCRIPTOR_REQUEST]  = zsiDeserializeZdpMatchDescriptorReq,
  [ZSI_ZDP_MATCH_DESCRIPTOR_RESPONSE] = zsiDeserializeZdpMatchDescriptorResp,
  [ZSI_ZDP_SIMPLE_DESCRIPTOR_REQUEST] = zsiDeserializeZdpSimpleDescriptorReq,
  [ZSI_ZDP_SIMPLE_DESCRIPTOR_RESPONSE]= zsiDeserializeZdpSimpleDescriptorResp,
  [ZSI_ZDP_IEEE_ADDRESS_REQUEST]      = zsiDeserializeZdpIeeeAddressReq,
  [ZSI_ZDP_IEEE_ADDRESS_RESPONSE]     = zsiDeserializeZdpIeeeAddressResp,
  [ZSI_ZDP_BIND_REQUEST]              = zsiDeserializeZdpBindReq,
  [ZSI_ZDP_BIND_RESPONSE]             = zsiDeserializeZdpBindResp,
  [ZSI_ZDP_PERMIT_JOINING_REQUEST]    = zsiDeserializeZdpPermitJoiningReq,
  [ZSI_ZDP_PERMIT_JOINING_RESPONSE]   = zsiDeserializeZdpPermitJoiningResp
};

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for ZDP commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiZdpFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiZdpProcessingRoutines))
  {
    memcpy_P(&routine, &zsiZdpProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Finds routine for ZDP commands serialization (HOST side).

  \param[in] clusterId - ZDP cluster identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiSerializeRoutine_t zsiZdpFindSerializationRoutine(uint16_t clusterId)
{
  ZsiSerializeRoutine_t routine = NULL;
  switch (clusterId)
  {
    case SIMPLE_DESCRIPTOR_CLID:
      routine = zsiSerializeZdpSimpleDescriptorReq;
      break;

    case MATCH_DESCRIPTOR_CLID:
      routine = zsiSerializeZdpMatchDescriptorReq;
      break;

    case IEEE_ADDR_CLID:
      routine = zsiSerializeZdpIeeeAddressReq;
      break;

    case BIND_CLID:
      routine = zsiSerializeZdpBindReq;
      break;

    case MGMT_PERMIT_JOINING_CLID:
      routine = zsiSerializeZdpPermitJoiningReq;
      break;

    default:
      break;
  }

  return routine;
}
#elif defined(ZAPPSI_NP)
/**************************************************************************//**
  \brief Finds routine for ZDP commands serialization (NP side).

  \param[in] clusterId - ZDP cluster identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiSerializeRoutine_t zsiZdpFindSerializationRoutine(uint16_t clusterId)
{
  ZsiSerializeRoutine_t routine = NULL;
  switch (clusterId)
  {
    case SIMPLE_DESCRIPTOR_CLID:
      routine = zsiSerializeZdpSimpleDescriptorResp;
      break;

    case MATCH_DESCRIPTOR_CLID:
      routine = zsiSerializeZdpMatchDescriptorResp;
      break;

    case IEEE_ADDR_CLID:
      routine = zsiSerializeZdpIeeeAddressResp;
      break;

    case BIND_CLID:
      routine = zsiSerializeZdpBindResp;
      break;

    case MGMT_PERMIT_JOINING_CLID:
      routine = zsiSerializeZdpPermitJoiningResp;
      break;

    default:
      break;
  }

  return routine;
}

#endif /* ZAPPSI_HOST */

/* eof zsiZdp.c */
