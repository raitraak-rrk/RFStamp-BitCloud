
/***************************************************************************//**
  \file zsiHal.c

  \brief ZAppSI APS API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Viswanadham Kotla - Created.
   Last change:
    $Id: zsiAps.c 24441 2013-02-07 12:02:29Z viswanadhamKotla $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiHalSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>
#include <sysQueue.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiHalProcessingRoutines[]) =
{
  [ZSI_HAL_RESET_REQUEST]           = zsiDeserializeHAL_ResetReq,
  [ZSI_HAL_RESET_CONFIRM]           = zsiDeserializeHAL_ResetConf
};
/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for HAL commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiHalFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiHalProcessingRoutines))
  {
    memcpy_P(&routine, &zsiHalProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}
#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief HAL_WarmReset handler wrapper routine.

  \param[in] req - primitive pointer.

  \return None.
 ******************************************************************************/
void ZSI_ResetNP(void)
{
   uint8_t status;
   uint8_t* req = &status ;
   zsiProcessCommand(ZSI_SREQ_CMD, req, zsiSerializeHAL_ResetReq,
                    &status);
}

#endif
