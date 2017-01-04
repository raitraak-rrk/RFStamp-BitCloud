/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Init-Testharness.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Init.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Init"

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Init_Cpu(void)
{
    // Nothing to do
}

void N_Init_HalAndMemory(void)
{
    // Nothing to do
}

void N_Init_Stack(const N_Init_InitializationCallback_t* pCb)
{
    if (pCb->TaskListInitializer != NULL)
    {
        pCb->TaskListInitializer();
    }

    if (pCb->EarlyPlatformInitializer != NULL)
    {
        pCb->EarlyPlatformInitializer();
    }

    if (pCb->ApplicationInitializer != NULL)
    {
        pCb->ApplicationInitializer();
    }
}

void N_Init_Platform(void)
{
    // Nothing to do
}

bool N_Init_IsInitDone(void)
{
    return TRUE;
}
