/**************************************************************************//**
  \file bcTimer.h

  \brief Declaration of timer queue interface, hardware-independent module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    7/12/07 A. Khromykh - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _MNHALTIMER_H
#define _MNHALTIMER_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Types section
******************************************************************************/
/** \brief mode of timers  */
typedef enum
{
  TIMER_REPEAT_MODE,
  TIMER_ONE_SHOT_MODE
} TimerMode_t;

/** \brief fields of structure: \n
    uint32_t interval - timer firing interval (set by user) \n
    TimerMode_t mode - timer work mode (set by user). Must be chosen from: \n
           TIMER_REPEAT_MODE \n
           TIMER_ONE_SHOT_MODE \n
    void (*callback)(void) - pointer to timer callback function (set by user). \n
    next - pointer to next cell of list \n
    intervalLeft - absolute fired time */
typedef struct _Timer_t
{
  struct
  {
    struct _Timer_t *next;
    uint32_t sysTimeLabel;
  } service;
  uint32_t interval;
  TimerMode_t mode;
  void (*callback)(void); //!< Must not be set to NULL.
} Timer_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Adds timer to the timer's list. Timers sorted by remaining time.
\param[in]
  head - address of pointer to head of the timers list.
\param[in]
  timer - address of timer that must be added to the list.
\param[in]
  sysTime - current time, used for sorting.
******************************************************************************/
void halAddTimer(Timer_t **head, Timer_t *timer, uint32_t sysTime);

/**************************************************************************//**
\brief Removes timer from the timers list.
\param[in]
  head - address of pointer to head of the timers list.
\param[in]
  prev - address of the timer before the timer that must be removed from the list.
\param[in]
  p - address of timer that must be removed from the list.

\return pointer to next cell or pointer to head if deleting is head
******************************************************************************/
Timer_t* halRemoveTimer(Timer_t **head, Timer_t *prev, Timer_t *p);

/**************************************************************************//**
\brief The search of the timer in the timers list before one.
\param[in]
  head - address of pointer to head of the timers list.

\return pointer to saerching timer
******************************************************************************/
Timer_t *halFindPrevTimer(Timer_t **head,  Timer_t *p);

#endif /* _MNHALTIMER_H */

// eof bcTimer.h
