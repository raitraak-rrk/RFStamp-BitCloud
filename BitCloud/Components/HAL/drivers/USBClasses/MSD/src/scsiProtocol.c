/****************************************************************************//**
  \files scsiProtocol.h

  \brief Implementation of communication device protocol command.

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
#include <scsiProtocol.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// scsi commands
#define INQUIRY                      0x12
#define READ_CAPACITY_10             0x25
#define READ_10                      0x28
#define REQUEST_SENSE                0x03
#define TEST_UNIT_READY              0x00
#define WRITE_10                     0x2A
#define PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#define MODE_SENSE_6                 0x1A
#define READ_FORMAT_CAPACITIES       0x23
#define VERIFY_10                    0x2F
#define START_STOP_UNIT              0x1B

// capacity definitions
#define BLOCK_SIZE        512

//inquiry definitions
#define INQUIRY_PERIPHERAL        0x00
#define INQUIRY_REMOVABLE         0x80
#define INQUIRY_VERSION           0x04
#define INQUIRY_RESPONSE_FORMAT   0x02
#define INQUIRY_ADDITIONAL_LENGTH 0x20
#define INQUIRY_FLAGS             0x00
#define INQUIRY_VENDOR_ID         "Atmel   "
#define INQUIRY_PRODUCT_ID        "Mass Storage    "
#define INQUIRY_PRODUCT_REVISION  "0001"

// request sense definitions
#define REQUEST_SENSE_CODE                            0xF0
#define REQUEST_SENSE_CODE_NOT_VALID                  0x70
#define REQUEST_SENSE_OBSOLETE                        0x00
#define REQUEST_SENSE_NORMAL_FLAGS                    0x00
#define REQUEST_SENSE_ERROR_FLAGS                     0x05
#define REQUEST_SENSE_EJECTED_FLAGS                   0x02
#define REQUEST_SENSE_INFO                            0x00
#define REQUEST_SENSE_ADDITIONAL_SENSE_LENGTH         0x0A
#define REQUEST_SENSE_CMD_SPECIFIC_INFO               0x00
#define REQUEST_SENSE_NORMAL_ADDITIONAL_SENSE_CODE    0x00
#define REQUEST_SENSE_ERROR_ADDITIONAL_SENSE_CODE     0x20
#define REQUEST_SENSE_EJECTED_ADDITIONAL_SENSE_CODE   0x3A
#define REQUEST_SENSE_ADDITIONAL_SENSE_CODE_QUALIFIER 0x00
#define REQUEST_SENSE_FIELD_REPLACABLE_UNIT_CODE      0x00
#define REQUEST_SENSE_SENSE_KEY_SPECIFIC_VALID        0x80
#define REQUEST_SENSE_SENSE_KEY_SPECIFIC              0x00

// mode sense definitions
#define MODE_SENSE_LENGTH            0x03
#define MODE_SENSE_MEDIA_TYPE        0x00
#define MODE_SENSE_DEVICE_SPECIFIC   0x00
#define MODE_SENSE_BLOCK_DESC_LENGTH 0x00

// format capacities definitions
#define FORMAT_CAPACITIES_RESERVED  0x00
#define FORMAT_CAPACITIES_LENGTH    0x08
#define FORMAT_CAPACITIES_DESC_TYPE 0x01

/******************************************************************************
                   Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  uint32_t lastLogicalBlock;
  uint32_t logicalBlockLength;
} ScsiReadCapacityResponse_t;

typedef struct PACK
{
  uint8_t peripheral;
  uint8_t removable;
  uint8_t version;
  uint8_t responseDataFormat;
  uint8_t additionalLength;
  uint8_t flags[3];
  uint8_t vendorId[8];
  uint8_t productId[16];
  uint8_t productRevision[4];
} ScsiInquiryResponse_t;

typedef struct PACK
{
  uint8_t  responseCodeAndValid;
  uint8_t  obsolete;
  uint8_t  flags;
  uint32_t information;
  uint8_t  addSenseLen;
  uint32_t cmdSpecificInfo;
  uint8_t  additionalSenseCode;
  uint8_t  additionalSenseCodeQualifier;
  uint8_t  fieldReplacableUnitCode;
  uint8_t  senseKeySpecific[3];
} ScsiRequestSenseResponse_t;

typedef struct PACK
{
  uint8_t modeDataLength;
  uint8_t mediumType;
  uint8_t deviceSpecific;
  uint8_t blockDescriptorLength;
} ScsiModeSense6Response_t;

typedef struct PACK
{
  uint8_t reserved[3];
  uint8_t length;
  uint32_t blocksNumber;
  uint8_t descType;
  uint8_t blockLengthUp;
  uint16_t blockLengthDown;
} ScsiReadFormatCapacities_t;

typedef struct PACK
{
  uint8_t flags;
  uint32_t logicalBlockAddress;
  uint8_t groupNumber;
  uint16_t transferLength;
  uint8_t control;
} ScsiReadWriteCommand_t;

typedef struct PACK
{
  uint8_t flags;
  uint8_t reserved;
  uint8_t powerConditionModifier;
  uint8_t powerCondition;
  uint8_t control;
} ScsiStartStopUnitCommand_t;

typedef struct PACK
{
  uint8_t index;
  union
  {
    ScsiReadWriteCommand_t rwCommand;
    ScsiStartStopUnitCommand_t startStopCommand;
  };
} ScsiCommand_t;
END_PACK

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void processStartStopUnitCommand(ScsiStartStopUnitCommand_t *command);
static uint8_t formInquiryResponse(uint8_t *buffer);
static uint8_t formReadCapacityResponse(uint8_t *buffer);
static uint8_t formFormatCapacityResponse(uint8_t *buffer);
static uint8_t formModeSenseResponse(uint8_t *buffer);
static uint8_t formRequestSenseResponse(uint8_t *buffer);

/******************************************************************************
                   Local variables section
******************************************************************************/
static bool defaultRequestSenseResponse;
static uint32_t lastLogicalBlockNum;
static bool isEjected = false;

/******************************************************************************
                   Constants section
******************************************************************************/
const ScsiInquiryResponse_t inquiryResponse = {
  INQUIRY_PERIPHERAL,
  INQUIRY_REMOVABLE,
  INQUIRY_VERSION,
  INQUIRY_RESPONSE_FORMAT,
  INQUIRY_ADDITIONAL_LENGTH,
  {INQUIRY_FLAGS, INQUIRY_FLAGS, INQUIRY_FLAGS},
  INQUIRY_VENDOR_ID,
  INQUIRY_PRODUCT_ID,
  INQUIRY_PRODUCT_REVISION
};

const ScsiModeSense6Response_t modeSense6Response = {
  MODE_SENSE_LENGTH,
  MODE_SENSE_MEDIA_TYPE,
  MODE_SENSE_DEVICE_SPECIFIC,
  MODE_SENSE_BLOCK_DESC_LENGTH
};

/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Checks if received scsi command is supported.
\param[in]
  data - pointer to received command with parameters.
\return
  false - command is not supported;
  true - command is supported.
******************************************************************************/
bool scsiIsValidCommand(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  command->rwCommand.transferLength = SWAP16(command->rwCommand.transferLength);
  command->rwCommand.logicalBlockAddress = SWAP32(command->rwCommand.logicalBlockAddress);

  if (isEjected && (TEST_UNIT_READY == command->index))
    return false;

  switch (command->index)
  {
    case INQUIRY:
    case READ_CAPACITY_10:
    case TEST_UNIT_READY:
    case PREVENT_ALLOW_MEDIUM_REMOVAL:
    case READ_10:
    case WRITE_10:
    case MODE_SENSE_6:
    case READ_FORMAT_CAPACITIES:
    case VERIFY_10:
      defaultRequestSenseResponse = true;
      break;
    case START_STOP_UNIT:
      processStartStopUnitCommand(&command->startStopCommand);
      break;
    case REQUEST_SENSE:
      return true;
    default:
      defaultRequestSenseResponse = false;
      break;
  }

  return defaultRequestSenseResponse;
}

/**************************************************************************//**
\brief Checks if any response or data transfer needed for received
       scsi command.
\param[in]
  command - received command.
\return
  false - response is not needed;
  true - response is needed.
******************************************************************************/
bool scsiIsDataInOutPhaseNeeded(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  if ((TEST_UNIT_READY == command->index) || (PREVENT_ALLOW_MEDIUM_REMOVAL == command->index)
      || (VERIFY_10 == command->index) || (START_STOP_UNIT == command->index))
    return false;
  if ((READ_10 == command->index) || (WRITE_10 == command->index))
    if (0 == command->rwCommand.transferLength)
      return false;
  return true;
}

/**************************************************************************//**
\brief Checks if command is read or write command.
\param[in]
  command - received command.
\return
  false - command is not read/write command;
  true - command is read/write command.
******************************************************************************/
bool scsiIsReadWriteCommand(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  return ((READ_10 == command->index) || (WRITE_10 == command->index));
}

/**************************************************************************//**
\brief Checks if command is read command.
\param[in]
  command - received command.
\return
  false - command is not read command;
  true - command is read command.
******************************************************************************/
bool scsiIsReadCommand(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  return (READ_10 == command->index);
}

/**************************************************************************//**
\brief Blocks for read/write command.
\param[in]
  data - pointer to received command with parameters.
\return
  number of blocks to read or write from memory.
******************************************************************************/
uint16_t scsiBlocksAmount(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  if ((READ_10 == command->index) || (WRITE_10 == command->index))
    return command->rwCommand.transferLength;

  return 0;
}

/**************************************************************************//**
\brief Response for scsi command.
\param[in]
  command - received command.
\param[out]
  buffer - buffer with scsi command response.
\return
  length of response
******************************************************************************/
uint8_t scsiGetCommandResponse(uint8_t *data, uint8_t *buffer)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  if (INQUIRY == command->index)
    return formInquiryResponse(buffer);

  if (READ_CAPACITY_10 == command->index)
    return formReadCapacityResponse(buffer);

  if (MODE_SENSE_6 == command->index)
    return formModeSenseResponse(buffer);

  if (READ_FORMAT_CAPACITIES == command->index)
    return formFormatCapacityResponse(buffer);

  if (REQUEST_SENSE == command->index)
    return formRequestSenseResponse(buffer);

  return 0;
}

/**************************************************************************//**
\brief Sets number of last available memory block for scsi response.
\param[in]
  lastBlock - received command.
\param[out]
  buffer - number of last available memory block
******************************************************************************/
void scsiSetCapacity(uint32_t lastBlock)
{
  lastLogicalBlockNum = lastBlock;
}

/**************************************************************************//**
\brief Block address for read/write command.
\param[in]
  data - pointer to received command with parameters.
\return
  block address for read or write from memory.
******************************************************************************/
uint32_t scsiGetBlockAddress(uint8_t *data)
{
  ScsiCommand_t *command = (ScsiCommand_t *)data;

  return command->rwCommand.logicalBlockAddress;
}

/**************************************************************************//**
\brief Processes START_STOP_UNIT command.
\param[in]
  command - pointer to received command with parameters.
******************************************************************************/
static void processStartStopUnitCommand(ScsiStartStopUnitCommand_t *command)
{
  defaultRequestSenseResponse = true;
  if ((0x02 == command->powerConditionModifier) && (0 == command->powerCondition))
    isEjected = true;
}

/**************************************************************************//**
\brief Forms response for INQUIRY command.
\param[out]
  buffer - buffer with scsi command response.
******************************************************************************/
static uint8_t formInquiryResponse(uint8_t *buffer)
{
  memcpy(buffer, (uint8_t *)&inquiryResponse, sizeof(ScsiInquiryResponse_t));
  return sizeof(ScsiInquiryResponse_t);
}

/**************************************************************************//**
\brief Forms response for READ_CAPACITY_10 command.
\param[out]
  buffer - buffer with scsi command response.
******************************************************************************/
static uint8_t formReadCapacityResponse(uint8_t *buffer)
{
  ScsiReadCapacityResponse_t readCapacityResponse = {
    .lastLogicalBlock = SWAP32(lastLogicalBlockNum),
    .logicalBlockLength = SWAP32(BLOCK_SIZE)
  };

  memcpy(buffer, (uint8_t *)&readCapacityResponse, sizeof(ScsiReadCapacityResponse_t));
  return sizeof(ScsiReadCapacityResponse_t);
}

/**************************************************************************//**
\brief Forms response for READ_FORMAT_CAPACITY command.
\param[out]
  buffer - buffer with scsi command response.
******************************************************************************/
static uint8_t formFormatCapacityResponse(uint8_t *buffer)
{
   ScsiReadFormatCapacities_t readFormatCapacitiesResponse = {
    .reserved = {FORMAT_CAPACITIES_RESERVED, FORMAT_CAPACITIES_RESERVED, FORMAT_CAPACITIES_RESERVED},
    .length = FORMAT_CAPACITIES_LENGTH,
    .blocksNumber = SWAP32(lastLogicalBlockNum + 1),
    .descType = FORMAT_CAPACITIES_DESC_TYPE,
    .blockLengthUp = 0,
    .blockLengthDown = SWAP16(BLOCK_SIZE)
  };

  memcpy(buffer, (uint8_t *)&readFormatCapacitiesResponse, sizeof(ScsiReadFormatCapacities_t));
  return sizeof(ScsiReadFormatCapacities_t);
}

/**************************************************************************//**
\brief Forms response for MODE_SENSE_6 command.
\param[out]
  buffer - buffer with scsi command response.
******************************************************************************/
static uint8_t formModeSenseResponse(uint8_t *buffer)
{
  memcpy(buffer, (uint8_t *)&modeSense6Response, sizeof(ScsiModeSense6Response_t));
  return sizeof(ScsiModeSense6Response_t);
}

/**************************************************************************//**
\brief Forms response for REQUEST_SENSE command.
\param[out]
  buffer - buffer with scsi command response.
******************************************************************************/
static uint8_t formRequestSenseResponse(uint8_t *buffer)
{
  ScsiRequestSenseResponse_t requestSenseResponse = {
    .responseCodeAndValid = REQUEST_SENSE_CODE_NOT_VALID,
    .obsolete = REQUEST_SENSE_OBSOLETE,
    .flags = REQUEST_SENSE_EJECTED_FLAGS,
    .information = SWAP32(REQUEST_SENSE_INFO),
    .addSenseLen = REQUEST_SENSE_ADDITIONAL_SENSE_LENGTH,
    .cmdSpecificInfo = SWAP32(REQUEST_SENSE_CMD_SPECIFIC_INFO),
    .additionalSenseCode = REQUEST_SENSE_EJECTED_ADDITIONAL_SENSE_CODE,
    .additionalSenseCodeQualifier = REQUEST_SENSE_ADDITIONAL_SENSE_CODE_QUALIFIER,
    .fieldReplacableUnitCode = REQUEST_SENSE_FIELD_REPLACABLE_UNIT_CODE,
    .senseKeySpecific = {REQUEST_SENSE_SENSE_KEY_SPECIFIC, REQUEST_SENSE_SENSE_KEY_SPECIFIC, REQUEST_SENSE_SENSE_KEY_SPECIFIC}
  };

  if (isEjected)
    memcpy(buffer, (uint8_t *)&requestSenseResponse, sizeof(ScsiRequestSenseResponse_t));
  else
  {
    requestSenseResponse.responseCodeAndValid = REQUEST_SENSE_CODE;
    requestSenseResponse.senseKeySpecific[0] = REQUEST_SENSE_SENSE_KEY_SPECIFIC_VALID;

    if (defaultRequestSenseResponse)
    {
      requestSenseResponse.flags = REQUEST_SENSE_NORMAL_FLAGS;
      requestSenseResponse.additionalSenseCode = REQUEST_SENSE_NORMAL_ADDITIONAL_SENSE_CODE;

      memcpy(buffer, (uint8_t *)&requestSenseResponse, sizeof(ScsiRequestSenseResponse_t));
    }
    else
    {
      requestSenseResponse.flags = REQUEST_SENSE_ERROR_FLAGS;
      requestSenseResponse.additionalSenseCode = REQUEST_SENSE_ERROR_ADDITIONAL_SENSE_CODE;

      memcpy(buffer, (uint8_t *)&requestSenseResponse, sizeof(ScsiRequestSenseResponse_t));
    }
  }

  return sizeof(ScsiRequestSenseResponse_t);
}

//eof scsiProtocol.c
