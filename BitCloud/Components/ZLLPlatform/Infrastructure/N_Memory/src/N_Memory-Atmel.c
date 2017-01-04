/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Memory-Atmel.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_Memory.h"

#include "N_ErrH.h"
#include "N_Log.h"
#include "N_Util.h"

#include <stdlib.h>

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_Memory"

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

void* N_Memory_Alloc(size_t size)
{
    void* p = malloc(size);
    if (p != NULL)
    {
        memset(p, 0, size);
    }
    return p;
}

void* N_Memory_AllocChecked(size_t size)
{
    void* p = N_Memory_Alloc(size);
    N_ERRH_ASSERT_FATAL(p != NULL);
    return p;
}

void N_Memory_Free(void* ptr)
{
    free(ptr);
}
