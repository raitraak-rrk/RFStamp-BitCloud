/*********************************************************************************************//**
\file  

\brief D_XNv Initialization Interface

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: D_XNv_Init.h 24615 2013-02-27 05:14:24Z mahendranp $

***************************************************************************************************/

#ifndef D_XNV_INIT_H
#define D_XNV_INIT_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

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
/** D_XNv system integrity check callback. See D_XNv_Init parameter pfCallback.
*/
typedef void (*D_XNv_SystemCheckCallback_t)(void);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/


/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initializes the component.
    \param pfCallback function that is called before any write access to the NV chip. This function
                      can be used to trigger an assert when the system conditions could cause
                      data corruption when the NV chip is accessed.
*/
void D_XNv_Init(D_XNv_SystemCheckCallback_t pfCallback);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // D_XNV_INIT_H
