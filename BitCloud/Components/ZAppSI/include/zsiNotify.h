/**************************************************************************//**
  \file zsiNotify.h

  \brief Functions for raising ZSI notifications

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    2013-01-23  S. Dmitriev - Created.
   Last change:
    $Id: zsiNotify.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/
#ifndef _ZSI_NOTIFY_H
#define _ZSI_NOTIFY_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef enum
{
  ZSI_LOST_SYNCHRONIZATION = 0x00,
}ZSI_Status_t;

typedef struct _ZSI_UpdateNotf_t
{
  ZSI_Status_t status;
}ZSI_UpdateNotf_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
\brief Indicates zsi status update.
******************************************************************************/
void ZSI_StatusUpdateNotf(ZSI_UpdateNotf_t *notf);

/**************************************************************************//**
\brief Subscribe legacy ZSI notifications to events.
******************************************************************************/
void zsiInitNotifications(void);

#endif // _ZSI_NOTIFY_H
// eof zsiNotify.h
