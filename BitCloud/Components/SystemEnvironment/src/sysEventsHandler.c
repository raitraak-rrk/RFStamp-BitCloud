/**************************************************************************//**
\file  sysEventsHandler.c

\brief Event facility

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    28/02/12 D. Loskutnikov - Created
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysQueue.h>
#include <sysDbg.h>
#include <sysEvents.h>
#include <sysAssert.h>

/******************************************************************************
                    Static variables section
******************************************************************************/
// Queue of registered event receivers
static DECLARE_QUEUE(eventReceivers);
// Mask for fast query if event have at least one subscriber
static sysEvWord_t subscribed[SYS_EVENTS_MASK_SIZE];

static const unsigned int evWordNBits = sizeof(sysEvWord_t) * 8U;

/******************************************************************************
                    Implementation section
******************************************************************************/

/**************************************************************************//**
\brief Subscribe receiver to event. Same receiver may be subscribed to
multiple events by calling this function several times

\param[in] id - event id
\param[in] recv - receiver description
******************************************************************************/
void SYS_SubscribeToEvent(SYS_EventId_t id, SYS_EventReceiver_t *recv)
{
  const int pos = id / evWordNBits;
  const sysEvWord_t mask = 1U << (id % evWordNBits);

  SYS_E_ASSERT_FATAL((id <  SYS_MAX_EVENTS), SYS_ASSERT_WRONG_EVENT_SUBSCRIBE);

  // Stop processing, if receiver is already subscribed.
  if (SYS_IsEventSubscriber(id, recv))
    return;

  if (!isQueueElem(&eventReceivers, recv))
  {
    // Clear events mask (we're not in queue and therefore not subscribed to any event)
    memset(recv->service.evmask, 0U, sizeof(recv->service.evmask));
    putQueueElem(&eventReceivers, recv);
  }

  // Update receiver's mask and fast query mask
  recv->service.evmask[pos] |= mask;
  subscribed[pos] |= mask;
}

/**************************************************************************//**
\brief Unsubscribe receiver from event

\param[in] id - event id
\param[in] recv - receiver description
******************************************************************************/
void SYS_UnsubscribeFromEvent(SYS_EventId_t id, SYS_EventReceiver_t *recv)
{
  const int pos = id / evWordNBits;
  const sysEvWord_t mask = 1U << (id % evWordNBits);
  bool more_subscribers = false;

  SYS_E_ASSERT_FATAL((id < SYS_MAX_EVENTS), SYS_ASSERT_WRONG_EVENT_SUBSCRIBE);

  // Stop processing, if receiver wasn't subscribed.
  if (!SYS_IsEventSubscriber(id, recv))
    return;

  // Update receiver's mask
  recv->service.evmask[pos] &= ~mask;

  // Look if there is more subscribers for event
  for (const SYS_EventReceiver_t *hnd = getQueueElem(&eventReceivers); hnd; hnd = getNextQueueElem(hnd))
  {
    if (hnd->service.evmask[pos] & mask)
    {
      more_subscribers = true;
      break;
    }
  }

  // No more subscribers for event. Update fast query mask.
  if (!more_subscribers)
    subscribed[pos] &= ~mask;

// Dequeuing receiver is dangerous since user could unsubscribe and modify queue
// while being called by PostEvent's event delivery loop. Disabled for now.
#if 0
  for (int i = ARRAY_SIZE(recv->service.evlist), i, i--)
  {
    if (recv->service.evlist[i])
      return;
  }
  // No more subscriptions, dequeue receiver
  deleteQueueElem(eventReceivers, recv);
#endif
}

/**************************************************************************//**
\brief Post an event to be delivered to all subscribed receivers

\param[in] id - event id
\param[in] data - associated data
******************************************************************************/
void SYS_PostEvent(SYS_EventId_t id, SYS_EventData_t data)
{
  const int pos = id / evWordNBits;
  const sysEvWord_t mask = 1U << (id % evWordNBits);

  SYS_E_ASSERT_FATAL((id <  SYS_MAX_EVENTS), SYS_ASSERT_WRONG_EVENT_POST);

  if (!(subscribed[pos] & mask))  // There is no one listening
    return;

  for (const SYS_EventReceiver_t *hnd = getQueueElem(&eventReceivers); hnd; hnd = getNextQueueElem(hnd))
  {
    if (hnd->service.evmask[pos] & mask)
    {
      SYS_E_ASSERT_FATAL(hnd->func, SYS_POSTEVENT_NULLCALLBACK0);
      hnd->func(id, data);
    }
  }
}

/**************************************************************************//**
\brief Check if event have at least one subscriber

\param[in] id - event if
\return result
******************************************************************************/
bool SYS_IsEventDeliverable(SYS_EventId_t id)
{
  return subscribed[id / evWordNBits] & (1U << (id % evWordNBits));
}

/**************************************************************************//**
\brief Check if specified receiver is subscribed to event.

\param[in] id - event if
\param[in] recv - receiver description

\return result
******************************************************************************/
bool SYS_IsEventSubscriber(SYS_EventId_t id, SYS_EventReceiver_t *recv)
{
  const int pos = id / evWordNBits;
  const sysEvWord_t mask = 1U << (id % evWordNBits);

  return recv->service.evmask[pos] & mask;
}

// eof sysEventsHandler.c
