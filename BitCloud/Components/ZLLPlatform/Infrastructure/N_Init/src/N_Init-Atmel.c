/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Init-Atmel.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Init-Atmel_Bindings.h"
#include "N_Init.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

#include "N_Cmi_Init.h"
#include "N_Cmi.h"

#include <string.h>
#include <sysTaskManager.h>

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Init"

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static bool s_initDone = FALSE;
static const N_Init_InitializationCallback_t* s_initializationCallbacks = NULL;

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

// Callback function for N_Cmi_InitMacLayer to initialize the remaining parts of the platform
static void InitMacLayerDone(void)
{
    if (s_initializationCallbacks->ApplicationInitializer != NULL)
    {
        s_initializationCallbacks->ApplicationInitializer();
    }

    s_initDone = TRUE;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Init_Cpu(void)
{
    // Nothing to do
}

void N_Init_HalAndMemory(void)
{
    SYS_SysInit();
}

void N_Init_Stack(const N_Init_InitializationCallback_t* pCb)
{
    s_initializationCallbacks = pCb;

    if (s_initializationCallbacks->TaskListInitializer != NULL)
    {
        s_initializationCallbacks->TaskListInitializer();
    }

    // The initialization of the components is done after BitCloud is initialized/started.
    // BC will call the APL_TaskHandler as soon as it's ready.
    for (;;)
    {
        SYS_RunTask();
    }
}

// The N_Init_Platform will be called from the APL_TaskHandler, which can be called more than once.
// We need this function to be called only 1 time. We use a local (static) variable to keep track of this.
void N_Init_Platform(void)
{
    static bool s_initStarted = FALSE;

    if (!s_initStarted)
    {
        s_initStarted = TRUE;

        if (s_initializationCallbacks->EarlyPlatformInitializer != NULL)
        {
            s_initializationCallbacks->EarlyPlatformInitializer();
        }

        N_Cmi_Init();
        N_Cmi_UseNwkMulticast(FALSE);
        N_Cmi_InitMacLayer(InitMacLayerDone);

        // The initialization of N_Cmi for BitCloud is asynchronous. Wait for the
        // InitMacLayerDone callback before calling the application initializer.
    }
}

bool N_Init_IsInitDone(void)
{
    return s_initDone;
}
