/**************************************************************************//**
  \file tcRemove.h
  \brief Security Trust Centre remove primitive header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.01.20 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCREMOVEH
#define _TCREMOVEH

#include <tcCommon.h>
#include <macAddr.h>

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  uint8_t status;
} TC_RemoveDeviceConf_t;

/**************************************************************************//**
  \brief TBD.

  TBD
******************************************************************************/
typedef struct
{
  TC_Service_t service;
  ExtAddr_t    deviceAddr;
  ExtAddr_t    parentAddr;
  void (*TC_RemoveDeviceConf)(TC_RemoveDeviceConf_t *conf);
  TC_RemoveDeviceConf_t confirm; //!< Callback function's pointer. Must not be set to NULL.
} TC_RemoveDeviceReq_t;

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
void TC_RemoveDeviceReq(TC_RemoveDeviceReq_t *param);

#endif // _TCREMOVEH

// eof tcRemove.h
