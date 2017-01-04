/***************************************************************************//**
  \file zsiMac.c

  \brief ZAppSI MAC API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    14.01.2013  A. Khromykh - Created.
   Last change:
    $Id: zsiMac.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiDriver.h>
#include <zsiDbg.h>
#include <sysQueue.h>
#include <zsiMacSerialization.h>

/******************************************************************************
                   External global variables section
******************************************************************************/
#if defined(ZAPPSI_HOST)

#elif  defined(ZAPPSI_NP)
extern RF_RandomReq_t *zsiRandomReq;
#endif

/******************************************************************************
                   Local variables section
******************************************************************************/
#ifdef ZAPPSI_HOST
static ExtAddr_t zsiExtAddrCopy;
#elif defined(ZAPPSI_NP)

#endif /* ZAPPSI_NP */

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiMacProcessingRoutines[]) =
{
  [ZSI_MAC_IS_OWN_EXT_ADDR_REQ]   = zsiDeserializeMAC_IsOwnExtAddrReq,
  [ZSI_MAC_IS_OWN_EXT_ADDR_CONF]  = zsiDeserializeMAC_IsOwnExtAddrConf,
  [ZSI_MAC_GET_EXT_ADDR_REQ]      = zsiDeserializeMAC_GetExtAddrReq,
  [ZSI_MAC_GET_EXT_ADDR_CONF]     = zsiDeserializeMAC_GetExtAddrConf,
  [ZSI_RF_RANDOM_REQ]             = zsiDeserializeRF_RandomReq,
  [ZSI_RF_RANDOM_CONF]            = zsiDeserializeRF_RandomConf,
};

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for MAC commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiMacFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiMacProcessingRoutines))
  {
    memcpy_P(&routine, &zsiMacProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Checks, if extended address is equal to our own one
  \param extAddr - pointer to extended address to check

  \return true, if extended address is equal to our own one; false - otherwise
 ******************************************************************************/
bool MAC_IsOwnExtAddr(const ExtAddr_t *const extAddr)
{
  bool result;

  zsiProcessCommand(ZSI_SREQ_CMD, (void *const)extAddr, zsiSerializeMAC_IsOwnExtAddrReq, &result);

  return result;
}

/**************************************************************************//**
  \brief Gets pointer to extended address.

  \return pointer to extended address.
 ******************************************************************************/
const ExtAddr_t *MAC_GetExtAddr(void)
{
  ZsiEntityService_t service;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeMAC_GetExtAddrReq, &zsiExtAddrCopy);

  return &zsiExtAddrCopy;
}

/**************************************************************************//**
  \brief Requests random value from RF chip.

  \param reqParams - request parameters structure pointer.
******************************************************************************/
void RF_RandomReq(RF_RandomReq_t *reqParams)
{
  zsiProcessCommand(ZSI_AREQ_CMD, reqParams, zsiSerializeRF_RandomReq, NULL);
}

#elif defined(ZAPPSI_NP)

/**************************************************************************//**
\brief Confirm for RF_RandomReq.

\param conf - pointer to confirm with result.
******************************************************************************/
void zsiRfRandomConf(RF_RandomConf_t *conf)
{
  (void)conf;
  zsiProcessCommand(ZSI_AREQ_CMD, zsiRandomReq, zsiSerializeRF_RandomConf, NULL);
}

#endif /* ZAPPSI_NP */

/* eof zsiMac.c */

