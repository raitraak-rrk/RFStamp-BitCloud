/**************************************************************************//**
  \file apsmeRequestKey.h

  \brief Interface of APS Request Key Service.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-11-24 Max Gekk - Created.
   Last change:
    $Id: apsmeRequestKey.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APSME_REQUEST_KEY_H
#define _APSME_REQUEST_KEY_H
/**//**
 *
 *  The APSME provides services that allow a device to request the active
 * network key or a master key from another device (for example, its Trust
 * Center). See ZigBee Specification r19, 4.4.6, page 465.
 *
 **/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsCommon.h>

#if defined _SECURITY_ && defined _LINK_SECURITY_
/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * The type of key being requested:
 *  \li Network (0x01) and Application (0x02) key,
 *  \li Trust Center Link Key (0x04 - it's not supported),
 *  \li 0x00 and 0x03-0xFF = Reserved.
 **/
typedef enum
{
  APS_NETWORK_KEY_TYPE = 0x01,
  APS_APP_KEY_TYPE     = 0x02,
  APS_TC_LINK_KEY_TYPE = 0x04
} APS_RequestKeyType_t;

/** Status of creating and sending a request key command frame. */
typedef enum
{
  APS_REQUEST_KEY_SUCCESS_STATUS  = 0x00,
  APS_REQUEST_KEY_NO_SHORT_ADDRESS_STATUS = 0x01,
  APS_REQUEST_KEY_SECURITY_FAIL_STATUS = 0x02,
  APS_REQUEST_KEY_NOT_SENT_STATUS = 0x03,
  APS_REQUEST_KEY_TIMEOUT_STATUS  = 0x04,
  APS_REQUEST_KEY_TX_FAIL_STATUS  = 0x05,
  /* These names are deprecated and will be removed. */
  APS_RKR_SUCCESS_STATUS          = APS_REQUEST_KEY_SUCCESS_STATUS,
  APS_RKR_NO_SHORT_ADDRESS_STATUS = APS_REQUEST_KEY_NO_SHORT_ADDRESS_STATUS,
  APS_RKR_SECURITY_FAIL_STATUS    = APS_REQUEST_KEY_SECURITY_FAIL_STATUS,
  APS_RKR_NOT_SENT_STATUS         = APS_REQUEST_KEY_NOT_SENT_STATUS,
  APS_RKR_TIMEOUT                 = APS_REQUEST_KEY_TIMEOUT_STATUS
} APS_RequestKeyStatus_t;

/** This type is deprecated and will be removed. */
typedef APS_RequestKeyStatus_t APS_RkrStatus_t;

/**//**
 * \struct APS_RequestKeyConf_t apsmeRequestKey.h "aps.h"
 *
 * \brief Parameters of APSME-REQUEST-KEY.confirm primitive.
 **/
typedef struct
{
  APS_RequestKeyStatus_t status; //!< The status of requesting the key
} APS_RequestKeyConf_t;

/**//**
 * \struct APS_RequestKeyReq_t apsmeRequestKey.h "aps.h"
 *
 * \brief Parameters of APSME-REQUEST-KEY.request primitive.
 *
 *  See ZigBee Specification r19, Table 4.24, page 465.
 **/
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    /** Service field, used for queue support */
    QueueElement_t qelem;
    /** Request to send APS Update Device command. */
    ApsCommandReq_t commandReq;
    uint32_t ttl;
    bool transportKeyReceived;
    bool requestKeySent;
  } service;
  /** \endcond **/

  /** \ref Endian "[LE]" The extended 64-bit address of the device to which
   * the request-key command should be sent. */
  ExtAddr_t destAddress;
  /** The type of key being requested:
   * \li 0x01 = Network key,
   * \li 0x02 = Application key,
   * \li 0x04 = Trust Center Link Key. */
  APS_RequestKeyType_t keyType;
  /** \ref Endian "[LE]" If the KeyType parameter indicates an application key,
   * this parameter shall indicate an extended 64-bit address of a device that
   * shall receive the same key as the device requesting the key. */
  ExtAddr_t partnerAddress;
  /** Canfirmation parameters. \sa APS_RequestKeyConf_t */
  APS_RequestKeyConf_t confirm;
  /** Callback function pointer as a handler of corresponding
   * confirm primitive. Must not be set to NULL. */
  void (*APS_RequestKeyConf)(APS_RequestKeyConf_t *conf);
} APS_RequestKeyReq_t;

/**//**
 * \struct APS_RequestKeyInd_t apsmeRequestKey.h "aps.h"
 *
 * \brief Parameters of APSME-REQUEST-KEY.indication primitive.
 *
 *  See ZigBee Specification r19, Table 4.25, page 466.
 **/
typedef struct
{
  /** \ref Endian "[LE]" The extended 64-bit address of the device that sent
   * the request-key command. */
  ExtAddr_t srcAddress;
  /** The type of key being requested. Valid values:
   * \li APS_NETWORK_KEY_TYPE
   * \li APS_APP_KEY_TYPE
   **/
  APS_RequestKeyType_t keyType;
  /** \ref Endian "[LE]" If the KeyType parameter indicates an application key,
   * this parameter shall indicate an extended 64-bit address of a device that
   * shall receive the same key as the device requesting the key. */
  ExtAddr_t partnerAddress;
} APS_RequestKeyInd_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Request either the active network key or a new application link key

  \ingroup aps_security

     See ZigBee Specification r19, 4.4.6.1, page 465.

  \param[in] req - pointer to APSME-REQUEST-KEY.request's parameters.
                   \sa APS_RequestKeyReq_t
  \return None.

  \par Response statuses:
    Accessed via the APS_RequestKeyConf_t::status field of the
    APS_RequestKeyReq_t::APS_RequestKeyConf callback's argument.
 ******************************************************************************/
void APS_RequestKeyReq(APS_RequestKeyReq_t *req);

/**************************************************************************//**
  \brief Generate this primitive when APS receives a request-key command frame

  Upon receipt of the APSME-REQUEST-KEY.indication primitive, the ZDO shall
  be informed that the device referenced by the SrcAddress parameter is
  requesting a key. See ZigBee Specification r19, 4.4.6.2, page 466.

  \param[in] indParams - pointer to APSME-REQUEST-KEY.indication's parameters.
                   \sa APS_RequestKeyInd_t
  \return None.
 ******************************************************************************/
void APS_RequestKeyInd(APS_RequestKeyInd_t *indParams);

#endif /* _SECURITY_ and _LINK_SECURITY_ */

#endif /* _APSME_REQUEST_KEY_H */
/** eof apsmeRequestKey.h */

