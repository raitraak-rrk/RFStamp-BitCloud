/**************************************************************************//**
\file  ofdAt25fDriver.c

\brief Implementation of chip-flash interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    5/12/07 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if ((EXTERNAL_MEMORY == AT25F2048) ||(EXTERNAL_MEMORY == AT25DF041A)) && (APP_USE_OTAU == 1)
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
#include <sysAssert.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define SPI_BUSY_POLL_PERIOD      10
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
#if EXTERNAL_MEMORY  == AT25F2048
  static void ofdEraseSector(uint32_t sectorAddress);
  static void ofdContinueEraseImage(void);
#elif EXTERNAL_MEMORY == AT25DF041A
  static void ofdEraseChip(void);
#endif
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

  if (!(statusReg & RDY))
    return FLASH_READY;

  return FLASH_BUSY;
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
\brief Returns SUCCESS status.
******************************************************************************/
static void ofdReturnSuccessStatus(void)
{
  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK3);
  ofdCallback(OFD_STATUS_SUCCESS);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK4);
    ofdCallback(OFD_STATUS_INCORRECT_EEPROM_PARAMETER);
  }
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
#if EXTERNAL_MEMORY  == AT25F2048
  HAL_ReadSpi(&spiDesc, (uint8_t *)&manufacId, sizeof(uint32_t)-1);
#elif EXTERNAL_MEMORY == AT25DF041A
  HAL_ReadSpi(&spiDesc, (uint8_t *)&manufacId, sizeof(uint64_t)-3);
#endif
  GPIO_EXT_MEM_CS_set();

  if (OFD_MANUFACTURER_ID == (uint8_t)(manufacId >> 8))
  {
#if EXTERNAL_MEMORY == AT25F2048
    if (OFD_DEVICE_ID == (uint8_t)(manufacId >> 16))
#elif EXTERNAL_MEMORY == AT25DF041A
    if ((OFD_DEVICE_ID_1 == (uint8_t)(manufacId >> 16)) &&
        (OFD_DEVICE_ID_2 == (uint8_t)(manufacId >> 24)) &&
        (EXT_STRING_LENGTH == (uint8_t)(manufacId >> 32)))
#endif
    {
      // read image table to global variable
      ofdReadImageTable();
      return;
    }
  }

  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK5);
  ofdCallback(OFD_STATUS_UNKNOWN_EXTERNAL_FLASH_TYPE);
}

/**************************************************************************//**
\brief Reads status register from the external flash.

\return status register
******************************************************************************/
static inline uint8_t ofdReadStatusRegister(void)
{
  uint16_t regStatus = RDSR;

  GPIO_EXT_MEM_CS_clr();
  HAL_ReadSpi(&spiDesc, (uint8_t *)&regStatus, sizeof(uint16_t));
  GPIO_EXT_MEM_CS_set();

  return (uint8_t)(regStatus >> 8);
}

/**************************************************************************//**
\brief Sends "write enable" command to the external flash.
******************************************************************************/
void ofdSendWriteEnable(void)
{
  uint8_t wren = WREN;

  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, &wren, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();
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

#if EXTERNAL_MEMORY == AT25F2048
  if (OFD_POSITION_1 == ofdPosition)
    ofdEraseSector(SECTOR_ONE);
  else
    ofdEraseSector(SECTOR_THREE);

  delayedTransaction = ofdContinueEraseImage;
  ofdStartPollTimer(FLASH_TRANSACTION);
#elif EXTERNAL_MEMORY == AT25DF041A
  ofdEraseChip();
  ofdReturnSuccessStatus();
#endif
}

#if EXTERNAL_MEMORY == AT25F2048
/**************************************************************************//**
\brief Starts physical sector erasing in the external memory.

\param[in]
  sectorAddress - address from erased sector
******************************************************************************/
static void ofdEraseSector(uint32_t sectorAddress)
{
  uint32_t erasedSector = sectorAddress | ((uint32_t)SECTOR_ERASE << 24);

  ofdSendWriteEnable();
  erasedSector = OFD_LITTLE_TO_BIG_ENDIAN(erasedSector);
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&erasedSector, sizeof(uint32_t));
  GPIO_EXT_MEM_CS_set();
}
/**************************************************************************//**
\brief Continues image erasing in the external memory.
******************************************************************************/
static void ofdContinueEraseImage(void)
{
  if (OFD_POSITION_1 == ofdPosition)
    ofdEraseSector(SECTOR_TWO);
  else
    ofdEraseSector(SECTOR_FOUR);

  ofdReturnSuccessStatus();
}
#elif EXTERNAL_MEMORY == AT25DF041A
/**************************************************************************//**
\brief Starts entire external memory erasing.
******************************************************************************/
static void ofdEraseChip(void)
{
  uint8_t eraseChip = CHIP_ERASE;

  ofdSendWriteEnable();
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&eraseChip, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();
}
#endif // EXTERNAL_MEMORY == AT25DF041A


/**************************************************************************//**
\brief Writes data to the external memory.
******************************************************************************/
void ofdWriteData(void)
{
  uint8_t *dataPointer;
  uint16_t dataLength;
  uint32_t dataAddress;
  uint8_t writeInstruc = PROGRAM;

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
  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK6);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK7);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK8);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

/**************************************************************************//**
\brief Flushs memory buffer to flash.
******************************************************************************/
void ofdFlushData(void)
{}

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
#if EXTERNAL_MEMORY == AT25F2048
  uint16_t itr;

  localAccessStructure.length = OFD_BLOCK_FOR_CHECK_CRC;
  localAccessStructure.data = internalAccessParam.data;
  for (itr = 0; itr < OFD_BLOCK_FOR_CHECK_CRC; itr++)
    internalAccessParam.data[itr] = ofdReadInternalFlash(internalAccessParam.flashOffset++);
  internalAccessParam.length -= OFD_BLOCK_FOR_CHECK_CRC;
  ofdCallback = ofdWriteFlashDataCallback;
  ofdWriteData();
#elif EXTERNAL_MEMORY == AT25DF041A
  ofdSetActionForBootloader();
#endif
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK9);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

#if EXTERNAL_MEMORY  == AT25DF041A
/**************************************************************************//**
\brief Unprotects memory sectors for writing and erasing.
******************************************************************************/
void ofdUnprotectMemorySectors(void)
{
  uint8_t unprotect = WRSR;
  uint8_t unprotectArg = GLOBAL_UNPROTECT;

  ofdSendWriteEnable();
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, &unprotect, sizeof(uint8_t));
  HAL_WriteSpi(&spiDesc, &unprotectArg, sizeof(uint8_t));
  GPIO_EXT_MEM_CS_set();
}
#endif

#if defined(_OFD_DEBUG_)
/**************************************************************************//**
\brief Flash read callback.
******************************************************************************/
void ofdReadConfirm(OFD_Status_t status)
{
  HAL_WriteUsart(&usartDescriptor, debugBuffer, OFD_BLOCK_SIZE);
}

/**************************************************************************//**
\brief Usart write callback.
******************************************************************************/
void usartWriteConfirm(void)
{
  debugOffset += OFD_BLOCK_SIZE;
  if (debugOffset < OFD_IMAGE_SIZE)
  {
    ofdReadData(debugOffset, debugBuffer, OFD_BLOCK_SIZE, ofdReadConfirm);
  }
}

/**************************************************************************//**
\brief Initialization of usart for consinsting of flash transmitting.
******************************************************************************/
void ofdInitDebugInterface(void)
{
  usartDescriptor.tty            = USART_CHANNEL_1;
  usartDescriptor.mode           = USART_MODE_ASYNC;
  usartDescriptor.flowControl    = USART_FLOW_CONTROL_NONE;
  usartDescriptor.baudrate       = USART_BAUDRATE_38400;
  usartDescriptor.dataLength     = USART_DATA8;
  usartDescriptor.parity         = USART_PARITY_NONE;
  usartDescriptor.stopbits       = USART_STOPBIT_1;
  usartDescriptor.rxBuffer       = NULL;
  usartDescriptor.rxBufferLength = 0;
  usartDescriptor.txBuffer       = NULL;
  usartDescriptor.txBufferLength = 0;
  usartDescriptor.rxCallback     = NULL;
  usartDescriptor.txCallback     = usartWriteConfirm;

  HAL_OpenUsart(&usartDescriptor);
  ofdReadData(debugOffset, debugBuffer, OFD_BLOCK_SIZE, ofdReadConfirm);
}

/**************************************************************************//**
\brief Reads data from the external memory.

\param[in]
  address - flash cell address
\param[out]
  data - pointer to memory buffer
\param[in]
  size - size of memory buffer
\param[in]
  cb - pointer to callback
******************************************************************************/
void ofdReadData(uint32_t address, uint8_t *data, uint16_t size, OFD_Callback_t cb)
{
  uint8_t writeInstruc = READ;

  while (FLASH_BUSY == ofdCheckBusyState())
  {;}// waits till flash ready

  address = OFD_LITTLE_TO_BIG_ENDIAN(address<<8);
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, &writeInstruc, sizeof(uint8_t));
  HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t)-1);
  HAL_ReadSpi(&spiDesc, data, size);
  GPIO_EXT_MEM_CS_set();
  if (cb)
    cb(OFD_STATUS_SUCCESS);
}
#endif // defined(_OFD_DEBUG_)

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // ((EXTERNAL_MEMORY == AT25F2048) ||(EXTERNAL_MEMORY == AT25DF041A)) && (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1
// eof ofdAt25fDriver.c
