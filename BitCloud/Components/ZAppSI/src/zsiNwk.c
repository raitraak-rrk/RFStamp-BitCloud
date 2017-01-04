/***************************************************************************//**
  \file zsiNwk.c

  \brief ZAppSI NWK API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-26  A. Razinkov - Created.
   Last change:
    $Id: zsiNwk.c 27088 2014-08-19 05:54:45Z mukesh.basrani $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiNwkSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiNwkProcessingRoutines[]) =
{
  [ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_REQUEST] = zsiDeserializeNWK_GetShortByExtAddressReq,
  [ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_CONFIRM] = zsiDeserializeNWK_GetShortByExtAddressConf,
  [ZSI_NWK_GET_NEXT_HOP_REQUEST] = zsiDeserializeNWK_GetShortByExtAddressReq,
  [ZSI_NWK_GET_NEXT_HOP_CONFIRM] = zsiDeserializeNWK_GetShortByExtAddressConf,
  [ZSI_NWK_GET_UNICAST_DELIVERY_TIME_REQUEST] = zsiDeserializeNWK_GetUnicastDeliveryTimeReq,
  [ZSI_NWK_GET_UNICAST_DELIVERY_TIME_CONFIRM] = zsiDeserializeNWK_GetUnicastDeliveryTimeConf,
  [ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_REQUEST] = zsiDeserializeNWK_GetExtByShortAddressReq,
  [ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_CONFIRM] = zsiDeserializeNWK_GetExtByShortAddressConf,
  [ZSI_NWK_GET_SHORT_ADDRESS_REQUEST] = zsiDeserializeNWK_GetShortAddReq,
  [ZSI_NWK_GET_SHORT_ADDRESS_CONFIRM] = zsiDeserializeNWK_GetShortAddrConf
#ifdef _SECURITY_
    ,
  [ZSI_NWK_SET_KEY_REQUEST] = zsiDeserializeNWK_SetKeyReq,
  [ZSI_NWK_SET_KEY_CONFIRM] = zsiDeserializeNWK_SetKeyConf,
  [ZSI_NWK_ACTIVATE_KEY_REQUEST] = zsiDeserializeNWK_ActivateKeyReq,
  [ZSI_NWK_ACTIVATE_KEY_CONFIRM] = zsiDeserializeNWK_ActivateKeyConf
#endif /* _SECURITY_ */
};

#ifdef ZAPPSI_HOST
static ShortAddr_t zsiShortAddr;
static ExtAddr_t zsiExtAddr;
#endif /* ZAPPSI_HOST */

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for NWK commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiNwkFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiNwkProcessingRoutines))
  {
    memcpy_P(&routine, &zsiNwkProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Wrapper routine for linear search of entry with the given network
         address in the address map and the neighbor table.

  \param[in] dstAddr - network address of the target device.

  \return if a short address is found the this function return pointer to this
    address otherwise NULL.
 ******************************************************************************/
ShortAddr_t NWK_GetNextHop(ShortAddr_t dstAddr)
{
  zsiProcessCommand(ZSI_SREQ_CMD, (ShortAddr_t *)&dstAddr,
                    zsiSerializeNWK_GetNextHopReq, &zsiShortAddr);
  return zsiShortAddr;
}

/**************************************************************************//**
  \brief Wrapper routine for linear search of entry with the given short
         address in the address map and the neighbor table.

  \param[in] shortAddr - a short address.

  \return pointer to the extended address if it has been found, NULL otherwise.
 ******************************************************************************/
const ExtAddr_t* NWK_GetExtByShortAddress(const ShortAddr_t shortAddr)
{
  zsiProcessCommand(ZSI_SREQ_CMD, (ShortAddr_t *)&shortAddr,
                    zsiSerializeNWK_GetExtByShortAddressReq, &zsiExtAddr);
  return &zsiExtAddr;
}

/**************************************************************************//**
  \brief Wrapper routine for linear search of entry with the given extended
         address in the address map and the neighbor table.

  \param[in] extAddr - pointer to an extended (64-bit) address.

  \return if a short address is found the this function return pointer to this
    address otherwise NULL.
 ******************************************************************************/
const ShortAddr_t *NWK_GetShortByExtAddress(const ExtAddr_t *const extAddr)
{
  zsiProcessCommand(ZSI_SREQ_CMD, (ExtAddr_t *)extAddr,
                    zsiSerializeNWK_GetShortByExtAddressReq, &zsiShortAddr);
  return &zsiShortAddr;
}

/**************************************************************************//**
  \brief Wrapper routine to link short network and IEEE extended address.

  \param[in] shortAddr - a short address.
  \param[in] extAddr - an extended address.
  \return 'true' if short and extended addresses are linked successfully
    otherwise 'false'.
 ******************************************************************************/
bool NWK_LinkShortAndExtAddress(const ShortAddr_t shortAddr,
  const ExtAddr_t extAddr)
{
  sysAssert(0U, 0xFFFF);
  (void)shortAddr;
  (void)extAddr;
  return false;
}

/**************************************************************************//**
  \brief Wrapper routine to get medium delivery time of unicast data
    transmission on a network.

  \return Current value of unicast delivery time in milliseconds.
 ******************************************************************************/
uint32_t NWK_GetUnicastDeliveryTime(void)
{
  uint32_t deliveryTime;
  ZsiEntityService_t service;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeNWK_GetUnicastDeliveryTimeReq, &deliveryTime);

  return deliveryTime;
}

/**************************************************************************//**
  \brief Wrapper routine to get own short address.

  \return own short address.
 ******************************************************************************/
ShortAddr_t NWK_GetShortAddr(void)
{
  ShortAddr_t addr;
  bool service = true;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeNWK_GetShortAddrReq, &addr);

  return addr;
}
#ifdef _SECURITY_

/**************************************************************************//**
  \brief Wrapper routine to set a new network key.

  \param[in] key - a new network security key.
  \param[in] keySeqNum - key sequence number.

  \return 'true' if operation was completed successfully otherwise 'false'.
 ******************************************************************************/
bool NWK_SetKey(const uint8_t *const key, const NWK_KeySeqNum_t keySeqNum)
{
  ZsiSetKey_t nwkSetKey;
  sysAssert(0U, 0xFFFF);

  memcpy(nwkSetKey.key, key, SECURITY_KEY_SIZE);
  nwkSetKey.keySeqNum = keySeqNum;

  zsiProcessCommand(ZSI_SREQ_CMD, &nwkSetKey, zsiSerializeNWK_SetKeyReq, NULL);
  return true;
}

/**************************************************************************//**
  \brief Wrapper routine for network key switching. Set NWK key as active.

  \param[in] keySeqNum - key sequence number.
  \return 'true' if operation was completed successfully otherwise 'false'.
 ******************************************************************************/
bool NWK_ActivateKey(const NWK_KeySeqNum_t keySeqNum)
{
  uint8_t nwkKeySeqNum = keySeqNum;
  sysAssert(0U, 0xFFFF);
  zsiProcessCommand(ZSI_SREQ_CMD, &nwkKeySeqNum, zsiSerializeNWK_ActivateKeyReq, NULL);
  return true;
}

#endif /* _SECURITY_ */

#elif defined(ZAPPSI_NP)

#endif /* ZAPPSI_NP */

/* eof zsiNwk.c */
