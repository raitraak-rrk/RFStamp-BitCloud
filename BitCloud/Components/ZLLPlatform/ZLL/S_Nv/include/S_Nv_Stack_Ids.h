/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2013-2014, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal


$Id: S_Nv_Stack_Ids.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef S_NV_STACK_IDS_H
#define S_NV_STACK_IDS_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "S_Nv.h"

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/**
    This file defines the flash storage ids used by the ZLLPlatform.
    NOTE: Future additions to the ZLLPlatform should use IDs from the range
    S_NV_STACK_RANGE_MIN..S_NV_STACK_RANGE_MAX, defined in \ref wlPdsMemIds.h.
    The two items defined here are legacy and should not be changed.
*/
#define FREE_RANGES_STORAGE_ID                      (S_NV_PLATFORM_RANGE1_MIN + 1u) // addressManager
#define FACTORY_NEW_STORAGE_ID                      (S_NV_PLATFORM_RANGE1_MIN + 2u) // device info

#endif //S_NV_STACK_IDS_H
