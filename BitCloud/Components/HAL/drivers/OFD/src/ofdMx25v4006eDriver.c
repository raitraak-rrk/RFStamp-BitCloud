/**************************************************************************//**
\file  ofdmx25v4006eDriver.c

\brief Implementation of chip-flash interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    05/06/2015 Unithra.c - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if (EXTERNAL_MEMORY == MX25V4006E) && (APP_USE_OTAU == 1)
#if APP_USE_FAKE_OFD_DRIVER == 0

/******************************************************************************
                   Includes section
******************************************************************************/
#include <ofdExtMemory.h>
#include <ofdMemoryDriver.h>
#include <spi.h>
#include <eeprom.h>
#include <appTimer.h>
#include <driversDbg.h>
#include <halRfCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SPI_BUSY_POLL_PERIOD      60
// cycles of counting flash crc before context gap.
#define ATOMIC_COUNTING           128
#define EEPROM_OK                  0
#define EEPROM_BUSY                -2

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum
{
  FLASH_BUSY,
  FLASH_READY
} FlashBusyState_t;

typedef enum
{
  FLASH_TRANSACTION,
  EEPROM_TRANSACTION
} DelayedTransactionType_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static inline uint8_t ofdReadStatusRegister(void);
static inline void ofdStartPollTimer(DelayedTransactionType_t type);
static void ofdReadImageTable(void);
static void ofdSaveCrcCallback(void);
static void ofdEraseChip(void);
static void eraseTimerFired(void);

static void ofdSaveCurrentEepromImage(void);
static void ofdFlushCrcCallback(OFD_Status_t status, OFD_ImageInfo_t *pInfo);
static void ofdSaveCurrentEepromImageContinue(void);
static void ofdPollBusyState(void);
static void ofdStartFlashDelayedTransaction(void);
static void ofdStartEepromDelayedTransaction(void);
uint8_t ofdReadInternalFlash(uint32_t flashAddress);
#if defined(_OFD_DEBUG_)
void ofdReadData(uint32_t address, uint8_t *data, uint16_t size, OFD_Callback_t cb);
#endif
static bool ofdEepromHandler(int result, void(* action)());
/******************************************************************************
                   External variables section
******************************************************************************/
extern HAL_SpiDescriptor_t spiDesc;
extern OFD_Position_t ofdPosition;
extern OFD_Position_t actionSector;
extern OFD_Callback_t ofdCallback;
extern OFD_Callback_t ofdAuxCallback;
extern OFD_MemoryAccessParam_t localAccessStructure;
extern OFD_ImageInfo_t imageInfo;
extern OfdImageTable_t imageTable;
extern OfdInternalMemoryAccessParam_t internalAccessParam;

/******************************************************************************
                   Global variables section
******************************************************************************/
static HAL_AppTimer_t ofdBusyTimer =
{
  .interval = SPI_BUSY_POLL_PERIOD,
  .mode     = TIMER_ONE_SHOT_MODE,
};
static HAL_AppTimer_t eraseTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 5000,
  .callback = eraseTimerFired
};
void (* delayedTransaction)(void) = NULL;
#if defined(_OFD_DEBUG_)
static HAL_UsartDescriptor_t usartDescriptor;
static uint32_t debugOffset = 0ul;
static uint8_t  debugBuffer[OFD_BLOCK_SIZE];
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Checks flash station.

\return flash states: \n
  FLASH_BUSY  \n
  FLASH_READY
******************************************************************************/
static inline FlashBusyState_t ofdCheckBusyState(void)
{
  uint8_t statusReg = ofdReadStatusRegister();

  if (STATUS_WRITE_INPROGRESS == (statusReg & STATUS_WRITE_INPROGRESS))
    return FLASH_BUSY;

  return FLASH_READY;
}

/**************************************************************************//**
\brief Starts flash delayed transaction.
******************************************************************************/
static void ofdStartFlashDelayedTransaction(void)
{
  if (FLASH_READY == ofdCheckBusyState())
    delayedTransaction();
  else
    ofdStartPollTimer(FLASH_TRANSACTION);
}

/**************************************************************************//**
\brief Starts eeprom delayed transaction.
******************************************************************************/
static void ofdStartEepromDelayedTransaction(void)
{
  if (!HAL_IsEepromBusy())
    delayedTransaction();
  else
    ofdStartPollTimer(EEPROM_TRANSACTION);
}

/**************************************************************************//**
\brief Starts timer for start delayed transaction.

\param[in]
  type - transaction type (flash or eeprom)
******************************************************************************/
static inline void ofdStartPollTimer(DelayedTransactionType_t type)
{
  if (FLASH_TRANSACTION == type)
    ofdBusyTimer.callback = ofdStartFlashDelayedTransaction;
  else
    ofdBusyTimer.callback = ofdStartEepromDelayedTransaction;

  HAL_StartAppTimer(&ofdBusyTimer);
}

/**************************************************************************//**
\brief Returns SUCCESS status.
******************************************************************************/
static void ofdReturnSuccessStatus(void)
{
  sysAssert(ofdCallback, OFD_NULLCALLBACK3);
  ofdCallback(OFD_STATUS_SUCCESS);
}

/**************************************************************************//**
\brief Reads manufacturer ID and chip ID.
******************************************************************************/
void ofdFindStorageSpace(void)
{
  uint64_t manufacId = RDID;

  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdFindStorageSpace;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  GPIO_EXT_MEM_CS_clr();
  HAL_Delay(10);
  HAL_ReadSpi(&spiDesc, (uint8_t *)&manufacId, sizeof(uint64_t));
  GPIO_EXT_MEM_CS_set();

  if (OFD_MANUFACTURER_ID == (uint8_t)(manufacId >> 8))
  {
    if (OFD_DEVICE_ID_1 == (uint8_t)(manufacId >> 16))
    {
      // read image table to global variable
      ofdReadImageTable();
      return;
    }
  }

  sysAssert(ofdCallback, OFD_NULLCALLBACK5);
  ofdCallback(OFD_STATUS_UNKNOWN_EXTERNAL_FLASH_TYPE);
}

/**************************************************************************//**
\brief Reads status register from the external flash.

\return status register
******************************************************************************/
static inline uint8_t ofdReadStatusRegister(void)
{
  uint8_t regStatus = RDSR;

  GPIO_EXT_MEM_CS_clr();
  HAL_Delay(10);
  HAL_ReadSpi(&spiDesc, (uint8_t *)&regStatus, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();

  return regStatus;
}

/**************************************************************************//**
\brief Sends "write enable" command to the external flash.
******************************************************************************/
void ofdSendWriteEnable(void)
{
  uint8_t wren = WREN;

  GPIO_EXT_MEM_CS_clr();
  HAL_Delay(10);
  HAL_WriteSpi(&spiDesc, &wren, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();
}

/**************************************************************************//**
\brief Timer callback for erase operation completion.
******************************************************************************/
static void eraseTimerFired(void)
{
  ofdReturnSuccessStatus();
}

/**************************************************************************//**
\brief Starts image erasing in the external memory.
******************************************************************************/
void ofdEraseImage(void)
{
  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdEraseImage;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }
  ofdEraseChip();

  HAL_StartAppTimer(&eraseTimer);
}

/**************************************************************************//**
\brief Starts entire external memory erasing.
******************************************************************************/
static void ofdEraseChip(void)
{
  uint8_t eraseChip = CHIP_ERASE;

  ofdSendWriteEnable();
  GPIO_EXT_MEM_CS_clr();
  HAL_Delay(10);
  HAL_WriteSpi(&spiDesc, (uint8_t *)&eraseChip, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();
}

/**************************************************************************//**
\brief Writes data to the external memory.
******************************************************************************/
void ofdWriteData(void)
{
  uint8_t *dataPointer;
  uint16_t dataLength;
  uint32_t dataAddress = 0;
  uint8_t writeInstruc = PAGE_PROGRAM;


  if (!localAccessStructure.length)
  {
    ofdReturnSuccessStatus();
    return;
  }

  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdWriteData;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  dataAddress = OFD_LITTLE_TO_BIG_ENDIAN(localAccessStructure.offset);
  dataAddress >>= 8;
  dataPointer = localAccessStructure.data;
  dataLength = PAGE_SIZE - (uint8_t)localAccessStructure.offset;
  if (dataLength >= localAccessStructure.length)
    dataLength = localAccessStructure.length;

  localAccessStructure.data += dataLength;
  localAccessStructure.offset += dataLength;
  localAccessStructure.length -= dataLength;

  ofdSendWriteEnable();
  GPIO_EXT_MEM_CS_clr();
  HAL_Delay(10);
  HAL_WriteSpi(&spiDesc, &writeInstruc, sizeof(uint8_t));
  HAL_WriteSpi(&spiDesc, (uint8_t *)&dataAddress, sizeof(uint32_t)-1);
  HAL_WriteSpi(&spiDesc, dataPointer, dataLength);
  GPIO_EXT_MEM_CS_set();

  delayedTransaction = ofdWriteData;
  ofdStartPollTimer(FLASH_TRANSACTION);
}

/**************************************************************************//**
\brief Calls callback about end of eeprom saving.
******************************************************************************/
static void ofdSaveCrcCallback(void)
{
  sysAssert(ofdCallback, OFD_NULLCALLBACK6);
  ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_SUCCESS, &imageInfo);
}

/**************************************************************************//**
\brief Saves image table to the internal eeprom.
******************************************************************************/
void ofdSaveImageTable(void)
{
  HAL_EepromParams_t params;

  params.address = OFD_SERVICE_INFO_SIZE-1;
  params.data = &imageTable;
  params.length = sizeof(OfdImageTable_t);

  if (!ofdEepromHandler(HAL_WriteEeprom(&params, ofdSaveCrcCallback), ofdSaveImageTable))
  {
    sysAssert(ofdCallback, OFD_NULLCALLBACK7);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

/**************************************************************************//**
\brief Saves crc to the internal eeprom.
******************************************************************************/
void ofdSaveCrc(void)
{
  HAL_EepromParams_t params;

  params.address = ofdPosition + 1;
  params.data = &imageInfo.crc;
  params.length = sizeof(uint8_t);

  if (!ofdEepromHandler(HAL_WriteEeprom(&params, ofdSaveImageTable), ofdSaveCrc))
  {
    sysAssert(ofdCallback, OFD_NULLCALLBACK8);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

/**************************************************************************//**
\brief Checks image crc.
******************************************************************************/
void ofdCheckCrc(void)
{
  uint32_t address;
  uint8_t writeInstruc = READ;
  uint8_t atomicCounting = ATOMIC_COUNTING;

  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdCheckCrc;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  while (atomicCounting--)
  {
    address = localAccessStructure.offset;
    address = OFD_LITTLE_TO_BIG_ENDIAN(address<<8);
    GPIO_EXT_MEM_CS_clr();
    HAL_Delay(10);
    HAL_WriteSpi(&spiDesc, &writeInstruc, sizeof(uint8_t));
    HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t)-1);
    HAL_ReadSpi(&spiDesc, localAccessStructure.data, OFD_BLOCK_FOR_CHECK_CRC);
    GPIO_EXT_MEM_CS_set();  // release spi cs
    imageInfo.crc = ofdCrc(imageInfo.crc, localAccessStructure.data, OFD_BLOCK_FOR_CHECK_CRC);
    localAccessStructure.offset += OFD_BLOCK_FOR_CHECK_CRC;
    localAccessStructure.length -= OFD_BLOCK_FOR_CHECK_CRC;
    if (!localAccessStructure.length)
    {
      ofdSaveCrc();
      return;
    }
  }
  // context gap
  delayedTransaction = ofdCheckCrc;
  ofdStartPollTimer(FLASH_TRANSACTION);
}

/**************************************************************************//**
\brief Callback for saving internal flash.

\param[in] status - status of the data flash writing
******************************************************************************/
void ofdWriteFlashDataCallback(OFD_Status_t status)
{
  uint32_t maxOffset;

  if (OFD_STATUS_SUCCESS != status)
  {
    if (ofdAuxCallback)
      ofdAuxCallback(status);
    return;
  }
  if (OFD_POSITION_1 == ofdPosition)
    maxOffset = OFD_IMAGE1_START_ADDRESS + OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE;
  else
    maxOffset = OFD_IMAGE2_START_ADDRESS + OFD_MCU_FLASH_SIZE - OFD_MCU_EEPROM_SIZE;

  if (localAccessStructure.offset < maxOffset)
  { // save mcu flash
    ofdSaveCurrentFlashImage();
    return;
  }

  internalAccessParam.eepromOffset = 0;
  // save eeprom image
  ofdSaveCurrentEepromImage();
}

/**************************************************************************//**
\brief Starts saving internal flash.
******************************************************************************/
void ofdSaveCurrentFlashImage(void)
{
  ofdSetActionForBootloader();
}

/**************************************************************************//**
\brief Callback for saving internal eeprom.

\param[in] status - status of the data eeprom writing
******************************************************************************/
void ofdWriteEepromDataCallback(OFD_Status_t status)
{
  uint32_t maxOffset;

  if (OFD_STATUS_SUCCESS != status)
  {
    if (ofdAuxCallback)
      ofdAuxCallback(status);
    return;
  }
  if (OFD_POSITION_1 == ofdPosition)
    maxOffset = OFD_IMAGE1_START_ADDRESS + OFD_MCU_FLASH_SIZE;
  else
    maxOffset = OFD_IMAGE2_START_ADDRESS + OFD_MCU_FLASH_SIZE;

  if (localAccessStructure.offset < maxOffset)
  { // save mcu eeprom
    ofdSaveCurrentEepromImage();
    return;
  }

  // start check crc
  OFD_FlushAndCheckCrc(ofdPosition, internalAccessParam.data, ofdFlushCrcCallback);
}

/**************************************************************************//**
\brief Flushs memory buffer to flash.
******************************************************************************/
void ofdFlushData(void)
{}

/**************************************************************************//**
\brief Continues saving internal flash.
******************************************************************************/
static void ofdSaveCurrentEepromImageContinue(void)
{
 localAccessStructure.length = OFD_BLOCK_FOR_CHECK_CRC;
 localAccessStructure.data = internalAccessParam.data;
 if (0 == internalAccessParam.eepromOffset)
   memset(internalAccessParam.data, 0xFF, OFD_SERVICE_INFO_SIZE);

 internalAccessParam.eepromOffset += OFD_BLOCK_FOR_CHECK_CRC;
 internalAccessParam.length -= OFD_BLOCK_FOR_CHECK_CRC;
 ofdCallback = ofdWriteEepromDataCallback;
 ofdWriteData();
}

/**************************************************************************//**
\brief Callback for start of saving of action for bootloader.

\param[in]
   status - status of the crc saving to eeprom
\param[in]
   pInfo - ponter to image information
******************************************************************************/
static void ofdFlushCrcCallback(OFD_Status_t status, OFD_ImageInfo_t *pInfo)
{
  (void)pInfo;

  if (OFD_STATUS_SUCCESS != status)
  {
    if (ofdAuxCallback)
      ofdAuxCallback(status);
    return;
  }
  ofdSetActionForBootloader();
}

/**************************************************************************//**
\brief Reads image table.
******************************************************************************/
static void ofdReadImageTable(void)
{
  HAL_EepromParams_t params;

  params.address = OFD_SERVICE_INFO_SIZE - sizeof(OfdImageTable_t);
  params.data = &imageTable;
  params.length = sizeof(OfdImageTable_t);

  if (!ofdEepromHandler(HAL_ReadEeprom(&params, ofdReturnSuccessStatus), ofdReadImageTable))
  {
    sysAssert(ofdCallback, OFD_NULLCALLBACK4);
    ofdCallback(OFD_STATUS_INCORRECT_EEPROM_PARAMETER);
  }
}

/**************************************************************************//**
\brief Routine of eeprom access.
\param[in]
   result - result of hal eeprom action
\param[in]
   action - initiator action
\return
   false - incorrect parameters
   true  - eeprom transaction is started
******************************************************************************/
static bool ofdEepromHandler(int result, void(* action)())
{
  switch (result)
  {
    case EEPROM_OK:
      return true;
    case EEPROM_BUSY:
      delayedTransaction = action;
      ofdStartPollTimer(EEPROM_TRANSACTION);
      return true;
    default:
      return false;
  }
}

/**************************************************************************//**
\brief Starts saving internal eeprom.
******************************************************************************/
static void ofdSaveCurrentEepromImage(void)
{
  HAL_EepromParams_t params;

  params.address = internalAccessParam.eepromOffset;
  params.data = internalAccessParam.data;
  params.length = OFD_BLOCK_FOR_CHECK_CRC;

  if (!ofdEepromHandler(HAL_ReadEeprom(&params, ofdSaveCurrentEepromImageContinue), ofdSaveCurrentEepromImage))
    if (ofdAuxCallback)
      ofdAuxCallback(OFD_STATUS_INCORRECT_EEPROM_PARAMETER);
}

/**************************************************************************//**
\brief Sets action for internal bootloader.
******************************************************************************/
void ofdSetActionForBootloader(void)
{
  HAL_EepromParams_t params;

  params.address = 0;
  params.data = (uint8_t *)&actionSector;
  params.length = sizeof(OFD_Position_t);

  if (!ofdEepromHandler(HAL_WriteEeprom(&params, ofdPollBusyState), ofdSetActionForBootloader))
    if (ofdAuxCallback)
      ofdAuxCallback(OFD_STATUS_INCORRECT_EEPROM_PARAMETER);
}

/**************************************************************************//**
\brief Waits for end of image saving.
******************************************************************************/
static void ofdPollBusyState(void)
{
  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdPollBusyState;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  if (ofdAuxCallback)
    ofdAuxCallback(OFD_STATUS_SUCCESS);
}

/**************************************************************************//**
\brief Reads image crc from internal eeprom.
******************************************************************************/
void ofdReadCrc(void)
{
  HAL_EepromParams_t params;

  params.address = ofdPosition + 1;
  params.data = &imageInfo.crc;
  params.length = sizeof(uint8_t);

  if (!ofdEepromHandler(HAL_ReadEeprom(&params, ofdSaveCrcCallback), ofdReadCrc))
  {
    sysAssert(ofdCallback, OFD_NULLCALLBACK9);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // (EXTERNAL_MEMORY == MX25V4006E) && (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1
// eof ofdmx25v4006eDriver.c
