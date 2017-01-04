/**************************************************************************//**
\file  isdConsoleTunneling.c

\brief Implementation of mechanism of console tunneling.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    30.01.13 S. Dmitriev - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#if (APP_USE_OTAU == 1)

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#if APP_INTERFACE == APP_INTERFACE_USBCDC
#include <usb.h>
#else
#include <usart.h>
#endif
#include <isdConsoleTunneling.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#if (APP_USE_ISD_CONSOLE_TUNNELING == 1)
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct _RingBuffer_t
{
  uint8_t *start;
  uint8_t *end;
  uint8_t *ptr;
  uint8_t *ptw;
  struct 
  {
    uint8_t isEmpty  :1;
    uint8_t isFull   :1;
    uint8_t reserved :6;
  };
} RingBuffer_t;

/******************************************************************************
                   Static Function Prototypes section
******************************************************************************/
static void raiseTunnelTxCallback(void);

static void initRingBuffer(RingBuffer_t *ring, uint8_t *buffer, uint16_t bufSize);
static void writeRingBuffer(RingBuffer_t *ring, uint8_t ch);
static uint8_t readRingBuffer(RingBuffer_t *ring);
static uint8_t lengthRingBuffer(RingBuffer_t *ring);
//static bool setRingBufferFull(RingBuffer_t *ring);

/******************************************************************************
                   Static variables section
******************************************************************************/

#if APP_INTERFACE == APP_INTERFACE_USBCDC
  static HAL_UsbHWDescriptor_t *tunnelDescriptor;
  static HAL_UsbRxIndCallback_t usbRxIndication;
  typedef HAL_UsbHWDescriptor_t ISD_TunnelDescriptor_t;
#else
  static HAL_UsartDescriptor_t *tunnelDescriptor;
  typedef  HAL_UsartDescriptor_t ISD_TunnelDescriptor_t;
#endif
static RingBuffer_t rxfifo;
static RingBuffer_t txfifo;
static bool wasSomethingSent = false;

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Initialize ISD tunnel.

\param[in] descriptor - pointer to USART descriptor

\return zero value if tunnel was initialized, false otherwise.
******************************************************************************/
int ISD_OpenTunnel(ISD_TunnelDescriptor_t *descriptor)
{
  if ((!descriptor->txBuffer) || (!descriptor->rxBuffer))
    return -1;

  if (descriptor->txBuffer)
    initRingBuffer(&txfifo, descriptor->txBuffer, descriptor->txBufferLength);

  if (descriptor->rxBuffer)  
    initRingBuffer(&rxfifo, descriptor->rxBuffer, descriptor->rxBufferLength);

  tunnelDescriptor = descriptor;

  return 0;
}

/**************************************************************************//**
\brief Close ISD tunnel.

\param[in] descriptor - pointer to USART descriptor

\return always zero. (to be correspond public API).
******************************************************************************/
int ISD_CloseTunnel(ISD_TunnelDescriptor_t *descriptor)
{
  if (tunnelDescriptor->txBuffer)
    initRingBuffer(&txfifo, NULL, 0U);

  if (tunnelDescriptor->rxBuffer)
    initRingBuffer(&rxfifo, NULL, 0U);

  tunnelDescriptor = NULL;
  
  (void)descriptor; // keep compiler happy

  return 0;
}

/**************************************************************************//**
\brief Read contents from ISD tunnel to buffer.

\param[in] descriptor - pointer to USART descriptor
\param[in] buffer - pointer to destination buffer
\param[in] length - buffer length

\return number of read bytes or -1 if error
******************************************************************************/
int ISD_ReadTunnel(ISD_TunnelDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t wasReaded = 0;

  if ((!tunnelDescriptor) || (tunnelDescriptor != descriptor))
    return -1;

  while ((!rxfifo.isEmpty) && (wasReaded < length))
  {
    *buffer++ = readRingBuffer(&rxfifo);
    wasReaded++;
  }

  return wasReaded;
}

/**************************************************************************//**
\brief Send contents of buffer over USART through ISD tunnel.

\param[in] descriptor - pointer to USART descriptor
\param[in] buffer - buffer to be sent
\param[in] length - buffer length

\return number of sent bytes or -1 if error
******************************************************************************/
int ISD_WriteTunnel(ISD_TunnelDescriptor_t *descriptor, uint8_t *buffer, uint16_t length)
{
  uint16_t wasWrote = 0;

  if ((!tunnelDescriptor) || (tunnelDescriptor != descriptor))
    return -1;

  while ((!txfifo.isFull) && (wasWrote < length))
  {
    writeRingBuffer(&txfifo, *buffer++);
    wasWrote++;
  }
  isdSendTunnelingDate();
  return wasWrote;
}

/**************************************************************************//**
\brief Puts char in rx tunneling buffer.

\param[in] char to write

\return true if char was written correctly, false otherwise.
******************************************************************************/
bool putCharInTunnel(uint8_t ch)
{
  if (!tunnelDescriptor)
    return false;

  if (rxfifo.isFull)
    raiseTunnelRxCallback();

  writeRingBuffer(&rxfifo, ch);
  return true;
}

/**************************************************************************//**
\brief Gets char from tx tunneling buffer.

\return read char
******************************************************************************/
uint8_t getCharFromTunnel(void)
{
  wasSomethingSent = true;
  return readRingBuffer(&txfifo);
}

/**************************************************************************//**
\brief Check there is something to tunneling from upper layer.

\return true if buffer of transmit isn't empty, false otherwise.
******************************************************************************/
bool isTunnelingDateToSend(void)
{
  if (txfifo.isEmpty)
  {
    if (wasSomethingSent)
    {
      wasSomethingSent = false;
      raiseTunnelTxCallback();
    }
    return false;
  }

  return true;
}

/**************************************************************************//**
\brief Check is allocated buffers for tunneling or not.

\return true if tunnel was initialized, false otherwise.
******************************************************************************/
bool isTunnelInitialized(void)
{
  if (!tunnelDescriptor)
    return false;

  return true;
}

/**************************************************************************//**
\brief Raised callback to upper layer.
******************************************************************************/
void raiseTunnelRxCallback(void)
{
  if ((tunnelDescriptor) && (tunnelDescriptor->rxCallback) && lengthRingBuffer(&rxfifo))
  {
   #if APP_INTERFACE == APP_INTERFACE_USBCDC
   usbRxIndication.length = lengthRingBuffer(&rxfifo);
   tunnelDescriptor->rxCallback(&usbRxIndication);
   #else
   tunnelDescriptor->rxCallback(lengthRingBuffer(&rxfifo));
   #endif
  }
}

/**************************************************************************//**
\brief Raised callback to upper layer.
******************************************************************************/
static void raiseTunnelTxCallback(void)
{
  if ((tunnelDescriptor) && (tunnelDescriptor->txCallback))
    tunnelDescriptor->txCallback();
}

/**************************************************************************//**
\brief Initializes circle buffer.
******************************************************************************/
static void initRingBuffer(RingBuffer_t *ring, uint8_t *buffer, uint16_t bufSize)
{
  ring->start = ring->ptr = ring->ptw = buffer;
  ring->end = ring->start + bufSize; 
  ring->isEmpty = true;
  ring->isFull = false;
  ring->reserved = 0U;
  memset(buffer, 0x00, bufSize);
}

/**************************************************************************//**
\brief Write char to circle buffer.
******************************************************************************/
static void writeRingBuffer(RingBuffer_t *ring, uint8_t ch)
{
  if (ring->isFull)
  {
    ring->ptr++;
    if (ring->end == ring->ptr)
      ring->ptr = ring->start;
  }

  *ring->ptw++ = ch;

  if (ring->end == ring->ptw)
    ring->ptw = ring->start;

  ring->isEmpty = false;
  if (ring->ptw == ring->ptr)
    ring->isFull = true;
}

/**************************************************************************//**
\brief Read char from circle buffer.

\return read byte
******************************************************************************/
static uint8_t readRingBuffer(RingBuffer_t *ring)
{
  uint8_t result;

  // nothing to read
  if (ring->isEmpty)
    return 0;

  result = *ring->ptr;

  ring->ptr++;

  if (ring->end == ring->ptr)
    ring->ptr = ring->start;

  ring->isFull = false;
  if (ring->ptw == ring->ptr)
    ring->isEmpty = true;

  return result;
}

/**************************************************************************//**
\brief Gets length of circle buffer.

\return length
******************************************************************************/
static uint8_t lengthRingBuffer(RingBuffer_t *ring)
{
  if (ring->isEmpty)
    return 0;

  if (ring->isFull)
    return (ring->end - ring->start) / sizeof(uint8_t);

  if (ring->ptr < ring->ptw)
    return (ring->ptw - ring->ptr) / sizeof(uint8_t);
  else 
    return  (ring->end - ring->ptr) / sizeof(uint8_t) + \
            (ring->ptw - ring->start) / sizeof(uint8_t);
}

/**************************************************************************//**
\brief Changing state circle buffer from empty to full.
******************************************************************************/
/*
static bool setRingBufferAsFull(RingBuffer_t *ring)
{
  if (ring->isEmpty)
  {
    ring->isEmpty = false;
    ring->isFull = true;
    return true;
  }
  return false;
}
*/
#else // APP_USE_ISD_CONSOLE_TUNNELING == 1

/**************************************************************************//**
\brief Gets char from tx tunneling buffer.

\return read char
******************************************************************************/
uint8_t getCharFromTunnel(void)
{
  return 0;
}

/**************************************************************************//**
\brief Puts char in rx tunneling buffer.

\param[in] char to write

\return true if char was written correctly, false otherwise.
******************************************************************************/
bool putCharInTunnel(uint8_t ch)
{
  (void)ch;
  return false;
}

/**************************************************************************//**
\brief Check is allocated buffers for tunneling or not.

\return true if tunnel was initialized, false otherwise.
******************************************************************************/
bool isTunnelInitialized(void)
{
  return false;
}

/**************************************************************************//**
\brief Check there is something to tunneling from upper layer.

\return true if buffer of transmit isn't empty, false otherwise.
******************************************************************************/
bool isTunnelingDateToSend(void)
{
  return false;
}

/**************************************************************************//**
\brief Raised callback to upper layer.
******************************************************************************/
void raiseTunnelRxCallback(void)
{}

#endif // APP_USE_ISD_CONSOLE_TUNNELING == 1
#endif // APP_USE_OTAU == 1

//eof isdConsoleTunneling.c