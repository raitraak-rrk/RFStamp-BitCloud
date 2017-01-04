/**************************************************************************//**
\file   ofdExtMemory.h

\brief  The public API of external flash driver.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    31/07/09 A. Khromykh - Created
*******************************************************************************/

#ifndef _OFDEXTMEMORY_H
#define _OFDEXTMEMORY_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
/** \brief Size of a memory block for internal OFD algorithms */
#define OFD_BLOCK_FOR_CHECK_CRC           64
#define OFD_BLOCK_SIZE \
          (APS_MAX_NON_SECURITY_ASDU_SIZE \
          - ZCL_IMAGE_BLOCK_RESP_HEADER_SIZE - ZCL_FRAME_STANDARD_HEADER_SIZE)
/******************************************************************************
                   Types section
******************************************************************************/
/** \brief Status messages for upper component about the OFD state */
typedef enum
{
  OFD_STATUS_SUCCESS = 0,
  OFD_STATUS_SERIAL_BUSY,
  OFD_STATUS_UNKNOWN_EXTERNAL_FLASH_TYPE,
  OFD_STATUS_INCORRECT_API_PARAMETER,
  OFD_STATUS_INCORRECT_EEPROM_PARAMETER,
  OFD_SERIAL_INTERFACE_BUSY
} OFD_Status_t ;

/** \brief The number of an image's position in the external flash */
typedef enum
{
  OFD_POSITION_1,  //!< The first image position
  OFD_POSITION_2,  //!< The second image position
  OFD_POSITION_MAX //!< The total number of available positions
} OFD_Position_t;

/** \brief Source type which has been initiator of image saving */
typedef enum
{
  OFD_IMAGE_WAS_SAVED_FROM_MCU,
  OFD_IMAGE_WAS_WRITTEN_THROUGH_API,
} OFD_ImageSource_t;

/** \brief Parameters for access to the external memory  \n
 offset - offset from start address  \n
 data - pointer to MCU RAM area with\for data for\from external memory  \n
 length - size of MCU RAM area with\for data. */
typedef struct
{
  uint32_t  offset;
  uint8_t  *data;
  uint32_t  length;
} OFD_MemoryAccessParam_t;

/** \brief Image crc */
typedef uint8_t OFD_Crc_t;

/** \brief Image information: crc and image type */
typedef struct
{
  OFD_ImageSource_t type; //!< An obsolete field
  OFD_Crc_t          crc;
} OFD_ImageInfo_t;

/** \brief callback signatures for the OFD API.
    ATTENTION!!!  Callback functions must not call OFD public API directly.
 */
typedef void (* OFD_Callback_t)(OFD_Status_t);
typedef void (* OFD_InfoCallback_t)(OFD_Status_t, OFD_ImageInfo_t *);

/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Opens the serial interface for connection with the external memory and checks memory type

The serial interface captured by executing this function may not be used
until OFD_Close() is called.

\param[in]
  cb - pointer to a callback function called once the interface is opened;
       must not be set to NULL
******************************************************************************/
void OFD_Open(OFD_Callback_t cb);

/**************************************************************************//**
\brief Closes the serial interface opened using OFD_Open()

The function releases the serial interface that has been opened to read from and write to
the external memory. The function is required if the serial interface is going to be used
by other components.
******************************************************************************/
void OFD_Close(void);

/**************************************************************************//**
\brief Erases an image at a given position in the external memory

The function must be called the specified image position in the external memory
before writing the new image to this position.

\param[in]
  pos - image position in the external memory
\param[in]
  cb - pointer to a callback function called once the image is completely erased;
       must not be set to NULL
******************************************************************************/
void OFD_EraseImage(OFD_Position_t pos, OFD_Callback_t cb);

/**************************************************************************//**
\brief Writes data to the external memory

The function writes a piece of the new image (specified by its image position \c pos)
at a given offset to the external memory.

The function can be called for the first time only after the serial interface
has been opened by OFD_Open() and the memory at the specified image position has
been erased by OFD_EraseImage().

\param[in]
  pos - image position of the new image
\param[in]
  accessParam - pointer to the structure containing: data offset, pointer to the
  buffer with the received data, and its length.
\param[in]
  cb - pointer to a callback function; must not be set to NULL
******************************************************************************/
void OFD_Write(OFD_Position_t pos, OFD_MemoryAccessParam_t *accessParam, OFD_Callback_t cb);

/**************************************************************************//**
\brief Flushes data from the internal buffer, checks image's crc and saves it to
the external memory

The function is called to write the rest of the new image from the internal buffer
to the external memory. The function also checks image's crc, using the buffer pointed
by \c countBuff to store temporary data, and saves it to the external memory.

Once the function completes these operations, the callback function is called.
The OFD_SwitchToNewImage() function may be called next to prepare the image for transition
to the MCU's flash and setting a command for bootloader.

\param[in]
  pos - image position of the new image
\param[in]
  countBuff - pointer to the memory for storing temporary data during function's execution
  (the buffer be of the OFD_BLOCK_FOR_CHECK_CRC length)
\param[in]
  cb - pointer to a callback function; must not be set to NULL
******************************************************************************/
void OFD_FlushAndCheckCrc(OFD_Position_t pos, uint8_t *countBuff, OFD_InfoCallback_t cb);

/**************************************************************************//**
\brief Saves the MCU's current application image and EEPROM to the external memory,
checks crc for it and sets the command for bootloader

The function prepares the MCU to switch to the new application image. Current image
is copied to the external memory at the specified image position (given by \c whereToSave),
and a command is set for bootloader that the new image (from position given by \c from)
should be written to the MCU's internal flash. The bootloader receives control after
hardware reset and writes the new image to the flash.

\param[in]
  whereToSave - image position where the MCU's current image and EEPROM should be saved
\param[in]
  from        - image position of the new image in the external memory
\param[in]
  copyBuff - pointer to the memory for storing temporary data during function's execution
  (the buffer be of the OFD_BLOCK_FOR_CHECK_CRC length)
\param[in]
  cb - pointer to a callback function indicating that the function has completed;
       must not be set to NULL
******************************************************************************/
void OFD_SwitchToNewImage(OFD_Position_t whereToSave, OFD_Position_t from, uint8_t *copyBuff, OFD_Callback_t cb);

/**************************************************************************//**
\brief Sets a command for bootloader that the new image can replace the old one

This is an obsolete function. The OFD_SwitchToNewImage() function should be used instead.

\param[in]
  from        - image position
\param[in]
  cb - pointer to callback; must not be set to NULL
******************************************************************************/
void OFD_ChangeImage(OFD_Position_t from, OFD_Callback_t cb);

/**************************************************************************//**
\brief Reads information about the image

This is an obsolete function. Should not be used.

\param[in]
  pos - image position
\param[in]
  cb - pointer to callback; must not be set to NULL
******************************************************************************/
void OFD_ReadImageInfo(OFD_Position_t pos, OFD_InfoCallback_t cb);

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
void OFD_CalCrc(OFD_Position_t pos, uint8_t *countBuff, uint32_t length, uint8_t expCrc, OFD_InfoCallback_t cb);
#endif /* _OFDEXTMEMORY_H */
