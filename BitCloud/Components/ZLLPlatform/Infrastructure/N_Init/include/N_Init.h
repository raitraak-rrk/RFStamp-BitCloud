/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Init.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_INIT_H
#define N_INIT_H

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

typedef struct N_Init_InitializationCallback_t
{
    /** Function that is called when the platform task list can be initialized
    */
    void (*TaskListInitializer)(void);

    /** Function that is called just before the stack is started. This callback function
     *  should initialize the components that are required before N_Cmi is initialized.
    */
    void (*EarlyPlatformInitializer)(void);

    /** Function that is called as final initialization routine. Within this function, the
     *  remaining platform components and the application components can be initialized.
    */
    void (*ApplicationInitializer)(void);

    int8_t endOfList;
} N_Init_InitializationCallback_t;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Initialize the CPU.
*/
void N_Init_Cpu(void);

/** Initialize the HAL and Memory allocation.
*/
void N_Init_HalAndMemory(void);

/** Initialize the ZigBee stack and register initialization functions for platform components.
*/
void N_Init_Stack(const N_Init_InitializationCallback_t* pCb);

/** Initialize the ZigBee Platform.
    \note This function will start calling the callback functions passed to N_Init_Stack.
*/
void N_Init_Platform(void);

/** Return TRUE when the platform is initialized
*/
bool N_Init_IsInitDone(void);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_INIT_H
