/**************************************************************************//**
  \file eeprom.c

  \brief Implementation of the EEPROM interface.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
     07/04/14 karthik.p_u - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(HAL_USE_EE_READY)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <halFlash.h>
#if defined(HAL_USE_EEPROM_EMULATION)
#include <halEeprom.h>
#endif

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  EEPROM_IDLE_STATE,
  EEPROM_READING_STATE,
  EEPROM_WRITING_STATE,
} EepromState_t;

/******************************************************************************
                   Variables section
******************************************************************************/
static EepromState_t halEepromState = EEPROM_IDLE_STATE;
static void (*halEepromDone)();

/******************************************************************************
                   Variables section
******************************************************************************/

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
Reads some number of bytes defined by HAL_EepromParams_t from the EEPROM.
Parameters:
  params - address of HAL_EepromParams_t defined by user.
  readDone - callback method
Returns:
  0  - success.
  -1 - the EEPROM has request that was not completed,
       number of byte to read too much.
  -2 - eeprom is busy
******************************************************************************/
int HAL_ReadEeprom(HAL_EepromParams_t *params, void (*readDone)())
{
#if !defined(HAL_USE_EEPROM_EMULATION)
  uint16_t address;
#endif

  if (EEPROM_IDLE_STATE != halEepromState)
    return -2;
  if (NULL == params)
    return -1;
  if ((uint16_t)(params->address + params->length) > EEPROM_DATA_MEMORY_SIZE)
    return -1;

  halEepromState = EEPROM_READING_STATE;
  halEepromDone = readDone;

#if defined(HAL_USE_EEPROM_EMULATION)
  if (eeprom_emulator_read_buffer(params->address, params->data, params->length) != STATUS_OK)
    return -1;
#else
  address = params->address;
  for (uint16_t i = 0; i < params->length; i++)
    params->data[i] = halFlashRead(address++);
#endif

  if (halEepromDone)
    halPostTask(HAL_EE_READY);
  else
    halEepromState = EEPROM_IDLE_STATE;

  return 0;
}

/******************************************************************************
Writes number of bytes defined by HAL_EepromParams_t to EEPROM.
By writeDone parameter user can control if write operation will be asynchronous
or synchronous.
Parameters:
  params - address of HAL_EepromParams_t defined by user.
  writeDone - address of callback. if writeDone is NULL write operation will be
              synchronous.
Returns:
   0 - success.
  -1 - the EEPROM has request that was not completed,
       number of byte to write too much.
  -2 - eeprom is busy
******************************************************************************/
int HAL_WriteEeprom(HAL_EepromParams_t *params, void (*writeDone)())
{
  HAL_EepromParams_t eepromParams;

  if (EEPROM_IDLE_STATE != halEepromState)
    return -2;
  if (NULL == params)
    return -1;
  if ((uint16_t)(params->address + params->length) > EEPROM_DATA_MEMORY_SIZE)
    return -1;

  halEepromState = EEPROM_WRITING_STATE;
  halEepromDone = writeDone;
  eepromParams = *params;

#if defined(HAL_USE_EEPROM_EMULATION)
  if (eeprom_emulator_write_buffer(eepromParams.address, eepromParams.data, eepromParams.length) != STATUS_OK)
    return -1;
#else
  while (eepromParams.length > 0)
    halFlashWriteEepromPage(&eepromParams);
#endif

  if (halEepromDone)
    halPostTask(HAL_EE_READY);
  else
    halEepromState = EEPROM_IDLE_STATE;

  return 0;
}

/**************************************************************************//**
\brief Stops current write operation.
******************************************************************************/
void HAL_StopWriteToEeprom(void)
{
  halEepromState = EEPROM_IDLE_STATE;
  halEepromDone = NULL;
}

/******************************************************************************
Checks the eeprom state.

Returns:
  true  - eeprom is busy;
  false - eeprom is free;
******************************************************************************/
bool HAL_IsEepromBusy(void)
{
  return (EEPROM_IDLE_STATE != halEepromState);
}

/******************************************************************************
  EEPROM Operation completion handler
******************************************************************************/
void halSigEepromReadyHandler(void)
{
  halEepromState = EEPROM_IDLE_STATE;
  halEepromDone();
}
#endif // defined(HAL_USE_EE_READY)

//eof eeprom.c
