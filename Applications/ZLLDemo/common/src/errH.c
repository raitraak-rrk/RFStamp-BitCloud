/**************************************************************************//**
  \file errH.c

  \brief
    Errors proccesing implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    21.11.12 N. Fomin - Created.
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#include <N_ErrH.h>
#include <errH.h>
#include <resetReason.h>
#include <debug.h>
#include <sysAssert.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define ERROR_HANDLERS_MAX_AMOUNT 5u
#define COMPID                    "ErrH"

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void errHProcessFatalError(const char *compId, uint16_t line);
static void assertHandler(SYS_AssertParam_t *assertParam);

/******************************************************************************
                    Static variables section
******************************************************************************/
static ErrorAbortCb_t errorHandlers[ERROR_HANDLERS_MAX_AMOUNT] = {0};
static uint8_t numHandlers;
static int8_t currentHandler = -1;

/******************************************************************************
                    Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Initializes errH module
******************************************************************************/
void errHInit(void)
{
  N_ErrH_Subscribe(errHProcessFatalError);
  SYS_AssertSubscribe(assertHandler);
}

/**************************************************************************//**
\brief Subscribes for fatal error

\param[in] handler - callback function which is called is case of error
******************************************************************************/
void errHSubscribeForFatal(ErrorAbortCb_t handler)
{
  N_ERRH_ASSERT_FATAL((ERROR_HANDLERS_MAX_AMOUNT - 1) > numHandlers);
  errorHandlers[numHandlers] = handler;
  currentHandler = numHandlers;
  numHandlers++;
}

/**************************************************************************//**
\brief Procceses fatal errors, calls subsribers

\param[in] compId - the name of the module where fatal error occured;
\param[in] line   - the line in the module woth fatal error
******************************************************************************/
static void errHProcessFatalError(const char *compId, uint16_t line)
{
  while (0 <= currentHandler)
  {
    uint8_t tmp = currentHandler;
    currentHandler--;
    // First decrement, then call: if the call fails (calls ErrH_Fatal recursively), it is not
    // retried next time (possibly an infinite loop)!
    (errorHandlers[tmp])(compId, line);
  }

#ifndef _DEBUG_
  HAL_WarmReset();
#endif

  while (1);
}

/**************************************************************************//**
\brief Handler to notify assertion (from down layers)

\param[in] SYS_AssertParam_t * type, holding information about the assertion
******************************************************************************/
static void assertHandler(SYS_AssertParam_t *assertParam)
{
  if (assertParam)
  {
    switch (assertParam->level)
    {
      case FATAL_LEVEL:
      {
        LOG_STRING(assertFtlStr, "FATAL:Code:0x%x \n");
        appSnprintf(assertFtlStr, assertParam->dbgCode);

#ifdef RELEASE
        //HAL_WarmReset();
#endif
        /* ensure the print buffer gets flushes out */
        appSnprintf(assertFtlStr, assertParam->dbgCode);
        appSnprintf(assertFtlStr, assertParam->dbgCode);

        /* fatal is considered irrecoverable.
           go for reset, after backing up needed stuff */
        while (1)
        {
          (void)assertParam->dbgCode;
        }
      }
      break;

      /* Add code here to take appropriate action for a level */
      case ERROR_LEVEL:
      {
        LOG_STRING(assertErrStr, "ERROR:Code:0x%x \n");
        appSnprintf(assertErrStr, assertParam->dbgCode);
      }
      break;

      case WARN_LEVEL:
      {
        //LOG_STRING(assertInfStr, "WARN:Code:0x%x \n");
        //appSnprintf(assertInfStr, assertParam->dbgCode);
      }
      default:
      break;
    }
  }
  else
  {
    LOG_STRING(assertInvStr, "Invalid assert param\n");
    appSnprintf(assertInvStr);
  }
}
/**************************************************************************//**
\brief Subscribes for fatal errors, calls subscribers

\param[in] compId - the name of the module where fatal error occurred;
\param[in] line   - the line in the module with fatal error
******************************************************************************/
void errHFatal (const char *compId, uint16_t line)
{
  (void)compId;
  (void)line;
}
/* eof errH.c */
