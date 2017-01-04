/**************************************************************************//**
  \file apsCommandReq.h

  \brief Definitions of APS command structure.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-06-19 Dmitry Kolmakov - Moved from apsCommon.c.
   Last change:
    $Id: apsCommandReq.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_COMMAND_REQ_H
#define _APS_COMMAND_REQ_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <apsCommon.h>
#include <appFramework.h>
#include <nwk.h>
#include <macAddr.h>
#include <apsCryptoKeys.h>
#include <zdoAddrResolving.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/

#if defined _SECURITY_
/**//**
 * \enum APS_SPID_t apsAIB.h "aps.h"
 *
 * \brief Type of a security policy identifier of APS command.
 * It is used to get access to the array with encryption/decryption policy of
 * incoming and outgoing APS commands.
 **/
typedef enum _APS_SecurityPolicyId_t
{
  APS_DEFAULT_SPID = 0,
  /** The first 14 security policy identifiers match exactly to APS
   * command identifiers from APS packets. See apsCommand.h */
  APS_SKKE_1_SPID = 0x01,           /* APS_CMD_SKKE_1_ID */
  APS_SKKE_2_SPID = 0x02,           /* APS_CMD_SKKE_2_ID */
  APS_SKKE_3_SPID = 0x03,           /* APS_CMD_SKKE_3_ID */
  APS_SKKE_4_SPID = 0x04,           /* APS_CMD_SKKE_4_ID */
  APS_TRANSPORT_KEY_SPID = 0x05,    /* APS_CMD_TRANSPORT_KEY_ID */
  APS_UPDATE_DEVICE_SPID = 0x06,    /* APS_CMD_UPDATE_DEVICE_ID */
  APS_REMOVE_DEVICE_SPID = 0x07,    /* APS_CMD_REMOVE_DEVICE_ID */
  APS_REQUEST_KEY_SPID = 0x08,      /* APS_CMD_REQUEST_KEY_ID */
  APS_SWITCH_KEY_SPID  = 0x09,      /* APS_CMD_SWITCH_KEY_ID */
  APS_EA_INIT_CHLNG_SPID = 0x0A,    /* APS_CMD_EA_INIT_CHLNG_ID */
  APS_EA_RSP_CHLNG_SPID  = 0x0B,    /* APS_CMD_EA_RSP_CHLNG_ID */
  APS_EA_INIT_MAC_DATA_SPID = 0x0C, /* APS_CMD_EA_INIT_MAC_DATA_ID */
  APS_EA_RSP_MAC_DATA_SPID  = 0x0D, /* APS_CMD_EA_RSP_MAC_DATA_ID */
  APS_TUNNEL_SPID = 0x0E,           /* APS_CMD_TUNNEL_ID */
  /** Security policy that depends on APS command payload. */
  APS_TRANSPORT_APP_KEY_SPID,
  APS_TRANSPORT_NWK_KEY_SPID,
  APS_TRANSPORT_TRUST_KEY_SPID,
  APS_MAX_SPID
} APS_SecurityPolicyId_t;

/** Type of request to transmit APS command. */
typedef struct _ApsCommandReq_t
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    /** Service field, used for queue support */
    QueueElement_t next;
    /** Partner device short address */
    ShortAddr_t partnerShortAddr;
    /** Service structure for memory allocation on the NWK layer */
    NWK_AllocDataReq_t allocateReq;
    /** Callback function for preparing an NWK-DATA.request */
    void (*prepareNwkDataReq)(struct _ApsCommandReq_t *const commandReq);
#if defined _LINK_SECURITY_
    /** Is it an unecrypted duplicated of the original command which requires
     * sending the duplicate according to security policy or not.*/
    bool isDuplicateCommand;
    /** Service structure for payload encryption */
    SSP_EncryptFrameReq_t encryptReq;
#endif /* _LINK_SECURITY_ */
    /** Is used for address resolving */
    ZDO_ResolveAddrReq_t resolveAddrReq;
  } service;
  /** Indicates the command frame is or should be encrypted on the NWK layer */
  bool nwkSecurity;
  /** Security policy identifier used to decide how to encrypt the command */
  APS_SecurityPolicyId_t apsSecurityPolicyId;
  /** Identifies the type of the key which is used to protect the frame */
  uint8_t keyIdentifier;
  /** If the extendedNonce field is equal to 1 then the source address shall
   * be present in the auxilary header */
  bool extendedNonce;
  /** Actual command frame lenght, APS header plus payload */
  uint8_t frameLength;
  /** Pointer to the extended address of a partner device */
  ExtAddr_t *partnerExtAddr;
  /** Callback function for preparing an APS command. Must not be set to NULL. */
  void (*prepareCommand)(struct _ApsCommandReq_t *const commandReq);
  /** Callback function for confirming of APS command transmission. Must not be set to NULL. */
  void (*confirmCommand)(struct _ApsCommandReq_t *const commandReq,
    const APS_Status_t status);
} ApsCommandReq_t;

typedef struct _ApsSecurityStatusDescriptor_t
{
  /** Type of key used to secure a transport key command */
  APS_Status_t status;
  /** Handle of the link key with wich a transport key command has been secured */
#ifdef _LINK_SECURITY_
  APS_KeyHandle_t linkKeyHandle;
#endif /* _LINK_SECURITY_ */
} ApsSecurityStatusDescriptor_t;
#endif /* _SECURITY_ */

#endif /* _APS_COMMAND_REQ_H */
/** eof apsCommandReq.h */

