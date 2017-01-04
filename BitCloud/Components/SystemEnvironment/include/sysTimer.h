/**************************************************************************//**
  \file  sysTimer.h

  \brief Simple interface of HAL application timer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-18 Max Gekk - Created.
   Last change:
    $Id: sysTimer.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _SYS_TIMER_H
#define _SYS_TIMER_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appTimer.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/* Internal states of system timer. */
typedef enum _SYS_TimerState_t
{
  SYS_TIMER_STOPPED = 0x00,
  SYS_TIMER_STARTED = 0x01
} SYS_TimerState_t;

/* Type of the system timer. */
typedef struct _SYS_Timer_t
{
  SYS_TimerState_t state;
  HAL_AppTimer_t timer;
} SYS_Timer_t;


/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Initialize the System Timer.

  \ingroup sys
  
  \param[in] sysTimer - timer pointer will be initialized.
  \param[in] mode - timer mode:  TIMER_ONE_SHOT_MODE or TIMER_REPEAT_MODE.
  \param[in] interval - timer interval (milliseconds).
  \param[in] handler - timer handler function pointer.

  \return None.
 *****************************************************************************/
void SYS_InitTimer(SYS_Timer_t *const sysTimer, const TimerMode_t mode,
  const uint32_t interval, void (*handler)(void));

/******************************************************************************
  \brief Start the HAL Application Timer.

  \ingroup sys
  
  \param[in] appTimer - timer pointer will be started.
  \return None.
 ******************************************************************************/
void SYS_StartTimer(SYS_Timer_t *const sysTimer);

/******************************************************************************
  \brief Stop the HAL Application Timer.

  \ingroup sys
  
  \param[in] appTimer - timer pointer will be stopped.

  \return None.
 *****************************************************************************/
void SYS_StopTimer(SYS_Timer_t *const sysTimer);

#endif /* _SYS_TIMER_H */
/** eof sysTimer.h */
