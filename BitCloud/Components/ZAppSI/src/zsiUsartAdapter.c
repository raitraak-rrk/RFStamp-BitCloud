/***************************************************************************//**
  \file zsiUsartAdapter.c

  \brief ZAppSI USART adapter implementation.
         Should be compiled with application.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-01  A. Razinkov - Created.
   Last change:
    $Id: zsiUsartAdapter.c 25433 2013-10-04 11:38:06Z nfomin $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiSerialController.h>
#include <zsiSerialInterface.h>
#include <zsiMemoryManager.h>
#include <appTimer.h>
#include <zsiDbg.h>
#include <sysUtils.h>
#include <zsiDriver.h>
#include <halTaskManager.h>

#if (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART) || (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO)

/******************************************************************************
                               Defines section
 ******************************************************************************/
#define ZSI_MEDIUM_CHANNEL APP_ZAPPSI_MEDIUM_CHANNEL
#define LINK_SAFETY_TIMEOUT 300UL /* 300 ms */

/******************************************************************************
                               Types section
 ******************************************************************************/
enum
{
  ZSI_USART_WAITING_SOF_STATE,
  ZSI_USART_WAITING_FRAME_CONTROL_STATE,
  ZSI_USART_RECEIVING_LENGTH_LSB_STATE,
  ZSI_USART_RECEIVING_LENGTH_MSB_STATE,
  ZSI_USART_RECEIVING_SEQUENCE_NUMBER_STATE,
  ZSI_USART_RECEIVING_DATA_STATE,
  ZSI_USART_ERROR_STATE
};


/******************************************************************************
                              Global functions prototypes section
******************************************************************************/
void zsiMediumHandler(void);

/******************************************************************************
                              Static functions prototypes section
******************************************************************************/
static void zsiUsartRxCallback(uint16_t bytesAmount);
static void zsiUsartLinkSafetyTimerFired(void);

/******************************************************************************
                              Static variables section
******************************************************************************/
static HAL_UsartDescriptor_t zsiUsartDescriptor;
static uint8_t rxState = ZSI_USART_WAITING_SOF_STATE;
static uint8_t rxStatus = ZSI_NO_ERROR_STATUS;
static uint8_t *rxBuffer;
static uint8_t *poW;
static HAL_AppTimer_t usartLinkSafetyTimer =
{
  .interval = LINK_SAFETY_TIMEOUT,
  .mode = TIMER_ONE_SHOT_MODE,
  .callback = zsiUsartLinkSafetyTimerFired
};

/******************************************************************************
                              Implementations section
******************************************************************************/
/******************************************************************************
  \brief Stub subtask

  \return None.
 ******************************************************************************/
void zsiMediumHandler(void)
{}

/******************************************************************************
  \brief Serial medium initialization routine.
         Should be implemented in particular medium adapter.

  \return None.
 ******************************************************************************/
int zsiMediumInit(void)
{
  HAL_StopAppTimer(&usartLinkSafetyTimer);

  zsiUsartDescriptor.tty            = ZSI_MEDIUM_CHANNEL;
  zsiUsartDescriptor.mode           = USART_MODE_ASYNC;
  zsiUsartDescriptor.flowControl    = USART_FLOW_CONTROL_NONE;
  zsiUsartDescriptor.baudrate       = USART_BAUDRATE_38400;
  zsiUsartDescriptor.dataLength     = USART_DATA8;
  zsiUsartDescriptor.parity         = USART_PARITY_NONE;
  zsiUsartDescriptor.stopbits       = USART_STOPBIT_1;
  zsiUsartDescriptor.rxBuffer       = zsiAllocateMemory(ZSI_MUTUAL_MEMORY);
  zsiUsartDescriptor.rxBufferLength = ZSI_MEDIUM_RX_BUFFER_LENGTH;
  zsiUsartDescriptor.txBuffer       = NULL;
  zsiUsartDescriptor.txBufferLength = 0U;
  zsiUsartDescriptor.rxCallback     = zsiUsartRxCallback;
  zsiUsartDescriptor.txCallback     = zsiMediumSendingDone;
  return OPEN_ZAPPSI_INTERFACE(&zsiUsartDescriptor);
}

/******************************************************************************
  \brief Link safety timer fired callback. Indicates lost of syncronization or
         link failure.

  \return None.
 ******************************************************************************/
static void zsiUsartLinkSafetyTimerFired(void)
{
  if (rxBuffer)
  {
    zsiFreeMemory(rxBuffer);
  }
  sysAssert(0U, ZSIMEDIUM_ZSIMEDIUMLINKSAFETYTIMERFIRED0);
  poW = rxBuffer = NULL;
  rxState = ZSI_USART_WAITING_SOF_STATE;
}

/******************************************************************************
  \brief Pass data for transmission through medium.

  \param[in] frame - frame, which keeps serialized data.
  \param[in] size - actual frame size.

  \return Transmission status.
 ******************************************************************************/
int zsiMediumSend(void *frame, uint16_t size)
{
  return WRITE_ZAPPSI_INTERFACE(&zsiUsartDescriptor, frame, size);
}

/******************************************************************************
  \brief Performs holding of all HAL tasks except particular medium channel one.
         Should be used during SREQ/SRSP iteration.
         HAL tasks releasing should be performed as soon as possible.

  \return None.
 ******************************************************************************/
void zsiMediumPerformHalHoldTasks(void)
{
  HAL_HoldOnTasks((HalTaskBitMask_t)1 << ZAPPSI_INTERFACE_TASK);
  HOLD_ADDITIONAL_ZAPPSI_INTERFACE_TASKS(zsiUsartDescriptor.tty);
}

/******************************************************************************
  \brief Releasing all previously held HAL tasks.

  \return None.
 ******************************************************************************/
void zsiMediumReleaseAllHeldTasks(void)
{
  HAL_ReleaseAllHeldTasks();
  RELEASE_ADDITIONAL_ZAPPSI_INTERFACE_TASKS();
}

/******************************************************************************
  \brief Callback for new data received from USART.

  \param[in] length - received data size.

  \return None.
 ******************************************************************************/
static void zsiUsartRxCallback(uint16_t bytesAmount)
{
  static uint16_t bytesToReceive;
  static uint8_t byte;
  static uint8_t sequenceNumber;
  uint16_t leLength;
  static uint8_t frameControl;

  while (bytesAmount)
  {
    switch (rxState)
    {
      /* Check for valid SOF */
      case ZSI_USART_WAITING_SOF_STATE:
        if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, &byte, sizeof(byte)) <= 0)
        {
          rxState = ZSI_USART_ERROR_STATE;
          return;
        }

        if (ZSI_SOF_SEQUENCE == byte)
        {
          /* SOF captured - reset rx status and switch to length waiting */
          rxStatus = ZSI_NO_ERROR_STATUS;
          rxState = ZSI_USART_WAITING_FRAME_CONTROL_STATE;
        }
        else
        {
          sysAssert(0U, ZSIMEDIUM_ZSIMEDIUMRXCALLBACK0);
        }
        bytesAmount--;
        break;

      /* Capture frame control and identify frame type */
      case ZSI_USART_WAITING_FRAME_CONTROL_STATE:
        if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, &frameControl, sizeof(frameControl)) <= 0)
        {
          rxState = ZSI_USART_ERROR_STATE;
          return;
        }

        rxState = ZSI_USART_RECEIVING_LENGTH_LSB_STATE;
        bytesAmount--;
        break;

      /* Postpone length LSB reading */
      case ZSI_USART_RECEIVING_LENGTH_LSB_STATE:
        rxState = ZSI_USART_RECEIVING_LENGTH_MSB_STATE;
        if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, (uint8_t *)&byte,
                          sizeof(uint8_t)) <= 0)
        {
          rxState = ZSI_USART_ERROR_STATE;
          return;
        }
        bytesAmount--;
        break;

      /* Read frame length */
      case ZSI_USART_RECEIVING_LENGTH_MSB_STATE:
        rxState = ZSI_USART_RECEIVING_SEQUENCE_NUMBER_STATE;

        if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, (uint8_t *)&leLength,
                          sizeof(uint8_t)) <= 0)
        {
          rxState = ZSI_USART_ERROR_STATE;
          return;
        }
        leLength = (leLength << 8) + byte;

        /* Adapt length to endiannes */
        bytesToReceive = LE16_TO_CPU(leLength);
        /* Detect non-AREQ frames on early stage */
        if ((sizeof(ZsiAckFrame_t) - ZSI_COMMAND_FRAME_PREAMBLE_SIZE) == bytesToReceive)
        {
          rxBuffer = zsiAllocateMemory(ZSI_RX_ACK_MEMORY);
        }
        else if (ZSI_SREQ_CMD == (frameControl & ZSI_CMD_TYPE_FIELD_MASK))
        {
          rxBuffer = zsiAllocateMemory(ZSI_SREQ_CMD);
        }
        else if (ZSI_SRSP_CMD == (frameControl & ZSI_CMD_TYPE_FIELD_MASK))
        {
          rxBuffer = zsiAllocateMemory(ZSI_SRSP_CMD);
        }
        else
        {
          rxBuffer = zsiAllocateMemory(ZSI_MUTUAL_MEMORY);
        }

        /* Check for overflow */
        if (!rxBuffer ||
            (sizeof(ZsiCommandFrame_t) <
             (ZSI_COMMAND_FRAME_PREAMBLE_SIZE + bytesToReceive))
           )
          rxStatus = ZSI_OVERFLOW_STATUS;
        /* If ok, put frame's preamble to allocated buffer */
        else
        {
          poW = rxBuffer;
          *poW++ = ZSI_SOF_SEQUENCE;
          *poW++ = frameControl;

          SYS_BYTE_MEMCPY(poW, &leLength, sizeof(leLength));
          poW += sizeof(leLength);
        }
        bytesAmount --;
        break;

      /* Read Sequence Number field and put it down to buffer, if allocated */
      case ZSI_USART_RECEIVING_SEQUENCE_NUMBER_STATE:
        rxState = ZSI_USART_RECEIVING_DATA_STATE;
        if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, &sequenceNumber,
                          sizeof(sequenceNumber)) <= 0)
        {
          rxState = ZSI_USART_ERROR_STATE;
          return;
        }

        if (rxBuffer)
        {
          *poW++ = sequenceNumber;
        }
        bytesAmount--;
        bytesToReceive--;
        break;

      /* Pass the rest of the data to the buffer, if allocated */
      case ZSI_USART_RECEIVING_DATA_STATE:
        if (bytesToReceive--)
        {
          if (READ_ZAPPSI_INTERFACE(&zsiUsartDescriptor, &byte, sizeof(byte)) <= 0)
          {
            rxState = ZSI_USART_ERROR_STATE;
            return;
          }

          if (rxBuffer)
          {
            *poW++ = byte;
          }
          bytesAmount--;
        }
        /* Frame completely received - notify serial controller */
        if (0U == bytesToReceive)
        {
          HAL_StopAppTimer(&usartLinkSafetyTimer);
          zsiMediumReceive(rxStatus, sequenceNumber, rxBuffer);
          poW = rxBuffer = NULL;
          rxState = ZSI_USART_WAITING_SOF_STATE;
        }
        break;

      case ZSI_USART_ERROR_STATE:
      default:
        return;
    }
  }

  /* Throw bone to link safety timer */
  if (ZSI_USART_WAITING_SOF_STATE != rxState)
  {
    HAL_StopAppTimer(&usartLinkSafetyTimer);
    HAL_StartAppTimer(&usartLinkSafetyTimer);
  }
}

#endif /* (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USART) || (APP_ZAPPSI_INTERFACE == APP_INTERFACE_USBFIFO) */
/* eof zsiUsartAdapter.c */
