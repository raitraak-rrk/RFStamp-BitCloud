/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_DeviceInfo_Init.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_DEVICEINFO_INIT_H
#define N_DEVICEINFO_INIT_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Task.h"
#include "N_Types.h"
#include "N_DeviceInfo.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initialises the component
    \param profile The profile for this device
    \param touchlinkRssiCorrection Correction on touchlink received signal strength (e.g. by housing or antenna type)
    \param touchlinkRssiThreshold Threshold for a touchlink scan response to become valid
    \param addressAssignmentCapable If this device can give other devices network addresses
    \param canBeLinkInitiator If this device can initiate link actions
    \param touchlinkZerodBmTransmitPower The transmit power (register) value for 0 dBm
*/
void N_DeviceInfo_Init( uint8_t touchlinkRssiCorrection, int8_t touchlinkRssiThreshold, 
                        bool addressAssignmentCapable, bool canBeLinkInitiator, uint8_t touchlinkZerodBmTransmitPower );

/** The event handler for the component's task
*/
bool N_DeviceInfo_EventHandler(N_Task_Event_t evt);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_DEVICEINFO_INIT_H
