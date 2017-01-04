/*********************************************************************************************//**
\file  

\brief 

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_ErrH.h 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

#ifndef N_ERRH_H
#define N_ERRH_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"
#include "N_Util.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/** Callback function type. Called when a fatal error occurs.

    The callback function should not return.
*/
typedef void (*N_ErrH_Callback_t)(const char* compId, uint16_t line);

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/** Unconditionally call the fatal error handler.
*/
#define N_ERRH_FATAL() \
    N_UTIL_SWALLOW_SEMICOLON( \
        N_ErrH_Fatal(COMPID, (uint16_t)__LINE__); \
        ) \
        /*lint -unreachable */

/** Check the condition and call the fatal error handler when it is false.
    \param cond The condition to check
    To save space, __FILE__ is not used. The location can be deduced from COMPID
    when used correctly.
*/
#define N_ERRH_ASSERT_FATAL(cond) \
    N_UTIL_SWALLOW_SEMICOLON( \
        if (!(cond)) \
        { \
            N_ERRH_FATAL(); \
            /*lint -unreachable */ \
        } \
    )

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Subscribe callback that is called when a fatal error occurs.

    Only one subscriber is allowed.
*/
void N_ErrH_Subscribe(N_ErrH_Callback_t pCallback);

/** A fatal error occured.

    Not intended to be called directly. Use the macros instead.
*/
void N_ErrH_Fatal(const char* compId, uint16_t line);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_ERRH_H
