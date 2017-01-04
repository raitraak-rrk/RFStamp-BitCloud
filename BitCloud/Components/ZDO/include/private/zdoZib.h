/**************************************************************************//**
  \file zdoZib.h

  \brief The header file describes the ZDO information base.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-13 Max Gekk - Refactoring.
   Last change:
    $Id: zdoZib.h 23842 2012-12-13 12:31:22Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_ZIB_H
#define _ZDO_ZIB_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <nwkCommon.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
typedef struct _ZIB_t
{
  /* The node descriptor contains information about the capabilities of the ZigBee
   * node and is mandatory for each node. */
  NodeDescriptor_t nodeDescriptor;
  /* The node power descriptor gives a dynamic indication of the power status of the
   * node and is mandatory for each node.*/
  PowerDescriptor_t powerDescriptor;
  /* Duration of a scan procedure. This parameter is used during network formation
   * and joining. */
  ScanDuration_t scanDuration;
  /* The parameter determines the association permit period.
   * 0xff means "always on", 0x00 means "always off. */
  uint8_t permitJoinDuration;
  /* ZDP response wait timeout */
  uint16_t zdpResponseTimeout;
} ZIB_t;

/******************************************************************************
                           External variables section
 ******************************************************************************/
extern ZIB_t csZIB;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Gets information from the Config Server and initializes ZIB.
 ******************************************************************************/
ZDO_PRIVATE void zdoReloadZibFromConfigServer(void);

#endif /* _ZDO_ZIB_H */
/** eof zdoZib.h */
