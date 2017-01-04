/**************************************************************************//**
  \file apsAIB.h

  \brief Interface of APS information base.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-19 Max Gekk - Created.
   Last change:
    $Id: apsAIB.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/**//**
 *
 *  The APS information base comprises the attributes required to manage the APS
 * layer of a device. The attributes of the AIB are listed in ZigBee spec r19,
 * Table 2.24, page 61. The securityrelated AIB attributes are described in
 * sub-clause 4.4.10, page 489.
 **/
#if !defined _APS_AIB_H
#define _APS_AIB_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwk.h>
#include <apsCommon.h>
#include <apsCommandReq.h>
#include <configServer.h>

/******************************************************************************
                             Definitions section
 ******************************************************************************/
#define APS_AIB_NONMEMBER_RADIUS_DEFAULT_VALUE 2
/** Bit masks for APS_SecurityPolicy_t type. */
/** The bit permits (1) (or prohibits(0)) processing of an unsecured APS command
 * for which there is no link key associated with the device that sent the command. */
#define APS_NO_SEC_CMD_NO_KEY      (1U << 0)
/** The bit permits (1) (or prohibits(0)) processing of an unsecured command when
 * the GLOBAL link key exists. */
#define APS_NO_SEC_CMD_GLOBAL_KEY  (1U << 1)
/** The bit permits (1) (or prohibits(0)) processing of an unsecured command when
 * there is a UNIQUE link key for the device that sent the command. */
#define APS_NO_SEC_CMD_UNIQUE_KEY  (1U << 2)
/** The bit permits (1) (or prohibits(0)) processing of a secured command that
 * was encrypted with the GLOBAL link key. */
#define APS_SEC_CMD_GLOBAL_KEY     (1U << 3)
/** The bit permits (1) (or prohibits(0)) processing of a secured command that
 * was encrypted with a UNIQUE link key. */
#define APS_SEC_CMD_UNIQUE_KEY     (1U << 4)
/** The bit indicates that the APS layer should encrypt an APS command with the GLOBAL link key. */
#define APS_ENCRYPT_OUT_CMD_BY_GLOBAL_KEY  (1U << 5)
/** The bit indicates that the APS layer should encrypt an APS command with a UNIQUE link key. */
#define APS_ENCRYPT_OUT_CMD_BY_UNIQUE_KEY  (1U << 6)
/** The bit controls transmission of unsecured duplicate command. */
#define APS_IS_DUP_CMD_REQUIRED  (1U << 7)

#define APS_ENCRYPT_OUT_CMD \
  (APS_ENCRYPT_OUT_CMD_BY_GLOBAL_KEY | APS_ENCRYPT_OUT_CMD_BY_UNIQUE_KEY)

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \typedef APS_SecurityPolicy_t apsAIB.h "aps.h"
 *
 * \brief Type defining security policy for an APS command.
 **/
typedef uint8_t APS_SecurityPolicy_t;

/**//**
 * \struct AIB_t apsAIB.h "aps.h"
 *
 * \brief Attributes in APS Information Base.
 **/
typedef struct
{
  /** Indicates how many hops of non-member nodes a multicast APS command
   * may go through after leaving the group (used in NWK-layer multicasting).
   * Valid range: 0 - 7, default value - 2. */
  NwkRadius_t nonMemberRadius;
  uint8_t maxFrameRetries;
#if defined _SECURITY_
  /** \ref Endian "[LE]" Identifies the Trust Center address. */
  ExtAddr_t  trustCenterAddress;
  /** \ref Endian "[LE]" Network address of the Trust Center. */
  ShortAddr_t tcNwkAddr;
#if defined _LINK_SECURITY_
  /** Security policy for incoming and outgoing APS commands. */
  APS_SecurityPolicy_t securityPolicy[APS_MAX_SPID];
#endif /* _LINK_SECURITY_ */
#endif /* _SECURITY_ */
#ifdef _APS_FRAGMENTATION_
  uint8_t fragWindowSize;
#ifdef _CERTIFICATION_
  uint8_t txFragDropMask;
  uint8_t rxFragDropMask;
#endif /* _CERTIFICATION_  */
#endif /* _APS_FRAGMENTATION_ */
} AIB_t;

/**//**
 * \struct APS_TcMode_t apsAIB.h "aps.h"
 *
 * \brief The type used to specify if the device is a trust center and what
 * type of the trust center, if it is indeed a trust center.
 **/
typedef enum _APS_TcMode_t
{
  /** The device is not a trust center.*/
  APS_NOT_TRUST_CENTER,
  /** The device is the common trust center for the whole network.*/
  APS_CENTRALIZED_TRUST_CENTER,
  /** The device is one of multiple trust centers (this may happen when the network
   * configuration allows any router to serve as a trust center). */
  APS_DISTRIBUTED_TRUST_CENTER
} APS_TcMode_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
#if defined _SECURITY_
/**************************************************************************//**
  \brief Set the Trust Center address

  \param[in] addr - pointer to the extended address to set

  \return none
 ******************************************************************************/
void APS_SetTrustCenterAddress(const ExtAddr_t *addr);

/**************************************************************************//**
  \brief Check whether supplied extended address is the address of Trust Center

  \param[in] addr - pointer to an extended address

  \return \c true, if the provided address is the TC's address, \c false - otherwise
 ******************************************************************************/
bool APS_IsTcAddress(const ExtAddr_t *addr);

/**************************************************************************//**
  \brief Get a pointer to the Trust Center address

  \return pointer to the Trust Center extended address
 ******************************************************************************/
ExtAddr_t* APS_GetTrustCenterAddress(void);

/**************************************************************************//**
  \brief Get a pointer to the Trust Center Network address

  \return pointer to the Trust Center network address
 ******************************************************************************/
ShortAddr_t* APS_GetTrustCenterNwkAddress(void);

/**************************************************************************//**
  \brief Gets the trust center role of the current device.

  \return The device's role as a trust Center:
    \li APS_NOT_TRUST_CENTER - not a trust center,
    \li APS_CENTRALIZED_TRUST_CENTER - a single trust center in the network,
    \li APS_DISTRIBUTED_TRUST_CENTER  - one of multiple trust centers (each router can
        be a trust center).
 ******************************************************************************/
APS_TcMode_t APS_GetOwnTcMode(void);

/**************************************************************************//**
  \brief Check whether the node is the Trust Center or not.

  \return true, if the node is one of distributed trust centers or the centralized
    trust center; otherwise return false.
 ******************************************************************************/
bool APS_IsTrustCenter(void);

/**************************************************************************//**
  \brief Check whether the provided extended address is a trusted address

  \param[in] addr - a pointer to an extended address to check

  \return \c true, if the argument is the address of the centralized Trust Center
        or there are multiple trust centers in the network (distributed TC mode enabled),
        otherwise return \c false.
 ******************************************************************************/
bool APS_IsTrustedAddress(const ExtAddr_t *addr);

#if defined _LINK_SECURITY_
/**************************************************************************//**
  \brief Updates security policy for the command with a given security policy ID

  \param[in] spid - security policy identifier of the command
  \param[in] policy - bit map where each bit permits or prohibits processing
                      of an incoming command and determines how to encrypt
                      an outgoing command with the given spid

  \return None
 ******************************************************************************/
void APS_UpdateSecurityPolicy(const APS_SecurityPolicyId_t spid,
  const APS_SecurityPolicy_t policy);

#endif /* _LINK_SECURITY_ */
#endif /* _SECURITY_ */

#endif /* _APS_AIB_H */
/** eof apsAIB.h */

