/**************************************************************************//**
 \file nwkAttributes.h

 \brief Interface of network attributes.

 \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-05-01 Max Gekk - Created
   Last change:
    $Id: nwkAttributes.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_ATTRIBUTES_H
#define _NWK_ATTRIBUTES_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <appFramework.h>
#include <nwkCommon.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief This function return a short address that is allocated by parent.

  \return Short address of this node or NWK_NO_SHORT_ADDR.
 ******************************************************************************/
ShortAddr_t NWK_GetShortAddr(void);

/**************************************************************************//**
  \brief Setting new short address of this device.

  \param[in] shortAddr - new short (16-bit) address.
  \param[in] silent - report (false) or not (true) to upper-layer.

  \return NWK_SUCCESS_STATUS - if this request is valid
     otherwise NWK_NOT_PERMITTED_STATUS.
 ******************************************************************************/
NWK_Status_t NWK_SetShortAddr(const ShortAddr_t shortAddr, const bool silent);

/**************************************************************************//**
  \brief This function return a short address of parent.

  \return Short address of parent device or NWK_NO_SHORT_ADDR
 ******************************************************************************/
ShortAddr_t NWK_GetParentShortAddr(void);

/**************************************************************************//**
  \brief This function return a short panid of network.

  \return short panid or 0xFFFF
 ******************************************************************************/
PanId_t NWK_GetPanId(void);

/**************************************************************************//**
  \brief Setting a short Id of PAN.

  \return short (16-bit) pan id.
 ******************************************************************************/
void NWK_SetPanId(PanId_t panId);

/**************************************************************************//**
  \brief This function return a extended panid of network.

  \return extended panid or 0
 ******************************************************************************/
ExtPanId_t NWK_GetExtPanId(void);

/**************************************************************************//**
  \brief Set a NWK extended PAN ID parameter value.

  \param[in] extPanId - pointer to new NWK extended PAN ID parameter value.
  \return None.
 ******************************************************************************/
void NWK_SetExtPanId(const ExtPanId_t *extPanId);

/**************************************************************************//**
  \brief Current RF channel.
 ******************************************************************************/
Channel_t NWK_GetLogicalChannel(void);

/**************************************************************************//**
  \brief Setting a new RF channel.

  \param[in] channel - the number of new channel.
 ******************************************************************************/
void NWK_SetLogicalChannel(const Channel_t channel);

/**************************************************************************//**
  \brief Maximum delivery time of broadcasting transmission on a network.

    Distination nodes of broadcasting transmission are only full function devices.

  \return Current value of broadcast delivery time in milliseconds.
 ******************************************************************************/
uint32_t NWK_GetBroadcastDeliveryTime(void);

/**************************************************************************//**
  \brief Medium delivery time of unicast packet to a sleep end device.

  \return Current value of unicast delivery time in milliseconds.
 ******************************************************************************/
uint32_t NWK_GetUnicastDeliveryTime(void);

/**************************************************************************//**
  \brief Set a short address of the network manager.

  \param[in] - short address of the network manager.
 ******************************************************************************/
void NWK_SetManagerAddr(const ShortAddr_t shortAddr);

/**************************************************************************//**
  \brief Getting the short address of the network manager.

  \return Network manager address.
 ******************************************************************************/
ShortAddr_t NWK_GetManagerAddr(void);

/**************************************************************************//**
  \brief Set an id of network snapshot.

  \param[in] - new value of the update identifier.
 ******************************************************************************/
void NWK_SetUpdateId(const NwkUpdateId_t updateId);

/**************************************************************************//**
  \brief Getting an id of network snapshot.

  \return Network snapshot id
 ******************************************************************************/
NwkUpdateId_t NWK_GetUpdateId(void);

/**************************************************************************//**
  \brief Sets symmetric link attribute value.

  \param[in] symLink - new symmetric link attribute value.
  \return None.
 ******************************************************************************/
void NWK_SetSymLink(const bool symLink);

/**************************************************************************//**
  \brief Changing the route fail order.

    After 2^order failure attempts a route entry will be deleted.

  \param[in] order - new order of route fail.
  \return NWK_SUCCESS_STATUS if order is valid
          otherwise NWK_INVALID_PARAMETER_STATUS.
 ******************************************************************************/
NWK_Status_t NWK_SetFailRouteOrder(const uint8_t order);

/**************************************************************************//**
  \brief Gets attribute value which specifies if static addressing mode is in use

  \return True, if static addressing mode is in use; false - otherwise
 ******************************************************************************/
bool NWK_IsStaticAddressingMode(void);

/**************************************************************************//**
  \brief Gets protocol version attribute value.

  \return Protocol version attribute value.
 ******************************************************************************/
uint8_t NWK_GetProtocolVersion(void);

/**************************************************************************//**
  \brief Getting the total number of transmissions

  \return Value of total number of transmissions
 ******************************************************************************/
uint16_t NWK_GetTransmitCouner(void);

/**************************************************************************//**
  \brief Getting the transmission failures counter.

  \return Value of transmission failures counter
 ******************************************************************************/
uint16_t NWK_GetTransmitFailureCounter(void);

/**************************************************************************//**
  \brief Resets the transmission failures counter and the total tx counter.
 ******************************************************************************/
void NWK_ResetTransmitCounters(void);

/**************************************************************************//**
  \brief Getting a device type parameter value.

  \return device type parameter value.
 ******************************************************************************/
DeviceType_t NWK_GetDeviceType(void);

/**************************************************************************//**
  \brief Perform comparision of new newNwkUpdateId with the current value.

  \param[in] newNwkUpdateId - network updateId to be tested.

  \return true if new network updateId is more recent, false otherwise.
 ******************************************************************************/
bool NWK_IsNewUpdateIdMoreRecent(NwkUpdateId_t newNwkUpdateId);

/**************************************************************************//**
  \brief Check, if specified nwkUpdateId is equal or higher than current one.

  \param[in] newNwkUpdateId - network updateId to be tested.

  \return true if specified nwkUpdateId is equal or higher then current one,
          false - otherwise.
 ******************************************************************************/
bool NWK_IsNewUpdateIdNonLess(NwkUpdateId_t newNwkUpdateId);

#endif /* _NWK_ATTRIBUTES_H */
/** eof nwkAttributes.h */

