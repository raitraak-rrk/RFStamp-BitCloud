/**************************************************************************//**
  \file nwkTxSecurity.h

  \brief Interface of transmission security.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2009-08-31 M. Gekk - Created.
   Last change:
    $Id: nwkTxSecurity.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWK_TX_SECURITY_H
#define _NWK_TX_SECURITY_H

#if defined _SECURITY_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <nwkConfig.h>
#include <mac.h>
#include <sspSfp.h>
#include <nwkTx.h>

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Check of that enciphering for a proceeding network packet is required.

  \param[in] outPkt - pointer to a network output packet.
  \return 'true' if encrypting is required otherwise 'false'
 ******************************************************************************/
NWK_PRIVATE bool nwkIsEncryptionRequired(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Request to encrypt an output network packet.

  \param[in] encryptReq - encrypt frame primitive's parameters structure.
  \param[in] macDataReq - MCPS-DATA request primitive's parameters structure.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkEncryptOutputPacket(SSP_EncryptFrameReq_t *const encryptReq,
  const MAC_DataReq_t *const macDataReq);

/**************************************************************************//**
  \brief Is decrypting of output packet required?

  \param[in] outPkt - pointer to an output packet.
  \return 'true' if decrypting is required otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkIsDecryptionRequired(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Request to decrypt an output network packet.

  \param[in] decryptReq - decrypt frame primitive's parameters structure.
  \param[in] macDataReq - MCPS-DATA request primitive's parameters structure.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkDecryptOutputPacket(SSP_DecryptFrameReq_t *const decryptReq,
  const MAC_DataReq_t *const macDataReq);

#else /* _SECURITY_ */

#define nwkIsEncryptionRequired(outPkt) false
#define nwkIsDecryptionRequired(outPkt) false

#endif /* _SECURITY_ */
#endif /* _NWK_TX_SECURITY_H */
/** eof nwkTxSecurity.h */

