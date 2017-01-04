/**************************************************************************//**
  \file tcDeviceTableCtrlHandler.h

  \brief Security Trust Centre devices' permission table control header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.03 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCDEVICETABLECTRLHANDLERH
#define _TCDEVICETABLECTRLHANDLERH

/******************************************************************************
                             Includes section.
******************************************************************************/
#include <sysTypes.h>
#ifdef _PERMISSIONS_
/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief Initializes Trust Centre device permission table.

  \param none.
  \return none.
******************************************************************************/
void tcResetDevicePermissionTable(void);

/**************************************************************************//**
  \brief Finds device in Trust Centre device permission table.

  \param none.
  \return true if device presents, false - otherwise.
******************************************************************************/
bool tcFindDeviceInPermissionTable(ExtAddr_t *extAddr);
#endif
#endif // _TCDEVICETABLECTRLHANDLERH
// eof tcDeviceTableCtrlHandler.h

