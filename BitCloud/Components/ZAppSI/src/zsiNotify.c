/***************************************************************************//**
  \file zsiNotify.c

  \brief Implementation of functions for raising ZSI notifications.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-01-23  S. Dmitriev - Created.
 ******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zsiNotify.h>
#include <sysEvents.h>
#include <sysEvents.h>

/******************************************************************************
                    Static functions prototypes section
******************************************************************************/
static void zsiEventListener(SYS_EventId_t ev, SYS_EventData_t data);

/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Subscribe legacy ZSI notifications to events.
******************************************************************************/
void zsiInitNotifications(void)
{
  static SYS_EventReceiver_t zsiEventReceiver = { .func = zsiEventListener};
  
  SYS_SubscribeToEvent(BC_ZSI_LOST_SYNCHRONIZATION, &zsiEventReceiver);
}

/**************************************************************************//**
\brief Listener for ZAppSI events

\param[in] ev - event
\param[in] data - data
******************************************************************************/
void zsiEventListener(SYS_EventId_t ev, SYS_EventData_t data)
{
  ZSI_UpdateNotf_t notf;

  (void) data;

  switch (ev)
  {
    case BC_ZSI_LOST_SYNCHRONIZATION:
      notf.status = ZSI_LOST_SYNCHRONIZATION;
      break;

    default:
      return;
  }

  ZSI_StatusUpdateNotf(&notf);
}