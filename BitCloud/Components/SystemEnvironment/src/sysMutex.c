/**************************************************************************//**
  \file sysMutex.c

  \brief Implementation of system asynchronous mutex.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-11-23 Max Gekk - Created.
   Last change:
    $Id: sysMutex.c 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysMutex.h>
#include <sysDbg.h>
#include <sysUtils.h>
#include <sysAssert.h>

/******************************************************************************
                            Implementations section
 ******************************************************************************/
/**************************************************************************//**
  \brief Puts owner's structure to the mutex queue.

  \param[in] mutex - the pointer to a mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     is allocated by the owner. Memory can't be reused by owner
                     the mutex is locked by the owner.

  \return true - the locking operation finishes successfuly otherwise
          false - the operation was postponed. The SYS_MutexLockConf() will be
          called when the mutex is locked for this owner.
 ******************************************************************************/
bool SYS_MutexLock(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner)
{
  const bool isUnlocked = (NULL == getQueueElem(mutex));

  SYS_E_ASSERT_FATAL(((NULL != mutex) && (NULL != owner)), SYSMUTEX_MUTEXLOCK0);
  putQueueElem(mutex, &owner->service.qelem);

  return isUnlocked;
}

/**************************************************************************//**
  \brief Unlocks a mutex using synchronous request.

  \param[in] mutex - the pointer to a locked mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     can be reused after this operation.

  \return true - the locking operation finishes successfuly otherwise
          false - the mutex is not locked by the owner.
 ******************************************************************************/
bool SYS_MutexUnlock(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner)
{
  QueueElement_t *qelem;
  SYS_MutexOwner_t *currentOwner;

  SYS_E_ASSERT_FATAL(((NULL != mutex) && (NULL != owner)), SYSMUTEX_MUTEXUNLOCK0);

  qelem = getQueueElem(mutex);
  if (NULL == qelem)
  {
    SYS_E_ASSERT_FATAL(false, SYSMUTEX_MUTEXUNLOCK1);
    return false;
  }

  currentOwner = GET_PARENT_BY_FIELD(SYS_MutexOwner_t, service.qelem, qelem);
  if (owner != currentOwner)
  {
    SYS_E_ASSERT_FATAL(false, SYSMUTEX_MUTEXUNLOCK2);
    return false;
  }
  else
    deleteHeadQueueElem(mutex);

  qelem = getQueueElem(mutex);
  if (NULL != qelem)
  {
    currentOwner = GET_PARENT_BY_FIELD(SYS_MutexOwner_t, service.qelem, qelem);
    SYS_E_ASSERT_FATAL(currentOwner->SYS_MutexLockConf, SYSMUTEX_MUTEXUNLOCK3);
    currentOwner->SYS_MutexLockConf(mutex, currentOwner->context);
  }

  return true;
}

/**************************************************************************//**
  \brief Checks whether mutex is locked.

  \ingroup sys

  \param[in] mutex - the pointer to a locked mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     can be reused after this operation.

  \return true - the mutex is locked by the owner,
          false - the mutex is not locked by the owner.
 ******************************************************************************/
bool SYS_IsMutexLocked(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner)
{
  SYS_E_ASSERT_FATAL(((NULL != mutex) && (NULL != owner)), SYSMUTEX_ISMUTEXLOCKED0);

  return isQueueElem(mutex, &owner->service.qelem);
}

/** eof sysMutex.c */
