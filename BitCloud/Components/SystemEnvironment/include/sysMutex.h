/**************************************************************************//**
  \file sysMutex.h

  \brief Interface of asynchronous mutex.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-11-23 Max Gekk - Created.
   Last change:
    $Id: sysMutex.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _SYS_MUTEX_H
#define _SYS_MUTEX_H
/**//**
 *  The mutex is similar to binary semaphore with one significant difference:
 * the mutex has an owner. It means that a module (a component) locks a mutex only
 * it can unlock it. If a module tries to unlock a mutex it hasn’t locked
 * (thus doesn’t own) then an error condition is encountered and, most importantly,
 * the mutex is not unlocked.
 **/
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysQueue.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
/** Memory allocation for global mutex. This mutex can be locked by any stack
 * layers and application. */
#define CREATE_MUTEX(mutex) DECLARE_QUEUE(mutex)
/** Declare the global mutex. The mutex must be created in another place. */
#define DECLARE_MUTEX(mutex) extern SYS_Mutex_t mutex

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Mutex type. It's a kind of queue to order incoming lock requests. */
typedef QueueDescriptor_t SYS_Mutex_t;

/** Type of mutex owner. */
typedef struct _SYS_MutexOwner_t
{
  /** Service fields - for internal needs. */
  struct
  {
    QueueElement_t qelem; /**< link used for queuing */
  } service;
  /** Purpose of this parameter is to save owner's execution context. It's up
   * to the owner how to use this parameter. Mutex implementation must not
   * modify this parameter. */
  void *context;
  /** This function is called in the case when the SYS_MutexLock() operation
   * was postponed and now the mutex is locked by the owner. */
  void (*SYS_MutexLockConf)(SYS_Mutex_t *const mutex, void *context);
} SYS_MutexOwner_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Makes the asynchronous request to lock the mutex.

  \ingroup sys

  \code Example:
    DECLARE_MUTEX(aesMutex);
    SYS_MutexOwner_t sspOwnerOfAesMutex = {.SYS_MutexLockConf = callback};

    ...
    if (!SYS_MutexLock(&aesMutex, &sspOwnerOfAesMutex))
      return; // Wait the callback function.
    // Work with the resource which is locked by the mutex.
  \endcode

  \param[in] mutex - the pointer to created mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     is allocated by the owner. Memory can't be reused by owner
                     the mutex is locked by the owner.

  \return true - the locking operation finishes successfuly otherwise
          false - the operation was postponed. The SYS_MutexLockConf() will be
          called when the mutex is locked for this owner.
 ******************************************************************************/
bool SYS_MutexLock(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner);

/**************************************************************************//**
  \brief Unlocks a mutex using synchronous request.
  \note  The mutex will be unlocked immediately.

  \ingroup sys

  \param[in] mutex - the pointer to a locked mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     can be reused after this operation.

  \return true - the unlocking operation finishes successfuly otherwise
          false - the mutex is not locked by the owner.
 ******************************************************************************/
bool SYS_MutexUnlock(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner);

/**************************************************************************//**
  \brief Checks whether mutex is locked.

  \ingroup sys

  \param[in] mutex - the pointer to a locked mutex.
  \param[in] owner - the pointer to owner's structure. Memory for this structure
                     can be reused after this operation.

  \return true - the mutex is locked by the owner,
          false - the mutex is not locked by the owner.
 ******************************************************************************/
bool SYS_IsMutexLocked(SYS_Mutex_t *const mutex, SYS_MutexOwner_t *const owner);

#endif /* _SYS_MUTEX_H */
/** eof sysMutex.h */
