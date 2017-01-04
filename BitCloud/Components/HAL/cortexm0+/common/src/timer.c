/**************************************************************************//**
  \file  timer.c

  \brief Functions to manipulate by timers list.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    19/08/13 Agasthian.s - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <bcTimer.h>

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Adds timer to the timer's list.
Parameters:
  head - address of pointer to head of the timers list.
  timer - address of timer that must be added to the list.
  sysTime - current time.
Returns:
  none.
******************************************************************************/
void halAddTimer(Timer_t **head, Timer_t *newTimer, uint32_t sysTime)
{
  if (!*head)
  {
    *head = newTimer;
    return;
  }

  Timer_t *it, *prev = NULL;
  for (it = *head; it; it = it->service.next)
  {
    uint64_t remain;
    if (it->service.sysTimeLabel <= sysTime)
      remain = (uint64_t)(it->service.sysTimeLabel) + it->interval - sysTime;
    else
      remain = (uint64_t)it->interval - (~it->service.sysTimeLabel + 1) - sysTime;
    if ((remain <= UINT32_MAX) && (remain >= newTimer->interval))
      break;
    prev = it;
  }
  if (it == *head)
  {
    newTimer->service.next = *head;
    *head = newTimer;
  }
  else
  {
    prev->service.next = newTimer;
    newTimer->service.next = it;
  }
}

/******************************************************************************
Removes timer from the timers list.
Parameters:
  head - address of pointer to head of the timers list.
  prev - address of the timer before the timer that must be removed from the list.
  p - address of timer that must be removed from the list.
Returns:
  pointer to next cell or pointer to head if deleting is head
******************************************************************************/
Timer_t* halRemoveTimer(Timer_t **head, Timer_t *prev, Timer_t *p)
{
  Timer_t *t;
  if (p == *head)
  {// removing first element of list
    t = p->service.next;
    p->service.next = 0;
    *head = t;
    return *head;
  }
  else
  {
    prev->service.next = p->service.next;
    p->service.next = 0;
    return prev->service.next;
  }
}

/******************************************************************************
The search of the timer in the timers list before one.
Parameters:
  head - address of pointer to head of the timers list.
Returns:
  pointer to saerching timer
******************************************************************************/
Timer_t *halFindPrevTimer(Timer_t **head,  Timer_t *p)
{
  Timer_t *t = *head;

  for (; t ;)
  {
    if (t->service.next == p)
      return t;
    t = t->service.next;
  }
  return NULL;
}

//eof timer.c
