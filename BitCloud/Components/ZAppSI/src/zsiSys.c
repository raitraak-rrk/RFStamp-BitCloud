/***************************************************************************//**
  \file zsiSys.c

  \brief ZAppSI system configuration API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-08  A. Razinkov - Created.
   Last change:
    $Id: zsiSys.c 26492 2014-03-26 10:49:30Z karthik.p_u $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <configServer.h>
#include <zsiSysSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiSysProcessingRoutines[]) =
{
  [ZSI_SYS_READ_PARAMETER_REQUEST] = zsiDeserializeCS_ReadParameterReq,
  [ZSI_SYS_READ_PARAMETER_CONFIRM] = zsiDeserializeCS_ReadParameterConf,
  [ZSI_SYS_WRITE_PARAMETER_REQUEST] = zsiDeserializeCS_WriteParameterReq,
  [ZSI_SYS_WRITE_PARAMETER_CONFIRM] = zsiDeserializeCS_WriteParameterConf
};

ZsiCsParameter_t zsiCsParameter;

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for SYS command deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiSysFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiSysProcessingRoutines))
  {
    memcpy_P(&routine, &zsiSysProcessingRoutines[commandId],
      sizeof(routine));
  }

  return routine;
}

/* eof zsiSys.c */
