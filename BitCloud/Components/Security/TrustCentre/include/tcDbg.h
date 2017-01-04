/**************************************************************************//**
  \file tcDbg.h

  \brief Security Trust Centre debug header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2007.12 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCDBGH
#define _TCDBGH

/******************************************************************************
                             Includes section.
******************************************************************************/
#include <dbg.h>

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief Trust Centre debug codes declaration.

   These codes are used for debuging.
******************************************************************************/
typedef enum
{
  TCAUTHENTICHANDLERC_APSUPDATEDEVICEINDICATION_01    = 0x7000,
  TCAUTHENTICHANDLERC_APSUPDATEDEVICEINDICATION_02    = 0x7001,
  TCAUTHENTICHANDLERC_TCAUTHENTICTASKHANDLER_01       = 0x7002,
  TCKEYUPDATEHANDLER_TCKEYUPDATETASKHANDLER_00        = 0x7003,
  TCAUTHENTICHANDLERC_APSREMOVEDEVICECONF_00          = 0x7004,
  TCKEYUPDATEHANDLERC_ENDUPDATENETWORKKEYOPERATION_00 = 0x7005,
  TCDEVICETABLECTRLHANDLER_TCSETDEVICEKEY_00          = 0x7006,
  TCAUTHENTICHANDLERC_APSESTABLISHKEYCONF_00          = 0x7007,
  TCTASKMANAGER_00                                    = 0x7008,
  TCAUTHENTICHANDLERC_APSUPDATEDEVICEINDICATION_03    = 0x7009,
  TCAUTHENTICHANDLERC_APSTRANSPORTKEYCONF_00          = 0x700A,
  TC_KEYUPDATE_NULLCALLBACK0                          = 0x700B,
  TC_REMOVEDEVICE_NULLCALLBACK0                       = 0x700C,
} TcDbgCodeId_t;
#endif // _TCDBGH

// eof tcDbg.h
