/**************************************************************************//**
\file  ofdAt45dbDriver.c

\brief Implementation of chip-flash interface.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    20/07/10 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if APP_USE_OTAU == 1
#if ((EXTERNAL_MEMORY == AT45DB041) || (EXTERNAL_MEMORY == AT45DB041E)) && (APP_USE_OTAU == 1)
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
#define ALL_DATA_HAS_BEEN_SAVED   0xFFFFFFFFul

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
static void ofdSaveCurrentEepromImage(void);
static void ofdFlushCrcCallback(OFD_Status_t status, OFD_ImageInfo_t *pInfo);
static void ofdSaveCurrentEepromImageContinue(void);
static void ofdPollBusyState(void);
static void ofdStartFlashDelayedTransaction(void);
static void ofdStartEepromDelayedTransaction(void);
static void ofdClearFlashInternalBuffer(void);
uint8_t ofdReadInternalFlash(uint32_t flashAddress);
#if (EXTERNAL_MEMORY == AT45DB041E)
static void ofdEraseChip(void);
#endif
static void ofdConfigPageSize(void);
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
#if (EXTERNAL_MEMORY == AT45DB041)
static uint16_t serviceGapData = 0;
#endif
static uint32_t flushedPageAddr = ALL_DATA_HAS_BEEN_SAVED;
#if defined(_OFD_DEBUG_)
static HAL_UsartDescriptor_t usartDescriptor;
static uint32_t debugOffset = 0ul;
static uint8_t  debugBuffer[OFD_BLOCK_SIZE];
#endif

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Checks flash state.

\return flash states: \n
  FLASH_BUSY  \n
  FLASH_READY
******************************************************************************/
static inline FlashBusyState_t ofdCheckBusyState(void)
{
  uint8_t statusReg = ofdReadStatusRegister();

  if (statusReg & RDY)
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
  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK10);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK11);
    ofdCallback(OFD_STATUS_INCORRECT_EEPROM_PARAMETER);
  }
}

/**************************************************************************//**
\brief Configuring page size.
******************************************************************************/
static void ofdConfigPageSize(void)
{
  uint32_t pageMode = 0xA6802A3D;
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&pageMode, sizeof(uint32_t));
  GPIO_EXT_MEM_CS_set(); 
}

#if (EXTERNAL_MEMORY == AT45DB041E)
/**************************************************************************//**
\brief Starts entire external memory erasing.
******************************************************************************/
static void ofdEraseChip(void)
{
  uint32_t eraseChip = CHIP_ERASE;

  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&eraseChip, sizeof(uint32_t));
  GPIO_EXT_MEM_CS_set();
}
#endif

/**************************************************************************//**
\brief Reads status register and check unchanged bits.
******************************************************************************/
void ofdFindStorageSpace(void)
{
#if EXTERNAL_MEMORY == AT45DB041E
  uint64_t manufacId = RDID;
  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdFindStorageSpace;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  GPIO_EXT_MEM_CS_clr();
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

#else
  if (STATUS_UNCHANGED_BITS == (ofdReadStatusRegister() & STATUS_UNCHANGED_BIT_MASK))
  {
    // read image table to global variable
    ofdReadImageTable();
    return;
  }
#endif

  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK12);
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
\brief Starts physical block(8 pages) erasing in the external memory.

\param[in]
  blockNumber - address of the erased block
******************************************************************************/
void ofdEraseBlock(uint8_t blockNumber)
{
  uint32_t erasedBlock = OFD_LITTLE_TO_BIG_ENDIAN((uint32_t)blockNumber << 4);

  erasedBlock >>= 8;
  erasedBlock |= BLOCK_ERASE;

  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&erasedBlock, sizeof(uint32_t));
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

#if (EXTERNAL_MEMORY == AT45DB041E)
  ofdEraseChip();
  ofdReturnSuccessStatus();
#else
  if (OFD_POSITION_1 == ofdPosition)
    ofdEraseBlock(FIRST_HALF_BLOCK_NUMBER + serviceGapData++);
  else
    ofdEraseBlock(SECOND_HALF_BLOCK_NUMBER + serviceGapData++);

  if (serviceGapData < OFD_USED_BLOCKS_AMOUNT)
  {
    delayedTransaction = ofdEraseImage;
    ofdStartPollTimer(FLASH_TRANSACTION);
  }
  else
  {
    serviceGapData = 0;
    ofdReturnSuccessStatus();
  }
#endif
}

/**************************************************************************//**
\brief Writes data to the external memory.
******************************************************************************/
void ofdWriteData(void)
{
  uint8_t *dataPointer;
  uint16_t dataLength;
  uint32_t address;
  uint32_t pageAddr;
  uint32_t byteAddr;
  static bool pageSizeConfigured = false;

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

  pageAddr = localAccessStructure.offset / PAGE_SIZE;

  if ((pageAddr > flushedPageAddr) && (ALL_DATA_HAS_BEEN_SAVED != flushedPageAddr))
  { // there is gap in the image address map.
    ofdFlushData();
    delayedTransaction = ofdClearFlashInternalBuffer;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  if (!pageSizeConfigured)
  {
    // To configure page size(256 bytes) after chip erase
    ofdConfigPageSize();
    pageSizeConfigured = true;
  }
  flushedPageAddr = pageAddr;
  byteAddr = localAccessStructure.offset % PAGE_SIZE;
  dataPointer = localAccessStructure.data;

  if ((byteAddr + localAccessStructure.length) > PAGE_SIZE)
    dataLength = PAGE_SIZE - byteAddr;
  else
    dataLength = localAccessStructure.length;

  localAccessStructure.data += dataLength;
  localAccessStructure.offset += dataLength;
  localAccessStructure.length -= dataLength;

  address = OFD_LITTLE_TO_BIG_ENDIAN(byteAddr);
  address |= WRITE_BUF1;

  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
  HAL_WriteSpi(&spiDesc, dataPointer, dataLength);
  GPIO_EXT_MEM_CS_set();

  if (PAGE_SIZE == (byteAddr + dataLength))
  {
    ofdFlushData();
    delayedTransaction = ofdClearFlashInternalBuffer;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  ofdReturnSuccessStatus();
}

/**************************************************************************//**
\brief Calls callback about end of eeprom saving.
******************************************************************************/
static void ofdSaveCrcCallback(void)
{
  SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK13);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK14);
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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK15);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

/**************************************************************************//**
\brief Clears internal flash buffer.
******************************************************************************/
static void ofdClearFlashInternalBuffer(void)
{
  uint32_t address = 0ul;
  uint64_t data;
  uint8_t itr;

  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdClearFlashInternalBuffer;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  memset((uint8_t *)&data, 0xFF, sizeof(uint64_t));

  address |= WRITE_BUF1;
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
  for (itr = 0; itr < (PAGE_SIZE / sizeof(uint64_t)); itr++)
     HAL_WriteSpi(&spiDesc, (uint8_t *)&data, sizeof(uint64_t));
  GPIO_EXT_MEM_CS_set();

  ofdWriteData();
}

/**************************************************************************//**
\brief Flushs memory buffer to flash.
******************************************************************************/
void ofdFlushData(void)
{
  if (ALL_DATA_HAS_BEEN_SAVED == flushedPageAddr)
    return;

  flushedPageAddr = OFD_LITTLE_TO_BIG_ENDIAN(flushedPageAddr);
  flushedPageAddr >>= 8;
  flushedPageAddr |= PROGRAM_BUF1;
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&flushedPageAddr, sizeof(uint32_t));
  GPIO_EXT_MEM_CS_set();
  flushedPageAddr = ALL_DATA_HAS_BEEN_SAVED;
}

/**************************************************************************//**
\brief Checks image crc.
******************************************************************************/
void ofdCheckCrc(void)
{
  uint32_t address;
  uint32_t pageAddr;
  uint32_t byteAddr;
  uint8_t atomicCounting = ATOMIC_COUNTING;

  if (FLASH_BUSY == ofdCheckBusyState())
  { // waits till flash ready
    delayedTransaction = ofdCheckCrc;
    ofdStartPollTimer(FLASH_TRANSACTION);
    return;
  }

  while (atomicCounting--)
  {
    pageAddr = localAccessStructure.offset / PAGE_SIZE;
    byteAddr = localAccessStructure.offset % PAGE_SIZE;
    address = byteAddr | (pageAddr << 8) | ((uint32_t)READ << 24);
    address = OFD_LITTLE_TO_BIG_ENDIAN(address);
    GPIO_EXT_MEM_CS_clr();
    HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
    // load 32 don't care bits
    HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
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
    maxOffset = OFD_IMAGE1_START_ADDRESS + OFD_EEPROM_OFFSET_WITHIN_IMAGE;
  else
    maxOffset = OFD_IMAGE2_START_ADDRESS + OFD_EEPROM_OFFSET_WITHIN_IMAGE;

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
#if EXTERNAL_MEMORY == AT45DB041
  uint16_t itr;

  localAccessStructure.length = OFD_BLOCK_FOR_CHECK_CRC;
  localAccessStructure.data = internalAccessParam.data;
  for (itr = 0; itr < OFD_BLOCK_FOR_CHECK_CRC; itr++)
  {
    OFD_LOAD_NO_COMMAND_TO_NVM;
    internalAccessParam.data[itr] = ofdReadInternalFlash(internalAccessParam.flashOffset++);
  }
  internalAccessParam.length -= OFD_BLOCK_FOR_CHECK_CRC;
  ofdCallback = ofdWriteFlashDataCallback;
  ofdWriteData();
#elif EXTERNAL_MEMORY == AT45DB041E
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
    maxOffset = OFD_IMAGE1_START_ADDRESS + OFD_IMAGE_SIZE;
  else
    maxOffset = OFD_IMAGE2_START_ADDRESS + OFD_IMAGE_SIZE;

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
    SYS_E_ASSERT_FATAL(ofdCallback, OFD_NULLCALLBACK16);
    ((OFD_InfoCallback_t)ofdCallback)(OFD_STATUS_INCORRECT_EEPROM_PARAMETER, &imageInfo);
  }
}

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
#if defined(ATMEGA1281) || defined(ATMEGA128RFA1)
  usartDescriptor.tty            = USART_CHANNEL_1;
#elif defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
  usartDescriptor.tty            = USART_CHANNEL_F0;
#endif
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
  uint32_t pageAddr;
  uint32_t byteAddr;


  while (FLASH_BUSY == ofdCheckBusyState())
  {;}// waits till flash ready
  pageAddr = address / PAGE_SIZE;
  byteAddr = address % PAGE_SIZE;
  address = byteAddr | (pageAddr << 8) | ((uint32_t)READ << 24);
  address = OFD_LITTLE_TO_BIG_ENDIAN(address);
  GPIO_EXT_MEM_CS_clr();
  HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
  // load 32 don't care bits
  HAL_WriteSpi(&spiDesc, (uint8_t *)&address, sizeof(uint32_t));
  HAL_ReadSpi(&spiDesc, data, size);
  GPIO_EXT_MEM_CS_set();
  if (cb)
    cb(OFD_STATUS_SUCCESS);
}
#endif // defined(_OFD_DEBUG_)

#endif // APP_USE_FAKE_OFD_DRIVER == 0
#endif // (EXTERNAL_MEMORY == AT45DB041) && (APP_USE_OTAU == 1)
#endif // APP_USE_OTAU == 1
// eof ofdAt25fDriver.c
