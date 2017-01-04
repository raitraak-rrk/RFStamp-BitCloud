/***************************************************************************//**
  \file zsiSpiAdapter.c

  \brief ZAppSI SPI adapter implementation.
         Should be compiled with application.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-07-10  N. Fomin - Created.
   Last change:
    $Id: zsiSpiAdapter.c 26866 2014-06-26 12:35:56Z mukesh.basrani $
 ******************************************************************************/

#if APP_ZAPPSI_INTERFACE == APP_INTERFACE_SPI

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiSerialController.h>
#include <spi.h>
#include <halrfCtrl.h>
#include <zsiMemoryManager.h>
#include <appTimer.h>
#include <zsiDbg.h>
#include <sysUtils.h>
#include <zsiDriver.h>
#include <irq.h>

/******************************************************************************
                               Defines section
 ******************************************************************************/
#define ZSI_MEDIUM_CHANNEL APP_ZAPPSI_MEDIUM_CHANNEL
#define LINK_SAFETY_TIMEOUT 100UL /* 100 ms */

#if (APP_ZAPPSI_SPI_MASTER_MODE == 1) && defined(AT91SAM7X256)
  #define VALID_MASTER_SPI
#endif

#if (APP_ZAPPSI_SPI_MASTER_MODE == 0) && defined(ATMEGA128RFA1)
  #define VALID_SLAVE_SPI
#endif

// Start of the writing
#define START_WRITE_MARKER 0x2A
// Start of the reading
#define START_READ_MARKER  0x2B

#if !defined(VALID_MASTER_SPI) && !defined(VALID_SLAVE_SPI)
  #error 'SPI adapter is not supported for the platform.'
#endif

#if defined(VALID_MASTER_SPI)
  // spi parametres
  #define APP_SPI_MASTER_CLOCK_RATE         1000000ul
  #define APP_DELAY_BETWEEN_CS_AND_CLOCK    0.000001
  #define APP_DELAY_BETWEEN_CONS_TRANSFER   0.00016
  #define APP_SOFT_USDELAY_BETWEEN_BYTES    200
  #if defined(AT91SAM7X256)
    #define SLAVE_IRQ    IRQ_0
  #endif
#endif


/******************************************************************************
                               Types section
 ******************************************************************************/
typedef enum _SpiState_t
{
  SPI_ERR_OR_OFF,
#if defined(VALID_SLAVE_SPI)
  WAITING_MARKER,
  WAITING_RX_LEN_LSB,
  WAITING_RX_LEN_MSB,
  WAITING_TX_FRAME_CONTROL,
#endif
#if defined(VALID_MASTER_SPI)
  SPI_IDLE,
  //WAITING_TX_MARKER,
  WAITING_RX_MARKER,
  WAITING_RX_LEN,
  WAITING_RX_SOF,
#endif
  WAITING_RX_DATA,
  WAITING_TX_DATA,
  WAITING_RX_FRAME_CONTROL,
  WAITING_TSN,
} SpiState_t;

#ifdef VALID_MASTER_SPI
typedef enum _SpiMasterTypeTransaction_t
{
  MASTER_READ_TRANSACTION,
  MASTER_WRITE_TRANSACTION,
  MASTER_LAST_BYTE
} SpiMasterTypeTransaction_t;

typedef enum _SpiSlaveDataState_t
{
  SLAVE_THERE_IS_DATA,
  SLAVE_DATA_IS_READY,
  SLAVE_DATA_IS_EMPTY
} SpiSlaveDataState_t;
#endif // VALID_MASTER_SPI

/******************************************************************************
                              Global functions prototypes section
******************************************************************************/
void zsiMediumHandler(void);

/******************************************************************************
                              Static functions prototypes section
******************************************************************************/
#ifdef VALID_MASTER_SPI
HAL_ASSIGN_PIN(MASTER_CS, A, AT91C_PIO_PA21);
HAL_ASSIGN_PIN(EXT_IRQ,  A, AT91C_PIO_PA29);
static void masterSpiDelayer(void);
static void spiMasterHandler(void);
static void slaveSpiDataInd(void);
#endif // VALID_MASTER_SPI

#ifdef VALID_SLAVE_SPI
static void spiSlaveHandler(uint16_t bytesAmount);
static void zsiSpiLinkSafetyTimerFired(void);
HAL_ASSIGN_PIN(IRQ_MASTER_LINE, D, 7);
#endif // VALID_SLAVE_SPI

/******************************************************************************
                              Static variables section
******************************************************************************/
static HAL_SpiDescriptor_t zsiSpiDescriptor;

static uint8_t    rxStatus = ZSI_NO_ERROR_STATUS;
static uint8_t   *rxBuffer;
static uint8_t   *poW;
static SpiState_t spiState = SPI_ERR_OR_OFF;
static uint8_t    sequenceNumber;

#ifdef VALID_MASTER_SPI
static uint16_t dataLength;
static uint8_t *dataPointer;
static bool     masterTransactionInProgress;
static uint8_t  frameControl;
static uint16_t length;

static volatile SpiSlaveDataState_t slaveIndReady = SLAVE_DATA_IS_EMPTY;
static SpiMasterTypeTransaction_t transacType;

static const HAL_IrqMode_t thereIsData = IRQ_LOW_LEVEL;
static const HAL_IrqMode_t dataReady   = IRQ_RISING_EDGE;
#endif // VALID_MASTER_SPI

#ifdef VALID_SLAVE_SPI
static uint16_t      frameLength;
static void         *framePointer;
static volatile bool holdSpiSlave = false;
static uint8_t       rxCnt = 0;
static HAL_AppTimer_t spiLinkSafetyTimer =
{
  .interval = LINK_SAFETY_TIMEOUT,
  .mode = TIMER_ONE_SHOT_MODE,
  .callback = zsiSpiLinkSafetyTimerFired
};
#endif // VALID_SLAVE_SPI

/******************************************************************************
                              Implementations section
******************************************************************************/
#ifdef VALID_MASTER_SPI
/******************************************************************************
  \brief Serial medium initialization routine.
         Should be implemented in particular medium adapter.

  \return None.
 ******************************************************************************/
int zsiMediumInit(void)
{
  zsiSpiDescriptor.tty            = ZSI_MEDIUM_CHANNEL;
  zsiSpiDescriptor.clockMode      = SPI_CLOCK_MODE0;
  zsiSpiDescriptor.symbolSize     = SPI_8BITS_SYMBOL;
  zsiSpiDescriptor.pack_parameter = HAL_SPI_PACK_PARAMETER(APP_SPI_MASTER_CLOCK_RATE, \
                                                           APP_DELAY_BETWEEN_CS_AND_CLOCK, \
                                                           APP_DELAY_BETWEEN_CONS_TRANSFER);
  zsiSpiDescriptor.callback       = masterSpiDelayer;
  // set up master irq line
  GPIO_MASTER_CS_set();
  GPIO_MASTER_CS_make_out();
  GPIO_EXT_IRQ_make_in();
  GPIO_EXT_IRQ_make_pullup();
  // eliminate warnings about unused functions
  (void)GPIO_MASTER_CS_read;
  (void)GPIO_MASTER_CS_state;
  (void)GPIO_MASTER_CS_make_pullup;
  (void)GPIO_MASTER_CS_make_in;
  (void)GPIO_MASTER_CS_toggle;
  (void)GPIO_EXT_IRQ_read;
  (void)GPIO_EXT_IRQ_state;
  (void)GPIO_EXT_IRQ_set;
  (void)GPIO_EXT_IRQ_clr;
  (void)GPIO_EXT_IRQ_make_out;
  (void)GPIO_EXT_IRQ_toggle;

  if (0 != HAL_RegisterIrq(SLAVE_IRQ, thereIsData, slaveSpiDataInd))
  {
    /* registration failed */
  }
  
  HAL_EnableIrq(SLAVE_IRQ);

  if (-1 != HAL_OpenSpi(&zsiSpiDescriptor))
  {
    spiState = SPI_IDLE;
    return 0;
  }
  return -1;
}

/******************************************************************************
  \brief Pass data for transmission through medium.

  \param[in] frame - frame, which keeps serialized data.
  \param[in] size - actual frame size.

  \return Transmission status.
 ******************************************************************************/
int zsiMediumSend(void *frame, uint16_t size)
{
  transacType = MASTER_WRITE_TRANSACTION;
  dataPointer = frame;
  dataLength  = size;
  spiState    = WAITING_TX_DATA;

  GPIO_MASTER_CS_clr();
  masterSpiDelayer();

  if (SPI_ERR_OR_OFF == spiState)
    return -1;
  return 0;
}

/******************************************************************************
  \brief Performs holding of all HAL tasks except particular medium channel one.
         Should be used during SREQ/SRSP iteration.
         HAL tasks releasing should be performed as soon as possible.

  \return None.
 ******************************************************************************/
void zsiMediumPerformHalHoldTasks(void)
{
  HAL_HoldOnTasks(((HalTaskBitMask_t)1 << HAL_SPI1_RXBUFF) | ((HalTaskBitMask_t)1 << HAL_SPI1_TXBUFE));
}

/******************************************************************************
  \brief Releasing all previously held HAL tasks.

  \return None.
 ******************************************************************************/
void zsiMediumReleaseAllHeldTasks(void)
{
  HAL_ReleaseAllHeldTasks();
}

/******************************************************************************
  \brief Add software delay between spi bytes.

  \return None.
 ******************************************************************************/
static void masterSpiDelayer(void)
{
  int transacState;

  switch (transacType)
  {
    case MASTER_READ_TRANSACTION:
      HAL_Delay(APP_SOFT_USDELAY_BETWEEN_BYTES);
      transacState = HAL_ReadSpi(&zsiSpiDescriptor, dataPointer++, sizeof(uint8_t));
      break;
    case MASTER_WRITE_TRANSACTION:
      HAL_Delay(APP_SOFT_USDELAY_BETWEEN_BYTES);
      transacState = HAL_WriteSpi(&zsiSpiDescriptor, dataPointer++, sizeof(uint8_t));
      break;

    case MASTER_LAST_BYTE:
      spiMasterHandler();
      return;

    default:
      break;
  }

  if (transacState)
  {
    spiState = SPI_ERR_OR_OFF;
    return;
  }

  masterTransactionInProgress = true;

  if (!--dataLength)
    transacType = MASTER_LAST_BYTE;
}

/******************************************************************************
  \brief "Data is ready" from spi slave interrupt task handler

  \return None.
 ******************************************************************************/
static void slaveSpiDataInd(void)
{
  if (SLAVE_DATA_IS_EMPTY == slaveIndReady)
    slaveIndReady = SLAVE_THERE_IS_DATA;
  else if (SLAVE_THERE_IS_DATA == slaveIndReady)
    slaveIndReady = SLAVE_DATA_IS_READY;

  HAL_DisableIrq(SLAVE_IRQ);
  zsiPostTask(ZSI_SERIAL_TASK_ID);
}

/******************************************************************************
  \brief Callback on spi transaction complete

  \return None.
 ******************************************************************************/
static void spiMasterHandler(void)
{
  static uint16_t bytesToReceive;
  static uint8_t  data;

  switch (spiState)
  {
    case WAITING_RX_MARKER:
      /* START_READ_MARKER transmission completed. Check for interrupt from slave
         to continue. */
      masterTransactionInProgress = false;
      break;

    case WAITING_TX_DATA:
      GPIO_MASTER_CS_set();
      // Full packet was transmitted
      spiState = SPI_IDLE;
      masterTransactionInProgress = false;
      zsiMediumSendingDone();
      break;

    case WAITING_RX_SOF:
      transacType = MASTER_READ_TRANSACTION;
      spiState    = WAITING_RX_FRAME_CONTROL;
      dataPointer = (uint8_t*)&frameControl;
      dataLength  = sizeof(uint8_t);
      masterSpiDelayer();
      break;

    case WAITING_RX_FRAME_CONTROL:
      transacType = MASTER_READ_TRANSACTION;
      dataPointer = (uint8_t*)&length;
      dataLength  = sizeof(length);
      spiState    = WAITING_RX_LEN;
      masterSpiDelayer();
      break;

    case WAITING_RX_LEN:
      transacType = MASTER_READ_TRANSACTION;
      spiState = WAITING_TSN;
      /* Adapt length to endiannes */
      bytesToReceive = LE16_TO_CPU(length);
      /* Detect non-AREQ frames on early stage */
      if ((sizeof(ZsiAckFrame_t) - ZSI_COMMAND_FRAME_PREAMBLE_SIZE) == bytesToReceive)
        rxBuffer = zsiAllocateMemory(ZSI_RX_ACK_MEMORY);
      /* 'byte' still keeps FRAME_CONTROL field value */
      else if (ZSI_SREQ_CMD == (frameControl & ZSI_CMD_TYPE_FIELD_MASK))
        rxBuffer = zsiAllocateMemory(ZSI_SREQ_CMD);
      else if (ZSI_SRSP_CMD == (frameControl & ZSI_CMD_TYPE_FIELD_MASK))
        rxBuffer = zsiAllocateMemory(ZSI_SRSP_CMD);
      else
        rxBuffer = zsiAllocateMemory(ZSI_MUTUAL_MEMORY);
     /* Check for overflow */
      if (!rxBuffer ||
         (sizeof(ZsiCommandFrame_t) <
              (ZSI_COMMAND_FRAME_PREAMBLE_SIZE + bytesToReceive))
           )
        rxStatus = ZSI_OVERFLOW_STATUS;
        /* If ok, put frame's preamble to allocated buffer */
      else
      {
        poW    = rxBuffer;
        *poW++ = ZSI_SOF_SEQUENCE;
        *poW++ = frameControl;
        memcpy(poW, &length, sizeof(length));
        poW   += sizeof(length);
      }
      dataPointer = (uint8_t*)&sequenceNumber;
      dataLength  = sizeof(sequenceNumber);
      masterSpiDelayer();
      break;

    case WAITING_TSN:
      transacType = MASTER_READ_TRANSACTION;
      spiState = WAITING_RX_DATA;
      if (rxBuffer)
        *poW++ = sequenceNumber;
      bytesToReceive--;
      dataPointer = (uint8_t*)&data;
      dataLength  = sizeof(data);
      masterSpiDelayer();
      break;

    /* Pass the rest of the data to the buffer, if allocated */
    case WAITING_RX_DATA:
      if (bytesToReceive-- && rxBuffer)
        *poW++ = data;
      /* Frame completely received - notify serial controller */
      if (0U == bytesToReceive)
      {
        GPIO_MASTER_CS_set();
        spiState = SPI_IDLE;
        masterTransactionInProgress = false;
        zsiMediumReceive(rxStatus, sequenceNumber, rxBuffer);
        poW = rxBuffer = NULL;
        return;
      }
      else
      {
        dataPointer = (uint8_t*)&data;
        dataLength  = sizeof(data);
        transacType = MASTER_READ_TRANSACTION;
        masterSpiDelayer();
      }
      break;

    case SPI_ERR_OR_OFF:
    default:
      return;
  }
}

/******************************************************************************
  \brief Subtask for "Data is ready", should be called from task handler also.

  \return None.
 ******************************************************************************/
void zsiMediumHandler(void)
{
  static uint8_t startMarker = START_READ_MARKER;

  /* Deffer task execution until master current transaction finished */
  if (masterTransactionInProgress)
    return;

  switch (slaveIndReady)
  {
    case SLAVE_THERE_IS_DATA:
      if (SPI_IDLE == spiState)
      {
        HAL_UnregisterIrq(SLAVE_IRQ);
        if (0 != HAL_RegisterIrq(SLAVE_IRQ, dataReady, slaveSpiDataInd))
        {
          /* registration failed */
        }

        HAL_EnableIrq(SLAVE_IRQ);

        GPIO_MASTER_CS_clr();
        transacType = MASTER_WRITE_TRANSACTION;
        dataPointer = &startMarker;
        dataLength  = sizeof(uint8_t);
        spiState    = WAITING_RX_MARKER;
        masterSpiDelayer();
      }
      break;

    case SLAVE_DATA_IS_READY:
      if (WAITING_RX_MARKER == spiState)
      {
        transacType = MASTER_READ_TRANSACTION;
        dataPointer = &frameControl;
        dataLength  = sizeof(frameControl);
        spiState    = WAITING_RX_SOF;
        masterSpiDelayer();

        slaveIndReady = SLAVE_DATA_IS_EMPTY;
        // enable irq from slave
        HAL_UnregisterIrq(SLAVE_IRQ);
        if (0 != HAL_RegisterIrq(SLAVE_IRQ, thereIsData, slaveSpiDataInd))
        {
          /* registration failed */
        }
        HAL_EnableIrq(SLAVE_IRQ);
      }
      break;

    default:
      break;
    }
}

#endif // VALID_MASTER_SPI

#ifdef VALID_SLAVE_SPI
/******************************************************************************
  \brief Serial medium initialization routine.
         Should be implemented in particular medium adapter.

  \return None.
 ******************************************************************************/
int zsiMediumInit(void)
{
  zsiSpiDescriptor.tty            = ZSI_MEDIUM_CHANNEL;
  zsiSpiDescriptor.clockMode      = SPI_CLOCK_MODE0;
  zsiSpiDescriptor.dataOrder      = SPI_DATA_MSB_FIRST;
  zsiSpiDescriptor.slave_callback = spiSlaveHandler;

  // set up master irq line
  GPIO_IRQ_MASTER_LINE_set();
  GPIO_IRQ_MASTER_LINE_make_out();
  (void)GPIO_IRQ_MASTER_LINE_read;
  (void)GPIO_IRQ_MASTER_LINE_state;
  (void)GPIO_IRQ_MASTER_LINE_make_pullup;
  (void)GPIO_IRQ_MASTER_LINE_make_in;
  (void)GPIO_IRQ_MASTER_LINE_toggle;

  if (-1 != HAL_OpenSpi(&zsiSpiDescriptor))
  {
    spiState = WAITING_MARKER;
    return 0;
  }
  return -1;
}

/******************************************************************************
  \brief Pass data for transmission through medium.

  \param[in] frame - frame, which keeps serialized data.
  \param[in] size - actual frame size.

  \return Transmission status.
 ******************************************************************************/
int zsiMediumSend(void *frame, uint16_t size)
{
  framePointer = frame;
  frameLength = size;

  if (WAITING_TX_FRAME_CONTROL == spiState)
  {
    // Send frame right now
    if (HAL_WriteSpi(&zsiSpiDescriptor, frame, size))
      spiState = SPI_ERR_OR_OFF;
    else
      spiState = WAITING_TX_DATA;

    GPIO_IRQ_MASTER_LINE_set();
    if (SPI_ERR_OR_OFF == spiState)
      return -1;
  }
  else
  {
    // Defer frame
    GPIO_IRQ_MASTER_LINE_clr();
    holdSpiSlave = true;
    zsiPostTask(ZSI_SERIAL_TASK_ID);
  }
  return 0;
}

/******************************************************************************
  \brief Performs holding of all HAL tasks except particular medium channel one.
         Should be used during SREQ/SRSP iteration.
         HAL tasks releasing should be performed as soon as possible.

  \return None.
 ******************************************************************************/
void zsiMediumPerformHalHoldTasks(void)
{}

/******************************************************************************
  \brief Releasing all previously held HAL tasks.

  \return None.
 ******************************************************************************/
void zsiMediumReleaseAllHeldTasks(void)
{}

/******************************************************************************
  \brief Link safety timer fired callback. Indicates lost of syncronization or
         link failure.

  \return None.
 ******************************************************************************/
static void zsiSpiLinkSafetyTimerFired(void)
{
  if (rxBuffer)
  {
    zsiFreeMemory(rxBuffer);
  }
  sysAssert(0U, ZSIMEDIUM_ZSIMEDIUMLINKSAFETYTIMERFIRED0);
  poW = rxBuffer = NULL;
  spiState = WAITING_MARKER;
}

/******************************************************************************
  \brief Subtask for "Data is ready to be sent", should be called from task handler also.

  \return None.
 ******************************************************************************/
void zsiMediumHandler(void)
{
  if (holdSpiSlave)
    zsiPostTask(ZSI_SERIAL_TASK_ID);
}

/******************************************************************************
  \brief Callback on reception of serial byte(s)

  \param[in] bytesAmount - number of received bytes

  \return None.
 ******************************************************************************/
static void spiSlaveHandler(uint16_t bytesAmount)
{
  static uint8_t  data;
  static uint8_t  byte;
  uint16_t        leLength;
  static uint16_t bytesToReceive;

  while (bytesAmount)
  {
    switch (spiState)
    {
      case WAITING_MARKER:
      {
        if (HAL_ReadSpi(&zsiSpiDescriptor, &data, sizeof(uint8_t)) <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }
        rxStatus = ZSI_NO_ERROR_STATUS;
        if (START_WRITE_MARKER == data)
          spiState = WAITING_RX_FRAME_CONTROL;
        else if (START_READ_MARKER == data)
        {
          spiState = WAITING_TX_FRAME_CONTROL;
          zsiMediumSend(framePointer, frameLength);
        }
        else
          sysAssert(0U, ZSIMEDIUM_ZSIMEDIUMRXCALLBACK0);
      }
      bytesAmount--;
      break;

      case WAITING_RX_FRAME_CONTROL:
      {
        if (HAL_ReadSpi(&zsiSpiDescriptor, &data, sizeof(uint8_t)) <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }
        spiState = WAITING_RX_LEN_LSB;
        bytesAmount--;
      }
      break;

      case WAITING_RX_LEN_LSB:
        spiState = WAITING_RX_LEN_MSB;
        if (HAL_ReadSpi(&zsiSpiDescriptor, (uint8_t *)&byte, sizeof(uint8_t)) <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }
        bytesAmount--;
        break;

      case WAITING_RX_LEN_MSB:
        spiState = WAITING_TSN;
        if (HAL_ReadSpi(&zsiSpiDescriptor, (uint8_t *)&leLength, sizeof(uint8_t)) <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }
        leLength = (leLength << 8) + byte;
        /* Adapt length to endiannes */
        bytesToReceive = LE16_TO_CPU(leLength);
        /* Detect non-AREQ frames on early stage */
        if ((sizeof(ZsiAckFrame_t) - ZSI_COMMAND_FRAME_PREAMBLE_SIZE) == bytesToReceive)
          rxBuffer = zsiAllocateMemory(ZSI_RX_ACK_MEMORY);
        /* 'byte' still keeps FRAME_CONTROL field value */
        else if (ZSI_SREQ_CMD == (data & ZSI_CMD_TYPE_FIELD_MASK))
          rxBuffer = zsiAllocateMemory(ZSI_SREQ_CMD);
        else if (ZSI_SRSP_CMD == (data & ZSI_CMD_TYPE_FIELD_MASK))
          rxBuffer = zsiAllocateMemory(ZSI_SRSP_CMD);
        else
          rxBuffer = zsiAllocateMemory(ZSI_MUTUAL_MEMORY);

        /* Check for overflow */
        if (!rxBuffer ||
            (sizeof(ZsiCommandFrame_t) <
              (ZSI_COMMAND_FRAME_PREAMBLE_SIZE + bytesToReceive))
           )
          rxStatus = ZSI_OVERFLOW_STATUS;
        /* If ok, put frame's preamble to allocated buffer */
        else
        {
          poW    = rxBuffer;
          *poW++ = ZSI_SOF_SEQUENCE;
          *poW++ = data;

          memcpy(poW, &leLength, sizeof(leLength));
          poW += sizeof(leLength);
        }
        bytesAmount--;
        break;

      case WAITING_TSN:
        spiState = WAITING_RX_DATA;
        if (HAL_ReadSpi(&zsiSpiDescriptor, &sequenceNumber,
             sizeof(sequenceNumber)) <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }

        if (rxBuffer)
          *poW++ = sequenceNumber;
        bytesAmount--;
        bytesToReceive--;
        break;

      /* Pass the rest of the data to the buffer, if allocated */
      case WAITING_RX_DATA:
        if (bytesToReceive--)
        {
          if (HAL_ReadSpi(&zsiSpiDescriptor, &data, sizeof(data)) <= 0)
          {
            spiState = SPI_ERR_OR_OFF;
            return;
          }

          if (rxBuffer)
            *poW++ = data;
          bytesAmount--;
        }
        /* Frame completely received - notify serial controller */
        if (0U == bytesToReceive)
        {
          HAL_StopAppTimer(&spiLinkSafetyTimer);
          zsiMediumReceive(rxStatus, sequenceNumber, rxBuffer);
          poW = rxBuffer = NULL;
          spiState = WAITING_MARKER;
          return;
        }
        break;

      case WAITING_TX_DATA:
      {
        uint32_t dummyBuf;
        uint8_t tmp = MIN(bytesAmount, (frameLength - rxCnt));
        int readCnt = HAL_ReadSpi(&zsiSpiDescriptor, (uint8_t *)&dummyBuf, MIN(tmp, sizeof(dummyBuf)));

        if (readCnt <= 0)
        {
          spiState = SPI_ERR_OR_OFF;
          return;
        }

        bytesAmount -= readCnt;
        rxCnt += readCnt;
        if (rxCnt >= frameLength)
        {
          // Full packet was transmitted
          HAL_StopAppTimer(&spiLinkSafetyTimer);
          holdSpiSlave = false;
          zsiMediumSendingDone();
          rxCnt = 0;
          spiState = WAITING_MARKER;
          if (!bytesAmount)
            return;
        }
      }
      break;

      case SPI_ERR_OR_OFF:
      default:
        return;
    }
  }

  HAL_StopAppTimer(&spiLinkSafetyTimer);
  HAL_StartAppTimer(&spiLinkSafetyTimer);
}

#endif // VALID_SLAVE_SPI

#endif /* APP_ZAPPSI_INTERFACE == APP_INTERFACE_SPI */
/* eof zsiSpiAdapter.c */
