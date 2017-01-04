/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Log.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Log.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"
#include "N_Util.h"
#include "N_Types.h"

#include <stdarg.h>
#include <string.h>

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Log"

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_Log_Callback_t s_pSubscriber = NULL;

static const char* s_compId = NULL;
static N_Log_Level_t s_logLevel;
static const char* s_func;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_Log_Subscribe(N_Log_Callback_t pCallback)
{
    N_ERRH_ASSERT_FATAL(s_pSubscriber == NULL);
    s_pSubscriber = pCallback;
}

void N_Log_Prepare(const char* compId, N_Log_Level_t level, const char* func)
{
    s_compId = compId;
    s_logLevel = level;
    s_func = func;
}

void N_Log_Trace(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    if (s_compId != NULL)   // N_Log_Prepare() should have been called
    {
        if (s_pSubscriber != NULL)
        {
            s_pSubscriber(s_compId, s_logLevel, s_func, format, ap);
        }
    }

    va_end(ap);

    // 'forget' the component ID - require a new N_Log_Prepare()
    s_compId = NULL;
}
