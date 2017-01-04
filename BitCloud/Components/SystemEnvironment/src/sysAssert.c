/***************************************************************************//**
  \file sysAssert.c

  \brief
   This component provides a Assert facility. The component provides application
   to register hooks that are invoked when Assert is encountered in execution. 
   Application handler should log the information and take corrective action.

  \author Atmel Corporation: http://www.atmel.com
  \n  Support email: avr@atmel.com
  \n Copyright (c) 2014, Atmel Corporation. All rights reserved.
  \n Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal

  $Id: sysAssert.c 2014-04-12 21:25:35Z mukesh $

*******************************************************************************/

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <sysAssert.h>
#ifndef MAP_ALL_ASSERT_LEVEL_TO_SYSASSERT
/******************************************************************************
                              Definitions section
 ******************************************************************************/

void SYS_AssertSubscribe(SYS_AssertCallback_t pCallback);

/******************************************************************************
                              Global variable section
 ******************************************************************************/

SYS_AssertParam_t gAssertParam;

#ifdef  USE_DBGCODE
uint16_t gAssertDbgCode;
#endif

#ifdef  USE_COMPID
uint16_t gAssertCompId;
#endif


#ifdef  USE_LINENUM
int16_t gAssertLine;
#endif

#ifdef USE_FILENAME
const char *gAssertFile;
#endif

SYS_AssertCallback_t s_pAssertSubscriber;


/** Subscribe callback, callback will be called when a assert occurs.
    Only one subscriber is allowed.
*/
void SYS_AssertSubscribe(SYS_AssertCallback_t pCallback)
{
  if (pCallback)
    s_pAssertSubscriber = pCallback;
}


/**************************************************************************//**
  \brief SYS_DefAssertCallbackWarn callback invoked on WARN LEVEL ASSERTS
  if enabled       
  \param[in] None
  \return None
******************************************************************************/
void SYS_DefAssertCallbackWarn()
{
  gAssertParam.level   = WARN_LEVEL;

#ifdef USE_DBGCODE
  gAssertParam.dbgCode = gAssertDbgCode;
#endif
#ifdef USE_LINENUM
  gAssertParam.line    = gAssertLine;
#endif
#ifdef USE_COMPID 
  gAssertParam.componentId =  gAssertCompId;
#endif
#ifdef USE_FILENAME
  gAssertParam.file = gAssertFile;
#endif

  if ( NULL != s_pAssertSubscriber ) 
    s_pAssertSubscriber(&gAssertParam); 
} 
 
/**************************************************************************//**
  \brief SYS_DefAssertCallbackError callback invoked on ERROR LEVEL ASSERTS
  if enabled       
  \param[in] None
  \return None
******************************************************************************/
void SYS_DefAssertCallbackError()
{
  gAssertParam.level   = ERROR_LEVEL;
   
#ifdef USE_DBGCODE
  gAssertParam.dbgCode = gAssertDbgCode;
#endif
#ifdef USE_LINENUM
  gAssertParam.line    = gAssertLine;
#endif
#ifdef USE_COMPID 
  gAssertParam.componentId =  gAssertCompId;
#endif
#ifdef USE_FILENAME
  gAssertParam.file = gAssertFile;
#endif

  if ( NULL !=  s_pAssertSubscriber  ) 
    s_pAssertSubscriber(&gAssertParam); 
} 

/**************************************************************************//**
  \brief SYS_DefAssertCallbackFatal callback called on FATAL LEVEL ASSERTS
  if enabled, This function invokes registered callback
  \param[in] None
  \return None
******************************************************************************/
void SYS_DefAssertCallbackFatal()
{
  gAssertParam.level   = FATAL_LEVEL;

#ifdef USE_DBGCODE
  gAssertParam.dbgCode = gAssertDbgCode;
#endif
#ifdef USE_LINENUM
  gAssertParam.line    = gAssertLine;
#endif
#ifdef USE_COMPID 
  gAssertParam.componentId =  gAssertCompId;
#endif
#ifdef USE_FILENAME
  gAssertParam.file = gAssertFile;
#endif

  if ( NULL != s_pAssertSubscriber ) 
    s_pAssertSubscriber(&gAssertParam); 
} 
#endif
