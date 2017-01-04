/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: devconfig.h 24526 2013-02-15 09:55:14Z akhromykh $

***************************************************************************************************/

#ifndef N_DEVCONFIG_H
#define N_DEVCONFIG_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "N_DeviceInfo.h" // for channel defines

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* DEVELOPER DEFINES
***************************************************************************************************/

/** Define to change the channel masks of the device */
//#define DEVCFG_PRIMARY_CHANNELMASK        (ZB_CHANNEL_12)
//#define DEVCFG_SECONDARY_CHANNELMASK      (0u)

/** Define to set a fixed panId */
//#define DEVCFG_FIXED_PANID        (0x1337u)

/** Define to set a fixed extended panId */
//#define DEVCFG_FIXED_EXT_PANID    { 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u }

/** Define to set a fixed network key */
//#define DEVCFG_FIXED_NWK_KEY      { 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u, 
//                                    0x09u, 0x0Au, 0x0Bu, 0x0Cu, 0x0Du, 0x0Eu, 0x0Fu, 0x00u }

/** Define to disable the watchdog */
//#define DEVCFG_DISABLE_WATCHDOG

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_DEVCONFIG_H
