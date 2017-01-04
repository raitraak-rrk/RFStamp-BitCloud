/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Timer-Internal-Testharness.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Timer_Bindings.h"
#include "N_Timer_Init.h"
#include "N_Timer.h"
#include "N_Timer-Internal.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Types.h"
#include "N_Util.h"
#include "S_SerialComm.h"

#include "Parser.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Timer_Internal"
#define TXID COMPID

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static uint32_t s_systemTime = 0uL;

static bool s_setTimerLoggingEnabled = FALSE;

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static int8_t N_Timer_Internal_SerialCommCallback(const char* function, char* arguments)
{
    int8_t result = RESULT_FAILURE;

    if (N_UTIL_STR_EQUAL(function, "Update"))
    {
        bool error = FALSE;

        result = RESULT_ILLEGAL_ARGUMENT;
        int32_t elapsedMs = Parser_GetInt32(&arguments, &error);

        if ( !error )
        {
            s_systemTime += (uint32_t) elapsedMs;
            N_Timer_Internal_Update();
            result = RESULT_SUCCESS;
        }
    }
    else if (N_UTIL_STR_EQUAL(function, "EnableSetTimerLogging"))
    {
        s_setTimerLoggingEnabled = TRUE;
        result = RESULT_SUCCESS;
    }
    else
    {
        N_LOG_NONFATAL();
        result = RESULT_UNKNOWN_COMMAND;
    }

    return result;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Timer_Internal_SetTimer(int32_t timeoutMs)
{
    if ( s_setTimerLoggingEnabled )
    {
        S_SerialComm_TxMessage(TXID, "SetTimer", "%ld", timeoutMs);
    }
}

uint32_t N_Timer_GetSystemTime_Impl(void)
{
    return s_systemTime;
}

void N_Timer_Init(void)
{
    S_SerialComm_Subscribe(TXID, N_Timer_Internal_SerialCommCallback);
}
