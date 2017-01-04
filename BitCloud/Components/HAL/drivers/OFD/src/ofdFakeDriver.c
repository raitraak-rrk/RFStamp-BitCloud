/**************************************************************************//**
\file  ofdFakeDriver.c

\brief Implementation of OTAU fake flash driver.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    2/06/10 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if (APP_USE_OTAU == 1)
#if APP_USE_FAKE_OFD_DRIVER == 1

/******************************************************************************
                   Includes section
******************************************************************************/
#include <ofdExtMemory.h>
#include <appTimer.h>
#include <driversDbg.h>
#include <sysAssert.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define CALL_CALLBACK_TIME      10

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void ofdRunDriverCb(void);
static void ofdRunInfoDriverCb(void);

/******************************************************************************
                   Global variables section
******************************************************************************/
static HAL_AppTimer_t ofdCallbackRunner =
{
  .interval = CALL_CALLBACK_TIME,
  .mode     = TIMER_ONE_SHOT_MODE,
};
static OFD_Callback_t ofdFuncCb = NULL;
static OFD_InfoCallback_t ofdFuncInfoCb = NULL;
static OFD_ImageInfo_t ofdImageInfo =
{
  .type = OFD_IMAGE_WAS_SAVED_FROM_MCU,
  .crc  = 0x00
};

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Run flash driver callback if that has been initialized.
******************************************************************************/
static void ofdRunDriverCb(void)
{
  SYS_E_ASSERT_FATAL(ofdFuncCb, OFD_NULLCALLBACK1);
  ofdFuncCb(OFD_STATUS_SUCCESS);
}

/**************************************************************************//**
\brief Run flash information driver callback if that has been initialized.
******************************************************************************/
static void ofdRunInfoDriverCb(void)
{
  if (ofdFuncInfoCb)
  {
    ofdFuncInfoCb(OFD_STATUS_SUCCESS, &ofdImageInfo);
  }
}

/**************************************************************************//**
\brief Opens serial interface and checks memory type.

\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_Open(OFD_Callback_t cb)
{
  ofdFuncCb = cb;
  ofdCallbackRunner.callback = ofdRunDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Closes serial interface.
******************************************************************************/
void OFD_Close(void)
{
}

/**************************************************************************//**
\brief Erases image in the external memory.

\param[in]
  pos - image position in the external memory
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_EraseImage(OFD_Position_t pos, OFD_Callback_t cb)
{
  (void)pos;
  ofdFuncCb = cb;
  ofdCallbackRunner.callback = ofdRunDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Writes data to the external memory.

\param[in]
  pos - image position for new data
\param[in]
  accessParam - pointer to the access structure
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_Write(OFD_Position_t pos, OFD_MemoryAccessParam_t *accessParam, OFD_Callback_t cb)
{
  (void)pos;
  (void)accessParam;
  ofdFuncCb = cb;
  ofdCallbackRunner.callback = ofdRunDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Flushes data from internal buffer, checks image crc and saves it to
the external memory.

\param[in]
  pos - image position for new data
\param[in]
  countBuff - pointer to the memory for internal data (memory size must be OFD_BLOCK_FOR_CHECK_CRC)
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_FlushAndCheckCrc(OFD_Position_t pos, uint8_t *countBuff, OFD_InfoCallback_t cb)
{
  (void)pos;
  (void)countBuff;
  ofdFuncInfoCb = cb;
  ofdCallbackRunner.callback = ofdRunInfoDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Saves current mcu flash and eeprom to the external memory, checks crc for its
and set command for bootloader.

\param[in]
  whereToSave - image position for current mcu flash and eeprom
\param[in]
  from        - new image position
\param[in]
  copyBuff - pointer to the memory for internal data (memory size must be OFD_BLOCK_FOR_CHECK_CRC)
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_SwitchToNewImage(OFD_Position_t whereToSave, OFD_Position_t from, uint8_t *copyBuff, OFD_Callback_t cb)
{
  (void)whereToSave;
  (void)from;
  (void)copyBuff;
  ofdFuncCb = cb;
  ofdCallbackRunner.callback = ofdRunDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Sets command for bootloader.

\param[in]
  from        - image position
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_ChangeImage(OFD_Position_t from, OFD_Callback_t cb)
{
  (void)from;
  ofdFuncCb = cb;
  ofdCallbackRunner.callback = ofdRunDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief Reads image informations.

\param[in]
  pos - image position
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_ReadImageInfo(OFD_Position_t pos, OFD_InfoCallback_t cb)
{
  (void)pos;
  ofdFuncInfoCb = cb;
  ofdCallbackRunner.callback = ofdRunInfoDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);
}

/**************************************************************************//**
\brief calculates image crc, saves it to eeprom. returns the crc to the caller.
       There will be an unnecessary EEPROM write at the end. But this should not
       create any side effect as it would get overwritten by a new CRC before
       the real image switch
\param[in]
  pos - image position for new data
\param[in]
  countBuff - pointer to the memory for internal data (memory size must be OFD_BLOCK_FOR_CHECK_CRC)
\param[in]
  length - length of the countBuff parameter
\param[in]
  expCrc - expected CRC
\param[in]
  cb - pointer to callback
******************************************************************************/
void OFD_CalCrc(OFD_Position_t pos, uint8_t *countBuff, uint32_t length, uint8_t expCrc, OFD_InfoCallback_t cb)
{
  (void)pos;
  ofdFuncInfoCb = cb;

  ofdImageInfo.crc = expCrc;
  ofdCallbackRunner.callback = ofdRunInfoDriverCb;
  HAL_StartAppTimer(&ofdCallbackRunner);

  (void)countBuff;
  (void)length;
}


#endif // APP_USE_OTAU == 1
#endif // (APP_USE_OTAU == 1)
#endif // APP_USE_FAKE_OFD_DRIVER == 1

// eof ofdFakeDriver.c
