/**************************************************************************//**
  \file apsmeSwitchKey.h

  \brief Interface of APS Switch Key Service.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-11-24 Max Gekk - Created.
   Last change:
    $Id: apsmeSwitchKey.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APSME_SWITCH_KEY_H
#define _APSME_SWITCH_KEY_H
/**//**
 *
 *  The APS layer provides services that allow a device (typically,
 * the trust center) to inform another device that it should be switched to a new
 * active network key. See ZigBee Specification r19, 4.4.7, page 467.
 *
 **/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsCommon.h>

#if defined _SECURITY_
/******************************************************************************
                                Types section
 ******************************************************************************/
/** Status of creating and sending a switch key command frame. */
typedef enum
{
  APS_SWITCH_SUCCESS_STATUS          = 0x00,
  APS_SWITCH_NO_SHORT_ADDRESS_STATUS = 0x01,
  APS_SWITCH_SECURITY_FAIL_STATUS    = 0x02,
  APS_SWITCH_NOT_SENT_STATUS         = 0x03,
  /* These names are deprecated and will be removed. */
  APS_SKR_SUCCESS_STATUS          = APS_SWITCH_SUCCESS_STATUS,
  APS_SKR_NO_SHORT_ADDRESS_STATUS = APS_SWITCH_NO_SHORT_ADDRESS_STATUS,
  APS_SKR_SECURITY_FAIL_STATUS    = APS_SWITCH_SECURITY_FAIL_STATUS,
  APS_SKR_NOT_SENT_STATUS         = APS_SWITCH_NOT_SENT_STATUS
} APS_SwitchStatus_t;

/** This type is deprecated and will be removed. */
typedef APS_SwitchStatus_t APS_SkrStatus_t;

/**//**
 * \struct APS_SwitchKeyConf_t apsmeSwitchKey.h "aps.h"
 *
 * \brief Confirmation parameters of APSME-SWITCH-KEY.request primitive.
 **/
typedef struct
{
  /*! The status of switching the key */
  APS_SwitchStatus_t status;
} APS_SwitchKeyConf_t;

/**//**
 * \struct APS_SwitchKeyReq_t apsmeSwitchKey.h "aps.h"
 *
 * \brief Parameters of APSME-SWITCH-KEY.request primitive.
 *
 *  See ZigBee Specification r19, Table 4.26, page 468.
 **/
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    /** Request to send APS Remove Device command. */
    ApsCommandReq_t commandReq;
  } service;
  /** \endcond **/

  /** \ref Endian "[LE]" The extended 64-bit address of the device to
   * which the switch-key command is sent. */
  ExtAddr_t destAddress;
  /** A sequence number assigned to a network key by the Trust Center
   * and used to distinguish network keys. */
  uint8_t keySeqNumber;
  /** Confirm primitive as a parameter of the callback function */
  APS_SwitchKeyConf_t confirm;
  /** Callback function pointer as a handler of corresponding
   * confirm primitive. Must not be set to NULL. */
  void (*APS_SwitchKeyConf)(APS_SwitchKeyConf_t *conf);
} APS_SwitchKeyReq_t;

/**//**
 * \struct APS_SwitchKeyInd_t apsmeSwitchKey.h "aps.h"
 *
 * \brief Parameters of APSME-SWITCH-KEY.indication primitive.
 *
 *  See ZigBee Specification r19, Table 4.27, page 469.
 **/
typedef struct
{
  /** \ref Endian "[LE]" The extended 64-bit address of the device
   * that sent the switch-key command. */
  ExtAddr_t srcAddress;
  /** A sequence number assigned to a network key by the Trust Center
   * and used to distinguish network keys. */
  NWK_KeySeqNum_t keySeqNumber;
} APS_SwitchKeyInd_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Sends a command to a remote device to switch to another network key

  \ingroup aps_security

  The command can be send by the trust center to force a specific device or all
  devices in the network to make another network key active. If a single node is to be
  affected the extended address of the node shall be specified in the \c destAddress field.
  To send a switch key command to all devices in the network assign \c destAddress to
  ::APS_BROADCAST_RX_ON_WHEN_IDLE_EXT_ADDRESS.

  A single network key must be active on all devices in the network. Two network keys
  may be active on different device, while the network is switching to the new network
  key. In high security the command to switch the network key must be sent by the trust
  center separately to each device in the network (unicast). In other security modes the
  trust center should send a broadcast request.

  The function conforms to a primitive defined in ZigBee Specification r19, 4.4.7.1, page 467.

  \param[in] req - pointer to APSME-SWITCH-KEY.request's parameters.
                   \sa APS_SwitchKeyReq_t
  \return None
  
  \par Response statuses:
    Accessed via the APS_SwitchKeyConf_t::status field of the 
    APS_SwitchKeyReq_t::APS_SwitchKeyConf callback's argument.
 ******************************************************************************/
void APS_SwitchKeyReq(APS_SwitchKeyReq_t *req);

/**************************************************************************//**
  \brief Informs the upper layer that it received a switch key command frame.

   The APS layer shall generate this primitive when it receives a switch-key
   command frame that is successfully decrypted and authenticated.
   See ZigBee Specification r19, 4.4.7.2, page 468.

  \param[in] ind - pointer to APSME-SWITCH-KEY.indication's parameters.
                   \sa APS_SwitchKeyInd_t
  \return None.
 ******************************************************************************/
void APS_SwitchKeyInd(APS_SwitchKeyInd_t *ind);

#endif /* _SECURITY_ */
#endif /* _APSME_SWITCH_KEY_H */
/** eof apsmeSwitchKey.h */

