/**************************************************************************//**
  \file debug.h

  \brief
    Debugging routines interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    24.06.11 A. Taradov - Created.
******************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <uartManager.h>

/******************************************************************************
                   Definitions section
*******************************************************************************/
// Assert codes
#define BSPOPENBUTTONSASSERT_0              0xf001
#define BSPCLOSEBUTTONSASSERT_0             0xf002
#define REMOTEFSM_JOINDEVICE_0              0xf003
#define NETWORKJOINING_DISCOVERYDONE_0      0xf004
#define NETWORKJOINING_JOINDONE_0           0xf004
#define NETWORKJOINING_STARTNETWORK_0       0xf005
#define NETWORKJOINING_STARTNETWORK_1       0xf006
#define REMOTECONSOLE_RESETDEVICETOFN_0     0xf005
#define REMOTECONSOLE_IDENTIFYDEVICE_0      0xf006
#define APP_ASSERT_0					    0xf100
/******************************************************************************
                    Prototypes section
******************************************************************************/
#if APP_DEVICE_EVENTS_LOGGING == 1
/**************************************************************************//**
\brief Print a formatted string on the LCD
******************************************************************************/
void dbgLcdMsg(const char *format, ...);

#else

#define dbgLcdMsg(...) while (0) {;}

#endif // APP_DEVICE_EVENTS_LOGGING


#endif // _DEBUG_H_

// eof debug.h
