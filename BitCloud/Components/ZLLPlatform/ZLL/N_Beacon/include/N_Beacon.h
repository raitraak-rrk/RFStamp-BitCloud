/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Beacon.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_BEACON_H
#define N_BEACON_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_Types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

typedef struct N_Beacon_t
{
    uint16_t sourceAddress;
    uint16_t panId;
    uint8_t logicalChannel;
    bool permitJoining;
    bool hasRouterCapacity;
    bool hasDeviceCapacity;
    uint8_t protocolVersion;
    uint8_t stackProfile;
    uint8_t lqi;
    uint8_t depth;
    uint8_t updateId;
    N_Address_ExtendedPanId_t extendedPanId;
} N_Beacon_t;

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

typedef enum N_Beacon_Order_t
{
    N_Beacon_Order_60ms = 2,
    N_Beacon_Order_240ms = 4,
} N_Beacon_Order_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/


/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_BEACON_H
