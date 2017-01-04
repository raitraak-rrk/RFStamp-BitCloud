/***************************************************************************//**
  \file zsiSynchronization.c

  \brief Initial stack parameters synchronization between HOST and NP.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-12  A. Razinkov - Created.
   Last change:
    $Id: zsiSynchronization.c 25428 2013-10-03 14:09:15Z nfomin $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <aps.h>
#include <zsiInit.h>
#include <configServer.h>

/******************************************************************************
                               Implementation section
 ******************************************************************************/
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Initial parameters synchronization between HOST and NP.

  \return None.
 ******************************************************************************/
void zsiSynchronizeStartupParameters(void)
{
  CS_WriteParameter(CS_CHANNEL_MASK_ID, &(uint32_t){CS_CHANNEL_MASK});
  CS_WriteParameter(CS_CHANNEL_PAGE_ID, &(uint8_t){CS_CHANNEL_PAGE});
  CS_WriteParameter(CS_EXT_PANID_ID, &(ExtPanId_t){CS_EXT_PANID});
  CS_WriteParameter(CS_UID_ID, &(ExtAddr_t){CS_UID});
  CS_WriteParameter(CS_END_DEVICE_SLEEP_PERIOD_ID, &(uint32_t){CS_END_DEVICE_SLEEP_PERIOD});
  CS_WriteParameter(CS_RF_TX_POWER_ID, &(int8_t){CS_RF_TX_POWER});
  CS_WriteParameter(CS_NWK_UNIQUE_ADDR_ID, &(bool){CS_NWK_UNIQUE_ADDR});
  CS_WriteParameter(CS_NWK_ADDR_ID, &(ShortAddr_t){CS_NWK_ADDR});
#if defined(_ENABLE_PERSISTENT_SERVER_) && defined(_POWER_FAILURE_)
  CS_WriteParameter(CS_POWER_FAILURE_ID, &(uint8_t){CS_POWER_FAILURE});
#endif /* _ENABLE_PERSISTENT_SERVER_ && _POWER_FAILURE_ */
#ifdef _SECURITY_
  CS_WriteParameter(CS_ZDO_SECURITY_STATUS_ID, &(uint8_t){CS_ZDO_SECURITY_STATUS});
  CS_WriteParameter(CS_APS_TRUST_CENTER_ADDRESS_ID, &(uint64_t){CS_APS_TRUST_CENTER_ADDRESS});
  CS_WriteParameter(CS_NETWORK_KEY_ID, &(uint8_t [SECURITY_KEY_SIZE])CS_NETWORK_KEY);
#endif
}
#endif /* ZAPPSI_HOST */

/* eof zsiSynchronization.c */
