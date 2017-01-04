/**********************************************************************//**
  \file sysQueue.h

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    21/05/07 D. Ovechkin - Created
**************************************************************************/
#ifndef _SYSQUEUE_H
#define _SYSQUEUE_H

#include <sysTypes.h>

/***************************************************************************
  Declare a queue and reset to the default state
  Every queue should be declared with this macros!
  Any queue element passed as parameter to functions should have as first
  field pointer for adding to a queue.

  Parameters:
    queue - the name of object.
  Returns:
    None
****************************************************************************/
#define DECLARE_QUEUE(queue) QueueDescriptor_t queue = {.head = NULL,}

// Type of queue element
typedef struct _QueueElement_t
{
  struct _QueueElement_t *next;
} QueueElement_t;

// Queue descriptor
typedef struct
{
  QueueElement_t *head;
} QueueDescriptor_t;

/***************************************************************************
  Reset a queue
  Parameters:
    queue - pointer to a queue descriptor
  Returns:
    None
****************************************************************************/
INLINE void resetQueue(QueueDescriptor_t *queue)
{
  queue->head = NULL;
}

/***************************************************************************
  Get a element from a queue. Element is got from the head
  Parameters:
    queue - pointer to a queue descriptor
  Returns:
    None
****************************************************************************/
INLINE void *getQueueElem(const QueueDescriptor_t *queue)
{
  return queue->head;
}

/***************************************************************************
  Get next element of queue after current element. The movement is made from
  head to tail. At the beginning of looking for elements the head element
  should be obtained.
  Parameters:
    currElement - current element
  Returns:
    NULL     - no next element
    NOT NULL - next element is got
****************************************************************************/
INLINE void* getNextQueueElem(const void *currElem)
{
  return currElem? ((const QueueElement_t*) currElem)->next: NULL;
}

bool isQueueElem(const QueueDescriptor_t *const queue, const void *const element);
bool putQueueElem(QueueDescriptor_t *queue, void *element);
void *deleteHeadQueueElem(QueueDescriptor_t *queue);
bool  deleteQueueElem(QueueDescriptor_t *queue, void *element);

#endif
//eof sysQueue.h
