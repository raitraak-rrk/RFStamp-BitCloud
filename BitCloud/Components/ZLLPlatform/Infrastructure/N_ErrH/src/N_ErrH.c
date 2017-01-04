/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_ErrH.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_ErrH.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

#if defined __COVERITY__
#include <stdlib.h>
#endif

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_ErrH"

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_ErrH_Callback_t s_pSubscriber = NULL;

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void N_ErrH_Subscribe(N_ErrH_Callback_t pCallback)
{
    N_ERRH_ASSERT_FATAL(s_pSubscriber == NULL);
    s_pSubscriber = pCallback;
}

void N_ErrH_Fatal(const char* compId, uint16_t line)
{
    if ( s_pSubscriber != NULL )
    {
        s_pSubscriber(compId, line);
    }

#if defined(__COVERITY__)
    abort();    // trick to fool Coverity to understand what we're doing - this code is not reachable
#elif defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)
    // there should be a subscriber and it should not return, but just in case...
    for (;;)
    {
        // if in the debugger, check the call stack!
    }
#else
    throw 0;    // throw exception 0 to abort this unit test and report to test framework
#endif
}
