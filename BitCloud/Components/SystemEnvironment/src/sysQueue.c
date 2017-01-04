/**********************************************************************//**
  \file sysQueue.c

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
#include <sysTypes.h>
#include <sysQueue.h>
#include <sysDbg.h>
#include <sysAssert.h>

/***************************************************************************
  Put a element to a queue. Element is added to the tail
  Parameters:
    queue   - pointer to a queue descriptor
    element - pointer to new element
  Returns:
    True - succesfully queued, False - not queued
****************************************************************************/
bool putQueueElem(QueueDescriptor_t *queue, void *element)
{

  if (isQueueElem(queue, element))
  {
    SYS_E_ASSERT_ERROR(false, SYS_ASSERT_ID_DOUBLE_QUEUE_PUT);
    return false;
  }

  ((QueueElement_t*)element)->next = NULL;
  if(!queue->head)
    queue->head = element;
  else
  {
    QueueElement_t *last = queue->head;
    while(last->next)
      last = last->next;
    last->next = element;
  }

  return true;
}

/***************************************************************************
  Delete a head element from a queue. Element is deleted from the head
  Parameters:
    queue - pointer to a queue descriptor
  Returns:
    TRUE  - element is removed successfully
    FALSE - queue is empty
****************************************************************************/
void* deleteHeadQueueElem(QueueDescriptor_t *queue)
{
  if(queue->head)
  {
    void *retval = queue->head;
    queue->head = queue->head->next;
    return retval;
  }
  else
  {
    return NULL;
  }
}

/***************************************************************************
  Delete the certain element of a queue.
  Parameters:
    element - element to be deleted
    queue   - pointer to a queue descriptor
  Returns:
    true if element was removed otherwise false
****************************************************************************/
bool deleteQueueElem(QueueDescriptor_t *queue, void *element)
{
  if (element)
  {
    if(queue->head == element)
    {
      queue->head = queue->head->next;
      return true;
    }
    else
    {
      QueueElement_t *prev = queue->head;
      while (prev && prev->next != element)
        prev = prev->next;
      if (prev)
      {
        prev->next = prev->next->next;
        return true;
      }
    }
  }
  return false;
}

/***************************************************************************
  Check if element is a member of specified queue.
  Parameters:
    queue - pointer to a queue descriptor
    element - pointer to an element
  Returns:
    True - if element is a queue member, false - otherwise.
****************************************************************************/
bool isQueueElem(const QueueDescriptor_t *const queue, const void *const element)
{
  QueueElement_t *it;

  for (it = queue->head; it; it = it->next)
    if (it == element)
      return true;

  return false;
}

//eof sysQueue.c
