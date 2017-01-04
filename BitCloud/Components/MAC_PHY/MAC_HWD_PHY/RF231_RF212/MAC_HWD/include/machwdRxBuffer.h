/******************************************************************************
  \file machwdRxBuffer.h

  \brief Prototypes of Rx Buffer module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    04/06/07 A. Mandychev - Created.
    20/07/07 A. Mandychev - Buffer management is implemented.
  Last change:
    $Id: machwdRxBuffer.h 27584 2015-01-09 14:45:42Z unithra.c $
******************************************************************************/

#ifndef _MACHWDRXBUFFER_H
#define _MACHWDRXBUFFER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <dbg.h>
#include <macDbg.h>
#include <sysTypes.h>
#include <atomic.h>
#include <macBuffers.h>
#include <machwdMemAccess.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  Reset state of receive buffer.
  Parameters: none.
  Returns: none.
******************************************************************************/
void machwdResetRxBuffers(void);

/******************************************************************************
  \brief Get pointer to next buffer in queue.
  \param[in] type - the type of target buffer.
******************************************************************************/
uint8_t *machwdNextBuffer(FrameRxBufferType_t type);

/******************************************************************************
  \brief Release next buffer in queue.
  \param[in] type - the type of target buffer.
******************************************************************************/
void machwdReleaseBuffer(FrameRxBufferType_t type);

/******************************************************************************
  \brief Check if next buffer is present in queue.
  \param[in] type - the type of target buffer.
  \return true if buffer is present.
******************************************************************************/
bool machwdNextBufferPresent(FrameRxBufferType_t type);

/******************************************************************************
                    Inline static functions section
******************************************************************************/
/******************************************************************************
  Check if required amount of space is available in the buffer.
  Parameters:
    length - amount of space required.
  Returns: true if required amount is available.
******************************************************************************/
INLINE bool machwdCheckBufferSpace(FrameRxBufferType_t type, uint8_t length)
{
  FrameRxBufferState_t *rxState = &macMem.hwd.rxBufferState[type];
  bool ok = false;

  SYS_ASSERT(rxState->reserved == NULL, MACHWDRXBUFFER_CHECKBUFFERSPACE_0);

  if (rxState->gap1b == rxState->buffer)
    ok = (rxState->gap2e - rxState->gap2b) > length;

  if (!ok)
    ok = (rxState->gap1e - rxState->gap1b) > length;

  return ok;
}

/******************************************************************************
  Commit buffer and make it available for machwdNextBuffer()
  Parameters: none
  Returns: none
******************************************************************************/
INLINE void machwdCommitRxBuffer(FrameRxBufferType_t type)
{
  FrameRxBufferState_t *rxState = &macMem.hwd.rxBufferState[type];
  SYS_ASSERT(rxState->reserved != NULL, MACHWDRXBUFFER_COMMITRXBUFFER_0);
  rxState->reserved = NULL;
}

/******************************************************************************
  Rollback buffer and discard any changes.
  Parameters: none
  Returns: none
******************************************************************************/
INLINE void machwdRollbackRxBuffer(FrameRxBufferType_t type)
{
  FrameRxBufferState_t *rxState = &macMem.hwd.rxBufferState[type];

  SYS_ASSERT(rxState->reserved != NULL, MACHWDRXBUFFER_ROLLBACKRXBUFFER_0);

  if (rxState->gap1b == rxState->buffer)
    rxState->gap2b = rxState->reserved;
  else
    rxState->gap1b = rxState->reserved;

  rxState->reserved = NULL;
}

/******************************************************************************
  Reserve buffer of specified length. Later this buffer must be commited or rolled back.
  Parameters:
    length - amount of space required.
  Returns: pointer to the buffer.
******************************************************************************/
INLINE uint8_t *machwdGetBuffer(FrameRxBufferType_t type, uint8_t length)
{
  FrameRxBufferState_t *rxState = &macMem.hwd.rxBufferState[type];
  uint8_t *ptr;

  SYS_ASSERT(rxState->reserved == NULL, MACHWDRXBUFFER_GETBUFFER_0);

  if (rxState->gap1b == rxState->buffer)
    if ((rxState->gap2e - rxState->gap2b) > length)
    {
      rxState->reserved = rxState->gap2b;
      *(rxState->gap2b++) = length;
      ptr = rxState->gap2b;
      rxState->gap2b += length;
      return ptr;
    }

  if ((rxState->gap1e - rxState->gap1b) > length)
  {
    rxState->reserved = rxState->gap1b;
    *(rxState->gap1b++) = length;
    ptr = rxState->gap1b;
    rxState->gap1b += length;
    return ptr;
  }

  return NULL;
}

#endif /* _MACHWDRXBUFFER_H */

// eof machwdRxBuffer.h
