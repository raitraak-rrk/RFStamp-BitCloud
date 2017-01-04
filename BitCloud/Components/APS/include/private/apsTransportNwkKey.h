/**************************************************************************//**
  \file apsTransportNwkKey.h

  \brief Interface of NWK key transport.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-05-15 Max Gekk - Created.
   Last change:
    $Id: apsTransportNwkKey.h 17549 2011-06-15 09:36:29Z mgekk $
 ******************************************************************************/
#if !defined _APS_TRANSPORT_NWK_KEY_H
#define _APS_TRANSPORT_NWK_KEY_H

#if defined _SECURITY_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsTransportKey.h>

/******************************************************************************
                               Define(s) section
 ******************************************************************************/
/* Tunneled auxiliary header consists of:
 * - security control field (1 octet),
 * - frame counter field (4 octets),
 * - source address (8 octets) */
#define APS_TUNNELED_AUXILIARY_HEADER_LEN  13
/* See ZigBee spec. r18, page 493, Table 4.38. For security level = 0x05. */
#define APS_TUNNELED_MIC_LEN               4

#define APS_TRANSPORT_NWK_KEY_PAYLOAD_LEN \
  (sizeof(ApsTransportNwkKeyFrame_t) - sizeof(ApduCommandHeader_t))

/******************************************************************************
                                 Types section
 ******************************************************************************/
BEGIN_PACK
/** Type of Transport-Key Command Frame (see ZigBee spec r19, Figure 4.8
 * with network key descriptor field (see Figure 4.10, page 480). */
typedef struct PACK _ApsTransportNwkKeyFrame_t
{
  /** Common part of APS transport key command frame. */
  ApsTransportKeyCommonFrame_t common;
  /** The key sub-field shall contain a network key. */
  uint8_t key[SECURITY_KEY_SIZE];
  /** The sequence number sub-field shall contain the sequence number associated
   * with this network key. See ZigBee spec r19, 4.4.9.2.3.2 */
  NWK_KeySeqNum_t keySeqNumber;
  /** The destination address sub-field shall contain the address of the device
   * which should use this network key. See ZigBee spec r19, page 480. */
  uint8_t destinationAddress[sizeof(ExtAddr_t)];
  /** The source address field sub-shall contain the address of the device
   * (for example, the Trust Center) which originally sent this network key.
   * See ZigBee Specification r19, 4.4.9.2.3.2, page 480. */
  uint8_t sourceAddress[sizeof(ExtAddr_t)];
} ApsTransportNwkKeyFrame_t;

/**
 * \brief Tunnel command frame payload.
 *
 * See ZigBee spec. r18, 4.4.9.8, page 490, Figure 4.22.
 **/
typedef struct PACK _ApsTunnelCommandFrame_t
{
  /** An APS header, which comprises frame control and addressing information. */
  ApduCommandHeader_t header;
  /** The 64-bit extended address of the device that is to receive the tunnelled command. */
  uint8_t dstAddr[sizeof(ExtAddr_t)];
  /** APS base header of the tunneled command: frame control and APS counter. */
  ApduCommandBaseHeader_t  tunneledHeader;
  /** The tunneled auxiliary frame indicate that a link key was used
   * and shall included the extended nonce field. ZigBee spec. r18, 4.4.9.8.3, page 491.*/
  uint8_t tunneledAuxHeader[APS_TUNNELED_AUXILIARY_HEADER_LEN];
  /** Identifier of the tunneled APS command. */
  uint8_t commandId;
  /** The tunnelled command field shall be the APS command frame to be sent to the
   * destination. See ZigBee spec. r18, 4.4.9.8.4 and Figure 4.22. */
  uint8_t tunneledPayload[APS_TRANSPORT_NWK_KEY_PAYLOAD_LEN];
  /** Tunneled APS MIC. See ZigBee spec. r18, page 490, Figure 4.22. */
  uint8_t mic[APS_TUNNELED_MIC_LEN];
} ApsTunnelCommandFrame_t;
END_PACK

/******************************************************************************
                                Prototypes section
 ******************************************************************************/
#if defined _TRUST_CENTRE_
/**************************************************************************//**
  \brief Checks specific parameters of Transport Nwk Key request.

  \param[in] req - pointer to parameters of APSME-TRANSPORT-KEY.request.
  \return Pointer to function for preparing and sending APS Transport Key command.
 ******************************************************************************/
APS_PRIVATE ApsTransportKeyCommand_t
apsCheckTransportNwkKeyReq(APS_TransportKeyReq_t *const req);
#endif /* _TRUST_CENTRE_ */

/**************************************************************************//**
  \brief Processing of a tunneling APS command.

    Extracting the embedded tunnel command and send it to destination.

  \param[in] commandInd - pointer to parameter's structure of received frame.
  \return 'true' if NWK buffer should be released otherwise 'false'.
 ******************************************************************************/
APS_PRIVATE bool apsTunnelInd(const ApsCommandInd_t *const commandInd);

/**************************************************************************//**
  \brief Process an incoming transport key command with NWK key.

  \param[in] commandInd - pointer to received command descriptor.

  \return 'false' if structure commandInd->nwkDataInd is used otherwise 'true'.
 ******************************************************************************/
APS_PRIVATE bool apsTransportNwkKeyInd(const ApsCommandInd_t *const commandInd);

#if !defined _TRUST_CENTRE_
#define apsCheckTransportNwkKeyReq(req) NULL
#endif
#endif /* _SECURITY_ */
#endif /* _APS_TRANSPORT_NWK_KEY_H */
/** eof apsTransportNwkKey.h */
