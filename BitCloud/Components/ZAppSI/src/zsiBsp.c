/***************************************************************************//**
  \file zsiBsp.c

  \brief ZAppSI BSP API wrapper.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id$
 ******************************************************************************/
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiBspSerialization.h>
#include <zsiDriver.h>
#include <zsiDbg.h>
#include <sysQueue.h>

/******************************************************************************
                              Static variables section
******************************************************************************/
static PROGMEM_DECLARE(ZsiProcessingRoutine_t zsiBspProcessingRoutines[]) =
{
  [ZSI_BSP_READ_UID_REQUEST]    = zsiDeserializeBSP_ReadUidReq,
  [ZSI_BSP_READ_UID_CONFIRM]    = zsiDeserializeBSP_ReadUidConf
};

#ifdef ZAPPSI_HOST
static ExtAddr_t zsiUid;
#endif

/******************************************************************************
                               Implementation section
 ******************************************************************************/
/**************************************************************************//**
  \brief Finds routine for BSP commands deserialization.

  \param[in] commandId - command identifier.

  \return Pointer to appropriate routine.
 ******************************************************************************/
ZsiProcessingRoutine_t zsiBspFindProcessingRoutine(uint8_t commandId)
{
  ZsiProcessingRoutine_t routine = NULL;

  if (commandId < ARRAY_SIZE(zsiBspProcessingRoutines))
  {
    memcpy_P(&routine, &zsiBspProcessingRoutines[commandId],
             sizeof(routine));
  }

  return routine;
}

#ifdef ZAPPSI_HOST
/**************************************************************************//**
  \brief Reads the UID from the chip

  \return uid - extended address
 ******************************************************************************/
int BSP_ReadUid(uint64_t *id)
{
  ZsiEntityService_t service;

  zsiProcessCommand(ZSI_SREQ_CMD, &service, zsiSerializeBSP_ReadUidReq,
                    &zsiUid);
  *id = zsiUid;
  return 0;
}
#endif /* ZAPPSI_HOST */
/* eof zsiBsp.c */