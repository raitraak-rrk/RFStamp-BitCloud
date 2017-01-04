/****************************************************************************//**
  \files mmc.c

  \brief Implementation of mmc protocol.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    30/08/11 N. Fomin - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <mem.h>
#include <driversDbg.h>
#include <sysAssert.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// card voltage ranges
#define OCR_VDD_16_17          (1 <<  4)
#define OCR_VDD_17_18          (1 <<  5)
#define OCR_VDD_18_19          (1 <<  6)
#define OCR_VDD_19_20          (1 <<  7)
#define OCR_VDD_20_21          (1 <<  8)
#define OCR_VDD_21_22          (1 <<  9)
#define OCR_VDD_22_23          (1 << 10)
#define OCR_VDD_23_24          (1 << 11)
#define OCR_VDD_24_25          (1 << 12)
#define OCR_VDD_25_26          (1 << 13)
#define OCR_VDD_26_27          (1 << 14)
#define OCR_VDD_27_28          (1 << 15)
#define OCR_VDD_28_29          (1 << 16)
#define OCR_VDD_29_30          (1 << 17)
#define OCR_VDD_30_31          (1 << 18)
#define OCR_VDD_31_32          (1 << 19)
#define OCR_VDD_32_33          (1 << 20)
#define OCR_VDD_33_34          (1 << 21)
#define OCR_VDD_34_35          (1 << 22)
#define OCR_VDD_35_36          (1 << 23)
// R1 response statuses
#define STATUS_APP_CMD           (1UL << 5)
#define STATUS_SWITCH_ERROR      (1UL << 7)
#define STATUS_READY_FOR_DATA    (1UL << 8)
#define STATUS_IDLE              (0UL << 9)
#define STATUS_READY             (1UL << 9)
#define STATUS_IDENT             (2UL << 9)
#define STATUS_STBY              (3UL << 9)
#define STATUS_TRAN              (4UL << 9)
#define STATUS_DATA              (5UL << 9)
#define STATUS_RCV               (6UL << 9)
#define STATUS_PRG               (7UL << 9)
#define STATUS_DIS               (8UL << 9)
#define STATUS_STATE           (0xFUL << 9)
#define STATUS_ERASE_RESET       (1UL << 13)
#define STATUS_WP_ERASE_SKIP     (1UL << 15)
#define STATUS_CIDCSD_OVERWRITE  (1UL << 16)
#define STATUS_OVERRUN           (1UL << 17)
#define STATUS_UNERRUN           (1UL << 18)
#define STATUS_ERROR             (1UL << 19)
#define STATUS_CC_ERROR          (1UL << 20)
#define STATUS_CARD_ECC_FAILED   (1UL << 21)
#define STATUS_ILLEGAL_COMMAND   (1UL << 22)
#define STATUS_COM_CRC_ERROR     (1UL << 23)
#define STATUS_UN_LOCK_FAILED    (1UL << 24)
#define STATUS_CARD_IS_LOCKED    (1UL << 25)
#define STATUS_WP_VIOLATION      (1UL << 26)
#define STATUS_ERASE_PARAM       (1UL << 27)
#define STATUS_ERASE_SEQ_ERROR   (1UL << 28)
#define STATUS_BLOCK_LEN_ERROR   (1UL << 29)
#define STATUS_ADDRESS_MISALIGN  (1UL << 30)
#define STATUS_ADDR_OUT_OR_RANGE (1UL << 31)
// R1 response sets
#define ERROR_STATUS (STATUS_ADDR_OUT_OR_RANGE | STATUS_ADDRESS_MISALIGN | STATUS_BLOCK_LEN_ERROR | STATUS_ERASE_SEQ_ERROR | \
                      STATUS_ERASE_PARAM | STATUS_WP_VIOLATION | STATUS_CARD_IS_LOCKED | STATUS_UN_LOCK_FAILED | \
                      STATUS_COM_CRC_ERROR | STATUS_ILLEGAL_COMMAND | STATUS_CARD_ECC_FAILED | STATUS_CC_ERROR | STATUS_ERROR | \
                      STATUS_UNERRUN | STATUS_OVERRUN | STATUS_CIDCSD_OVERWRITE | STATUS_SWITCH_ERROR)
// command arguments
#define CMD0_IDLE_STATE_ARGUMENT 0
#define CMD1_ACCESS_MODE_SECTOR  (1UL << 30)
#define CMD1_HOST_VOLTAGE_RANGE  (OCR_VDD_27_28 | OCR_VDD_28_29 | OCR_VDD_29_30 | OCR_VDD_30_31 | \
                                  OCR_VDD_31_32 | OCR_VDD_32_33 | OCR_VDD_33_34 | OCR_VDD_34_35 | \
                                  OCR_VDD_35_36)
#define CMD2_ARGUMENT            0
#define CMD6_SET_4_LINE_BUS      0x03B70100
#define CMD6_SET_HS_MODE         0x03B90100
// relative card address
#define REVALITE_CARD_ADDRESS (1 << 16)
// command responses length
#define NO_RESPONSE_LENGTH 0
#define R1_RESPONSE_LENGTH 1
#define R2_RESPONSE_LENGTH 4
#define R3_RESPONSE_LENGTH 1
// retry amount of card initialization
#define RETRY_AMOUNT 10000
// memory frequency masks for CDS memory register
#define FREQ_UNITS_MASK        0x07
#define FREQ_MULTIPLIER_MASK   0x78
#define FREQ_MULTIPLIER_OFFSET 0x03
// csd register definitions
#define CSD_SPEC_VERS_FOUR 4

/******************************************************************************
                   External global variables section
******************************************************************************/
/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
typedef union PACK
{
  uint32_t reg;
  struct PACK
  {
    uint32_t reserved1        : 5;
    uint32_t appCmd           : 1;
    uint32_t urgentBkops      : 1;
    uint32_t switchError      : 1;
    uint32_t readyForData     : 1;
    uint32_t currentState     : 4;
    uint32_t eraseReset       : 1;
    uint32_t reserved2        : 1;
    uint32_t wpEraseSkip      : 1;
    uint32_t cidCsdOverwrite  : 1;
    uint32_t overrun          : 1;
    uint32_t underrun         : 1;
    uint32_t error            : 1;
    uint32_t ccEror           : 1;
    uint32_t cardEccFailed    : 1;
    uint32_t illegalCommand   : 1;
    uint32_t comCrcError      : 1;
    uint32_t lockUnlockFailed : 1;
    uint32_t cardIsLocked     : 1;
    uint32_t wpViolation      : 1;
    uint32_t eraseParam       : 1;
    uint32_t eraseSeqError    : 1;
    uint32_t blockLenError    : 1;
    uint32_t addressMisalign  : 1;
    uint32_t addressOutOfRange: 1;
  };
} MmcCardStatus_t;

typedef union PACK
{
  uint32_t reg;
  struct PACK
  {
    uint32_t reserverd1    : 7;
    uint32_t voltage170_195: 1;
    uint32_t voltage20_26  : 7;
    uint32_t voltage27_36  : 9;
    uint32_t reserved2     : 5;
    uint32_t accessMode    : 2;
    uint32_t busy          : 1;
  };
} MmcOcrRegister_t;

typedef struct PACK
{
  uint8_t tranSpeed;
  uint8_t nsac;
  uint8_t taac;
  uint8_t reserved3        : 2;
  uint8_t specVersion      : 4;
  uint8_t csdStructure     : 2;
  uint32_t cSizeDown       : 10;
  uint32_t reserved2       : 2;
  uint32_t dsrImp          : 1;
  uint32_t readBlkMisalign : 1;
  uint32_t writeBlkMisalign: 1;
  uint32_t readBlPartial   : 1;
  uint32_t readBlLen       : 4;
  uint32_t ccc             : 12;
  uint32_t wpGroupSize     : 5;
  uint32_t eraseGroupMult  : 5;
  uint32_t eraseGroupSize  : 5;
  uint32_t cSizeMult       : 3;
  uint32_t vddWCurrMax     : 3;
  uint32_t vddWCurrMin     : 3;
  uint32_t vddRCurrMax     : 3;
  uint32_t vddRCurrMin     : 3;
  uint32_t cSizeUp         : 2;
  uint8_t notUsed          : 1;
  uint8_t crc              : 7;
  uint8_t ecc              : 2;
  uint8_t fileFormat       : 2;
  uint8_t tmpWriteProtect  : 1;
  uint8_t permWriteProtect : 1;
  uint8_t copy             : 1;
  uint8_t fileFormatGroup  : 1;
  uint16_t contentPropApp  : 1;
  uint16_t reserved1       : 4;
  uint16_t writeBlPartial  : 1;
  uint16_t writeBlLen      : 4;
  uint16_t r2wFactor       : 3;
  uint16_t defaultEcc      : 2;
  uint16_t wpGroupEnable   : 1;
} MmcCSDRegister_t;

typedef struct PACK
{
  uint8_t  productNameHigh;
  uint8_t  applicationId;
  uint8_t  cardBGA    : 2;
  uint8_t  reserved1  : 6;
  uint8_t  manufacturerId;
  uint32_t productNameMiddle;
  uint16_t productSerialNumberHigh;
  uint8_t  productRevision;
  uint8_t  productNameLow;
  uint8_t  reserverd2 : 1;
  uint8_t  crc        : 7;
  uint8_t  manufacturingDate;
  uint16_t productSerialNumberLow;
} MmcCIDRegister_t;
END_PACK

/******************************************************************************
                   Constants section
******************************************************************************/
static const uint32_t mmcFrequencyUnits[8] = {100, 1000, 10000, 100000, 0, 0, 0, 0};
static const uint32_t mmcFrequencyMulptiplier[16] = {0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80};

/******************************************************************************
                   Prototypes section
******************************************************************************/
static bool memSendCommand(HAL_HsmciDescriptor_t *descriptor, uint32_t command, uint32_t argument, uint8_t respLength);
static bool memCheckState(HAL_HsmciDescriptor_t *descriptor, uint16_t state);
static bool setMaximumCommunicationFrequency(HAL_HsmciDescriptor_t *descriptor, uint8_t freqCode, bool hsMode);
static MemStatus_t setBusWidthAndHsMode(HAL_HsmciDescriptor_t *descriptor, MmcCSDRegister_t *csdReg);

/******************************************************************************
                   Global variables section
******************************************************************************/
static uint32_t productSerialNumber;
static void (*absMemoryCallback)(MemStatus_t) = NULL;
static HAL_HsmciDescriptor_t *memDescriptor = NULL;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief HSMCI write callback.
******************************************************************************/
void memWriteCallback(void)
{
  MmcCardStatus_t *cardStatus;

  cardStatus = (MmcCardStatus_t *)(memDescriptor->commandDescriptor->responseBuffer);
  SYS_E_ASSERT_FATAL(absMemoryCallback, MSD_NULLCALLBACK6);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    absMemoryCallback(hsmciWriteError);
  absMemoryCallback(memSuccess);
}

/**************************************************************************//**
\brief HSMCI read callback.
******************************************************************************/
void memReadCallback(void)
{
  MmcCardStatus_t *cardStatus;

  cardStatus = (MmcCardStatus_t *)(memDescriptor->commandDescriptor->responseBuffer);
  SYS_E_ASSERT_FATAL(absMemoryCallback, MSD_NULLCALLBACK7);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    absMemoryCallback(hsmciReadError);
  absMemoryCallback(memSuccess);
}
/**************************************************************************//**
\brief Performs MMC memory initialization.
\param[in]
  commandDescr - pointer to hsmci command descriptor.
\return
  status of initialization procedure.
******************************************************************************/
MemStatus_t memInit(HAL_HsmciDescriptor_t *descriptor)
{
  uint16_t i = RETRY_AMOUNT;
  MmcCardStatus_t *cardStatus;
  MmcOcrRegister_t *ocrRegister;
  MmcCSDRegister_t *csdReg;
  HAL_HsmciCommandDescriptor_t *commandDescr  = descriptor->commandDescriptor;
  uint32_t sendOpCondResponse;
  MemStatus_t status;
  MmcCIDRegister_t *cidRegister;

  /* send first command - CMD0 with power init parameters */
  if (!memSendCommand(descriptor, POWER_ON_INIT, CMD0_IDLE_STATE_ARGUMENT, NO_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  /* send CMD0 command to go to idle mode */
  if (!memSendCommand(descriptor, GO_IDLE_STATE, CMD0_IDLE_STATE_ARGUMENT, NO_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  /* wait for internal initialization process */
  for (; i > 0; i--)
  {
    /* send CMD1 command to check host and device voltage correlation */
    if (!memSendCommand(descriptor, SEND_OP_COND, CMD1_HOST_VOLTAGE_RANGE | CMD1_ACCESS_MODE_SECTOR, R3_RESPONSE_LENGTH))
      return hsmciError;
    /* wait for hsmci bus becomes free */
    while (!HAL_HsmciCheckReady());
    ocrRegister = (MmcOcrRegister_t *)(commandDescr->responseBuffer);
    if (!(ocrRegister->reg & CMD1_HOST_VOLTAGE_RANGE))
      return voltageError;
    if (0 == ocrRegister->busy)
      continue;
    break;
  }
  if (0 == i)
    return initTimeoutError;
  sendOpCondResponse = commandDescr->responseBuffer[0];
  i = RETRY_AMOUNT;
  while (i--)
  {
    /* send CMD2 command to get card CID number */
    if (!memSendCommand(descriptor, ALL_SEND_CID, CMD2_ARGUMENT, R2_RESPONSE_LENGTH))
      return hsmciError;
    /* wait for hsmci bus becomes free */
    while (!HAL_HsmciCheckReady());
    if (sendOpCondResponse != commandDescr->responseBuffer[0])
      break;
  }
  if (0 == i)
    return stateError;
  cidRegister = (MmcCIDRegister_t *)(commandDescr->responseBuffer);
  productSerialNumber = cidRegister->productSerialNumberLow + ((cidRegister->productSerialNumberHigh) << 16);
  /* send CMD3 command to set card relative address */
  if (!memSendCommand(descriptor, SEND_RELATIVE_ADDR, REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_STBY))
    return stateError;
  /* send CMD9 command to read CSD register */
  if (!memSendCommand(descriptor, SEND_CSD, REVALITE_CARD_ADDRESS, R2_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  /* read allowed maximum communication frequency and set it */
  csdReg = (MmcCSDRegister_t *)(commandDescr->responseBuffer);
  status = setBusWidthAndHsMode(descriptor, csdReg);
  if (status != memSuccess)
    return setMaxFreqError;

  return memSuccess;
}

/**************************************************************************//**
\brief Reads MMC memory capacity.
\param[in]
  descriptor - pointer to hsmci descriptor.
\param[out]
  capInfo - pointer to memory capacity structure.
\return
  status of read capacity procedure.
******************************************************************************/
MemStatus_t memGetCapacityInfo(HAL_HsmciDescriptor_t *descriptor, MemCapacityInfo_t *capInfo)
{
  MmcCardStatus_t *cardStatus;
  MmcCSDRegister_t *csdReg;
  HAL_HsmciCommandDescriptor_t *commandDescr  = descriptor->commandDescriptor;

  /* send CMD13 command to discover current state */
  if (!memSendCommand(descriptor, SEND_STATUS, REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());

  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (STATUS_TRAN == (cardStatus->reg & STATUS_TRAN))
  {
    /* send CMD7 command to switch to stand-by state*/
    if (!memSendCommand(descriptor, SELECT_CARD, ~REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
      return hsmciError;
    /* wait for hsmci bus becomes free */
    while (!HAL_HsmciCheckReady());
    cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
    if (0 != (cardStatus->reg & ERROR_STATUS))
      return commandError;
    if (!memCheckState(descriptor, STATUS_STBY))
      return stateError;
  }
  /* send CMD9 command to read CSD register */
  if (!memSendCommand(descriptor, SEND_CSD, REVALITE_CARD_ADDRESS, R2_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());

  capInfo->logicalBlockLength = STANDARD_BLOCK_LENGTH;
  csdReg = (MmcCSDRegister_t *)(commandDescr->responseBuffer);
  capInfo->lastLogicalBlock = (1 << (csdReg->cSizeMult + 2)) * (csdReg->cSizeUp + 1 + (csdReg->cSizeDown << 2)) * ((1 << csdReg->readBlLen) / STANDARD_BLOCK_LENGTH)  - 1;

  if (!memCheckState(descriptor, STATUS_STBY))
    return stateError;

  /* send CMD7 command to switch to transfer state */
  if (!memSendCommand(descriptor, SELECT_CARD, REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  return memSuccess;
}

/**************************************************************************//**
\brief Checks MMC memory state.
\param[in]
  descriptor - pointer to hsmci descriptor;
  state - state in which MMC memory meant to be.
\return
  false - check failed;
  true - check success.
******************************************************************************/
static bool memSendCommand(HAL_HsmciDescriptor_t *descriptor, uint32_t command, uint32_t argument, uint8_t respLength)
{
  descriptor->commandDescriptor->command = command;
  descriptor->commandDescriptor->argument = argument;
  descriptor->commandDescriptor->responseLength = respLength;

  if (-1 == HAL_WriteHsmciCommand(descriptor))
    return false;

  return true;
}

/**************************************************************************//**
\brief Sends MMC command.
\param[in]
  descriptor - pointer to hsmci descriptor;
  command - command index and attributes;
  argument - command argument;
  respLength - command response length.
\return
  false - command sent failed;
  true - command sent success.
******************************************************************************/
static bool memCheckState(HAL_HsmciDescriptor_t *descriptor, uint16_t state)
{
  MmcCardStatus_t *cardStatus;
  /* send CMD13 command to discover current state */
  if (!memSendCommand(descriptor, SEND_STATUS, REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(descriptor->commandDescriptor->responseBuffer);

  return (cardStatus->reg & STATUS_STATE) == state;
}

/**************************************************************************//**
\brief Writes one data block to MMC memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of block to write.
\return
  status of write procedure.
******************************************************************************/
MemStatus_t memWriteBlock(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemStatus_t))
{
  HAL_HsmciCommandDescriptor_t *commandDescr = descriptor->commandDescriptor;
  HAL_HsmciDataTransferDescriptor_t *dataDescr = descriptor->dataTransferDescriptor;

  memDescriptor = descriptor;

  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  commandDescr->command = WRITE_BLOCK;
  commandDescr->argument = address << 9;
  commandDescr->responseLength = R1_RESPONSE_LENGTH;

  dataDescr->direction = HSMCI_WRITE;
  dataDescr->blockSize = STANDARD_BLOCK_LENGTH;
  absMemoryCallback = callback;
  descriptor->callback = memWriteCallback;

  if (-1 == HAL_WriteHsmci(descriptor))
    return hsmciWriteError;

  return memSuccess;
}

/**************************************************************************//**
\brief Reads one data block from MMC  memory at "address".
\param[in]
  descriptor - pointer to hsmci descriptor;
  address - address of block to read.
\return
  status of read procedure.
******************************************************************************/
MemStatus_t memReadBlock(HAL_HsmciDescriptor_t *descriptor, uint32_t address, void (*callback)(MemStatus_t))
{
  HAL_HsmciCommandDescriptor_t *commandDescr = descriptor->commandDescriptor;
  HAL_HsmciDataTransferDescriptor_t *dataDescr = descriptor->dataTransferDescriptor;

  memDescriptor = descriptor;

  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  commandDescr->command = READ_SINGLE_BLOCK;
  commandDescr->argument = address << 9;
  commandDescr->responseLength = R1_RESPONSE_LENGTH;

  dataDescr->direction = HSMCI_READ;
  dataDescr->blockSize = STANDARD_BLOCK_LENGTH;
  absMemoryCallback = callback;
  descriptor->callback = memReadCallback;

  if (-1 == HAL_ReadHsmci(descriptor))
    return hsmciReadError;

  return memSuccess;
}

/**************************************************************************//**
\brief Checks if MMC memory is ready for any data transfer.
\return
  false - memory is busy;
  true - memory is ready.
******************************************************************************/
bool memIsBusy(void)
{
  return HAL_HsmciCheckReady();
}

/**************************************************************************//**
\brief Sets maximum bus frequency for MMC memory.
\param[in]
  descriptor - pointer to hsmci descriptor;
  freqCode - frequency code.
\return
  false - setting failed;
  true - setting success.
******************************************************************************/
static bool setMaximumCommunicationFrequency(HAL_HsmciDescriptor_t *descriptor, uint8_t freqCode, bool hsMode)
{
  uint32_t freqUnit = mmcFrequencyUnits[freqCode & FREQ_UNITS_MASK];
  uint32_t freqMult = mmcFrequencyMulptiplier[(freqCode & FREQ_MULTIPLIER_MASK) >> FREQ_MULTIPLIER_OFFSET];

  descriptor->clockRate = freqUnit * freqMult * 100;
  if (hsMode)
    descriptor->clockRate *= 2;
  if (-1 == HAL_SetHsmciSpeed(descriptor))
    return false;

  return true;
}

/**************************************************************************//**
\brief Sets maximum 4-line dat bus and high speed mode if card supports it.
\param[in]
  descriptor - pointer to hsmci descriptor;
  csdReg - pointer to csd register structure.
\return
  status of operation.
******************************************************************************/
static MemStatus_t setBusWidthAndHsMode(HAL_HsmciDescriptor_t *descriptor, MmcCSDRegister_t *csdReg)
{
  MmcCardStatus_t *cardStatus;
  uint8_t specVersion = csdReg->specVersion;
  uint8_t tranSpeed = csdReg->tranSpeed;
  HAL_HsmciCommandDescriptor_t *commandDescr = descriptor->commandDescriptor;

  /* send CMD7 command to switch to transfer state */
  if (!memSendCommand(descriptor, SELECT_CARD, REVALITE_CARD_ADDRESS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

   /* send CMD16 command to set card block length */
  if (!memSendCommand(descriptor, SET_BLOCKLEN, STANDARD_BLOCK_LENGTH, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  if (specVersion < CSD_SPEC_VERS_FOUR)
  {
    if (!setMaximumCommunicationFrequency(descriptor, tranSpeed, false))
      return hsmciError;
    else
      return memSuccess;
  }

   /* send CMD6 command to switch to 4-line bus */
  if (!memSendCommand(descriptor, SWITCH, CMD6_SET_4_LINE_BUS, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  descriptor->busWidth = HSMCI_BUS_WIDTH_4;
  if (-1 == HAL_SetHsmciBusWidth(descriptor))
    return hsmciError;

  /* send CMD6 command to switch to high speed mode */
  if (!memSendCommand(descriptor, SWITCH, CMD6_SET_HS_MODE, R1_RESPONSE_LENGTH))
    return hsmciError;
  /* wait for hsmci bus becomes free */
  while (!HAL_HsmciCheckReady());
  cardStatus = (MmcCardStatus_t *)(commandDescr->responseBuffer);
  if (0 != (cardStatus->reg & ERROR_STATUS))
    return commandError;
  if (!memCheckState(descriptor, STATUS_TRAN))
    return stateError;

  descriptor->highSpeedMode = true;
  if (-1 == HAL_SetHsmciHighSpeedMode(descriptor))
    return hsmciError;

  if (!setMaximumCommunicationFrequency(descriptor, tranSpeed, false))
    return hsmciError;

  return memSuccess;
}

uint32_t memCardSerialNumber(void)
{
  return productSerialNumber;
}

//eof mmc.c
