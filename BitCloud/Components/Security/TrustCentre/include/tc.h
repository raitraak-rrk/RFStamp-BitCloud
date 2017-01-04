/******************************************************************************//**
  \file tc.h

  \brief Security Trust Centre main header file - includes all others TC public
    header files.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.01 - ALuzhetsky created.
********************************************************************************/
#ifndef _TCH
#define _TCH

/******************************************************************************
                             Include section.
******************************************************************************/
#include <tcAuthentic.h>
#include <tcCommon.h>
#include <tcDbg.h>
#include <tcDeviceTableCtrl.h>
#include <tcKeyUpdate.h>
#include <tcRemove.h>
#include <tcReset.h>
#ifdef _LINK_SECURITY_
  #include <tcKeyEstablish.h>
#endif // _LINK_SECURITY_

#endif // _TCH

// eof tc.h
