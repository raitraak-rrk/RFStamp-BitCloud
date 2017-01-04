/**************************************************************************//**
  \file zsiCommandId.h

  \brief ZAppSI command identifiers

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-29  A. Razinkov - Created.
   Last change:
    $Id: zsiCommandId.h 28222 2015-07-24 11:38:43Z unithra.c $
 ******************************************************************************/

#ifndef _ZSICOMMANDID_H
#define _ZSICOMMANDID_H

/******************************************************************************
                              Defines section
******************************************************************************/

/* SYS domain */
#define ZSI_SYS_READ_PARAMETER_REQUEST  0x00U
#define ZSI_SYS_READ_PARAMETER_CONFIRM  0x01U
#define ZSI_SYS_WRITE_PARAMETER_REQUEST 0x02U
#define ZSI_SYS_WRITE_PARAMETER_CONFIRM 0x03U

/* APS domain */
#define ZSI_APS_REGISTER_ENDPOINT_REQUEST              0x00U
#define ZSI_APS_REGISTER_ENDPOINT_CONFIRM              0x01U
#define ZSI_APS_DATA_INDICATION                        0x02U
#define ZSI_APS_DATA_REQUEST                           0x03U
#define ZSI_APS_DATA_CONFIRM                           0x04U
#define ZSI_APS_SET_LINK_KEY_REQUEST                   0x05U
#define ZSI_APS_SET_LINK_KEY_CONFIRM                   0x06U
#define ZSI_APS_GET_LINK_KEY_REQUEST                   0x07U
#define ZSI_APS_GET_LINK_KEY_CONFIRM                   0x08U
#define ZSI_APS_FIND_KEYS_REQUEST                      0x09U
#define ZSI_APS_FIND_KEYS_CONFIRM                      0x0AU
#define ZSI_APS_BIND_REQUEST                           0x0BU
#define ZSI_APS_BIND_CONFIRM                           0x0CU
#define ZSI_APS_GET_KEY_PAIR_DEV_ADDR_REQUEST          0x0DU
#define ZSI_APS_GET_KEY_PAIR_DEV_ADDR_CONFIRM          0x0EU
#define ZSI_APS_NEXT_KEYS_REQUEST                      0x0FU
#define ZSI_APS_NEXT_KEYS_CONFIRM                      0x10U
#define ZSI_APS_MARK_TC_KEY_REQUEST                    0x11U
#define ZSI_APS_MARK_TC_KEY_CONFIRM                    0x12U
#define ZSI_APS_SET_AUTHORIZED_STATUS_REQUEST          0x13U
#define ZSI_APS_SET_AUTHORIZED_STATUS_CONFIRM          0x14U
#define ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_REQUEST    0x15U
#define ZSI_APS_SET_DEFAULT_KEY_PAIR_STATUS_CONFIRM    0x16U
#define ZSI_APS_ARE_KEYS_AUTHORIZED_REQUEST            0x17U
#define ZSI_APS_ARE_KEYS_AUTHORIZED_CONFIRM            0x18U
#define ZSI_APS_GET_TRUST_CENTRE_ADDRESS_REQUEST       0x19U
#define ZSI_APS_GET_TRUST_CENTRE_ADDRESS_CONFIRM       0x1AU
#define ZSI_APS_NEXT_BINDING_ENTRY_REQUEST             0x1BU
#define ZSI_APS_NEXT_BINDING_ENTRY_CONFIRM             0x1CU

/* ZDO domain */
#define ZSI_ZDO_START_NETWORK_REQUEST  0x00U
#define ZSI_ZDO_START_NETWORK_CONFIRM  0x01U
#define ZSI_ZDO_RESET_NETWORK_REQUEST  0x02U
#define ZSI_ZDO_RESET_NETWORK_CONFIRM  0x03U
#define ZSI_ZDO_MGMT_NWK_UPDATE_NOTIFY 0x04U
#define ZSI_ZDO_GET_LQI_RSSI_REQUEST   0x05U
#define ZSI_ZDO_GET_LQI_RSSI_CONFIRM   0x06U
#define ZSI_ZDO_SLEEP_REQUEST          0x07U
#define ZSI_ZDO_SLEEP_CONFIRM          0x08U
#define ZSI_ZDO_WAKEUP_INDICATION      0x09U
#define ZSI_ZDO_BIND_INDICATION        0x0AU
#define ZSI_ZDO_UNBIND_INDICATION      0x0BU
#define ZSI_ZDO_START_SYNC_REQUEST     0x0CU
#define ZSI_ZDO_START_SYNC_CONFIRM     0x0DU
#define ZSI_ZDO_STOP_SYNC_REQUEST      0x0EU
#define ZSI_ZDO_STOP_SYNC_CONFIRM      0x0FU
#define ZSI_ZDO_NETWORK_STATUS_REQUEST 0x10U
#define ZSI_ZDO_NETWORK_STATUS_CONFIRM 0x11U

/* ZDP domain */
#define ZSI_ZDP_MATCH_DESCRIPTOR_REQUEST  0x00U
#define ZSI_ZDP_MATCH_DESCRIPTOR_RESPONSE 0x01U
#define ZSI_ZDP_IEEE_ADDRESS_REQUEST      0x02U
#define ZSI_ZDP_IEEE_ADDRESS_RESPONSE     0x03U
#define ZSI_ZDP_BIND_REQUEST              0x04U
#define ZSI_ZDP_BIND_RESPONSE             0x05U
#define ZSI_ZDP_PERMIT_JOINING_REQUEST    0x06U
#define ZSI_ZDP_PERMIT_JOINING_RESPONSE   0x07U
#define ZSI_ZDP_SIMPLE_DESCRIPTOR_REQUEST  0x08U
#define ZSI_ZDP_SIMPLE_DESCRIPTOR_RESPONSE 0x09U

/* NWK domain */
#define ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_REQUEST  0x00U
#define ZSI_NWK_GET_SHORT_BY_EXT_ADDRESS_CONFIRM  0x01U
#define ZSI_NWK_GET_NEXT_HOP_REQUEST              0x02U
#define ZSI_NWK_GET_NEXT_HOP_CONFIRM              0x03U
#define ZSI_NWK_GET_UNICAST_DELIVERY_TIME_REQUEST 0x04U
#define ZSI_NWK_GET_UNICAST_DELIVERY_TIME_CONFIRM 0x05U
#define ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_REQUEST  0x06U
#define ZSI_NWK_GET_EXT_BY_SHORT_ADDRESS_CONFIRM  0x07U
#define ZSI_NWK_GET_SHORT_ADDRESS_REQUEST         0x08U
#define ZSI_NWK_GET_SHORT_ADDRESS_CONFIRM         0x09U
#define ZSI_NWK_SET_KEY_REQUEST                   0x0AU
#define ZSI_NWK_SET_KEY_CONFIRM                   0x0BU
#define ZSI_NWK_ACTIVATE_KEY_REQUEST              0x0CU
#define ZSI_NWK_ACTIVATE_KEY_CONFIRM              0x0DU

/* KE domain */
#define ZSI_KE_ECC_GENERATE_KEY_REQ               0x00U
#define ZSI_KE_ECC_GENERATE_KEY_CONF              0x01U
#define ZSI_KE_ECC_KEY_BIT_GENERATE_REQ           0x02U
#define ZSI_KE_ECC_KEY_BIT_GENERATE_CONF          0x03U
#define ZSI_KE_SSP_BCB_HASH_REQ                   0x04U
#define ZSI_KE_SSP_BCB_HASH_CONF                  0x05U
#define ZSI_KE_SSP_KEYED_HASH_MAC_REQ             0x06U
#define ZSI_KE_SSP_KEYED_HASH_MAC_CONF            0x07U

/* MAC domain */
#define ZSI_MAC_IS_OWN_EXT_ADDR_REQ               0x00U
#define ZSI_MAC_IS_OWN_EXT_ADDR_CONF              0x01U
#define ZSI_MAC_GET_EXT_ADDR_REQ                  0x02U
#define ZSI_MAC_GET_EXT_ADDR_CONF                 0x03U
#define ZSI_RF_RANDOM_REQ                         0x04U
#define ZSI_RF_RANDOM_CONF                        0x05U

/* HAL Domain */
#define ZSI_HAL_RESET_REQUEST                     0x00U
#define ZSI_HAL_RESET_CONFIRM                     0x01U

/* BSP Domain */
#define ZSI_BSP_READ_UID_REQUEST                  0x00U
#define ZSI_BSP_READ_UID_CONFIRM                  0x01U

#endif /* _ZSICOMMANDID_H */
/* eof zsiCommandId.h */
