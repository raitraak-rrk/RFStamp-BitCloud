/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: S_Nv_Init.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef S_NV_INIT_H
#define S_NV_INIT_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Task.h"
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


/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initializes the internal flash service for early access.
    \note It is not required to call this function if the application does not require early access
    to internal Nv. Note that the S_Nv component is fully functional after calling S_Nv_EarlyInit(),
    but until S_Nv_Init() is called, chances are much higher that the component blocks when a sector
    becomes full. Only after S_Nv_Init() is called, compacting the sector is decoupled as much as
    possible.
*/
void S_Nv_EarlyInit(void);

/** Initializes the component and tells which items are persistent against S_Nv_EraseAll(FALSE).
*/
void S_Nv_Init(void);

/** The event handler for the component's task.
*/
bool S_Nv_EventHandler(N_Task_Event_t evt);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // S_NV_INIT_H
