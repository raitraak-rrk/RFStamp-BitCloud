/**************************************************************************//**
  \file macData.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
    data primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACDATA_H
#define _MACDATA_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <macAddr.h>
#include <macCommon.h>


/******************************************************************************
                        Defines section
******************************************************************************/


/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief MAC data TxOptions.
 * IEEE 802.15.4-2006 Table 41.
*/
typedef enum
{
  MAC_NO_TXOPTION                   = 0x00,
  MAC_ACK_TXOPTION                  = 0x01,
  MAC_GTS_TXOPTION                  = 0x02,
  MAC_INDIRECT_TXOPTION             = 0x04,
  MAC_NO_PANID_COMPRESSION_TXOPTION = 0x08 //It is not from IEEE 802.15.4
} MAC_TxOptions_t;

/**
 * \brief MCPS-DATA confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.1.2 MCPS-DATA.confirm.
*/
typedef struct
{
  //! The handle associated with the MSDU being confirmed.
  uint8_t       msduHandle;
  //! The status of the last MSDU transmission.
  MAC_Status_t  status;
}  MAC_DataConf_t;

/**
 * \brief MCPS-DATA request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.1.1 MCPS-DATA.request.
*/
typedef struct
{
  //! Service field - for internal needs.
  MAC_ServiceTransaction_t service;
  //! The source addressing mode for this primitive and subsequent MPDU.
  MAC_AddrMode_t  srcAddrMode;
  //! The destination addressing mode for this primitive and subsequent MPDU.
  MAC_AddrMode_t  dstAddrMode;
  //! The 16-bit PAN identifier of the entity to which the MSDU is being transferred.
  PanId_t         dstPanId;
  //! The individual device address of the entity to which the MSDU is being transferred.
  MAC_Addr_t      dstAddr;
  //! The number of octets contained in the MSDU to be transmitted by the MAC sublayer entity.
  uint8_t         msduLength;
  //! The set of octets pointer forming the MSDU to be transmitted by the MAC sublayer entity.
  uint8_t         *msdu;
  //! The handle associated with the MSDU to be transmitted by the MAC sublayer entity.
  uint8_t         msduHandle;
  //! The 3 bits indicate the transmission options for this MSDU.
  MAC_TxOptions_t txOptions;
  //! To configure CSMA
#ifdef _ZGPD_SPECIFIC_
  uint8_t performCsma;
#endif /* _ZGPD_SPECIFIC_ */
  //! MCPS-DATA confirm callback function's pointer.
  void (*MAC_DataConf)(MAC_DataConf_t *conf);
  //! MCPS-DATA confirm parameters' structure.
  MAC_DataConf_t confirm;
  
#if defined _SYS_ASSERT_ON_ && defined _IEEE_ZIGBEE_COMPLIANCE_
  //! Service field - for debug needs.
  bool interPan;
#endif // defined _SYS_ASSERT_ON_ && defined _IEEE_ZIGBEE_COMPLIANCE_
}  MAC_DataReq_t;


/**
 * \brief MCPS-DATA indication primitive's parameters.
 * IEEE 802.15.4-2006 7.1.1.3 MCPS-DATA.indication.
*/
typedef struct
{
  //! The source addressing mode for this primitive corresponding to the received.
  MAC_AddrMode_t srcAddrMode;
  //! The 16-bit PAN identifier of the entity from which the MSDU was received.
  PanId_t        srcPANId;
  //! The individual device address of the entity from which the MSDU was received.
  MAC_Addr_t     srcAddr;
  //! The destination addressing mode for this primitive corresponding to the received MPDU.
  MAC_AddrMode_t dstAddrMode;
  //! The 16-bit PAN identifier of the entity to which the MSDU is being transferred.
  PanId_t        dstPANId;
  //! The individual device address of the entity to which the MSDU is being transferred.
  MAC_Addr_t     dstAddr;
  //! The number of octets contained in the MSDU being indicated by the MAC sublayer entity.
  uint8_t        msduLength;
  //! The set of octets pointer forming the MSDU being indicated by the MAC sublayer entity.
  uint8_t        *msdu;
  //! LQI value measured during reception of the MPDU.
  uint8_t        linkQuality;
  //! The DSN of the received data frame.
  uint8_t        dsn;
  //! RSSI value measured during  reception of the MPDU. Non-standard addition.
  int8_t         rssi;
}  MAC_DataInd_t;


/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MCPS-DATA request primitive's prototype.
  \param reqParams -  MCPS-DATA request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_DataReq(MAC_DataReq_t *reqParams);

/**************************************************************************//**
  \brief MCPS-DATA indication primitive's prototype.
  \param indParams -  MCPS-DATA indication parameters' structure pointer.
  \return none.
******************************************************************************/
extern void MAC_DataInd(MAC_DataInd_t *indParams);

#endif /* _MACDATA_H */

// eof macData.h
