/**************************************************************************//**
\file  usbFifoFT245RL.c

\brief Implementation of FT245RL driver.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    12.07.11 A. Khromykh - Created
*******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <usbFifoVirtualUsart.h>
#include <usart.h>
#include <atomic.h>
#include <irq.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
// data port
#define USB_DATA_PORT   PORTB
#define USB_DATA_DDR    DDRB
#define USB_DATA_PIN    PINB

#elif defined(ATMEGA1281) || defined(ATMEGA2561)
// data port
#define USB_DATA_PORT   PORTA
#define USB_DATA_DDR    DDRA
#define USB_DATA_PIN    PINA

#endif

#define HANDLERS_GET(A, I) memcpy_P(A, &usbfifoHandlers[I], sizeof(UsbfifoTask_t))

/******************************************************************************
                   Types section
******************************************************************************/
typedef volatile uint8_t UsbfifoTaskBitMask_t;
typedef void (* UsbfifoTask_t)(void);

// USB FIFO task IDs.
typedef enum
{
  USB_FIFO_TASK_RX,
  USB_FIFO_TASK_TX,
  USB_FIFO_TASKS_NUMBER
} UsbfifoTaskId_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/
static void usbfifoRxBufferFiller(void);
static void usbfifoTxBufferCleaner(void);
static void usbfifoPostTask(UsbfifoTaskId_t taskId);
static void usbfifoTxComplete(void);
static void usbfifoRxComplete(void);

/******************************************************************************
                   External global variables section
******************************************************************************/
// pointer to application uart descriptor
extern HAL_UsartDescriptor_t *usbfifoPointDescrip;

/******************************************************************************
                   Global variables section
******************************************************************************/
static volatile UsbfifoTaskBitMask_t usbfifoTaskBitMask = 0; // USB FIFO tasks' bit mask.
static PROGMEM_DECLARE(UsbfifoTask_t usbfifoHandlers[USB_FIFO_TASKS_NUMBER]) =
{
  usbfifoRxComplete,
  usbfifoTxComplete
}; // List Of possible USB FIFO tasks.

/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Startup initialization.

\return
  0 - opened
  -1 - unable to initialize
******************************************************************************/
int usbfifoInit(void)
{
  // init decoder input pins
  GPIO_PC6_make_out();
  GPIO_PC7_make_out();
  // set pins to zero to select usb FIFO
  GPIO_PC6_clr();
  GPIO_PC7_clr();

  // setup default value
  GPIO_RD_set();
  GPIO_WR_set();
  // init R\W pins
  GPIO_RD_make_out();
  GPIO_WR_make_out();

  // register RXF interrupt
  if (0 != HAL_RegisterIrq(IRQ_7, IRQ_LOW_LEVEL, usbfifoRxBufferFiller))
    return -1;

  // register TXE interrupt
  if (0 != HAL_RegisterIrq(IRQ_6, IRQ_LOW_LEVEL, usbfifoTxBufferCleaner))
    return -1;

  USB_DATA_DDR = 0;

  return 0;
}

/**************************************************************************//**
\brief Clear startup initialization parameters
******************************************************************************/
void usbfifoUnInit(void)
{
  GPIO_PC6_make_in();
  GPIO_PC7_make_in();

  GPIO_RD_make_in();
  GPIO_WR_make_in();

  HAL_DisableIrq(IRQ_7);
  HAL_UnregisterIrq(IRQ_7);

  HAL_DisableIrq(IRQ_6);
  HAL_UnregisterIrq(IRQ_6);
}

/**************************************************************************//**
\brief Puts the byte received to the cyclic buffer.
******************************************************************************/
static void usbfifoRxBufferFiller(void)
{
  uint16_t           old;
  uint16_t           poW;
  uint16_t           poR;
  uint8_t           *buffer;
  HalUsartService_t *control;

  if (!usbfifoPointDescrip)
  { // unregistered intrrupt is occurred
    HAL_DisableIrq(IRQ_7);
    return;
  }

  control = &usbfifoPointDescrip->service;
  poW = control->rxPointOfWrite;
  poR = control->rxPointOfRead;
  buffer = usbfifoPointDescrip->rxBuffer;

  if (!buffer)
  {
    HAL_DisableIrq(IRQ_7);
    return;
  }

  old = poW;

  if (++poW == usbfifoPointDescrip->rxBufferLength)
    poW = 0;

  if (poW == poR)
  { // Buffer full.
    HAL_DisableIrq(IRQ_7);
    return;
  } // Buffer full.

  control->rxPointOfWrite = poW;
  GPIO_RD_clr();
  NOP;
  buffer[old] = USB_DATA_PIN;
  GPIO_RD_set();
  control->rxBytesInBuffer++;
  usbfifoPostTask(USB_FIFO_TASK_RX);
}

/**************************************************************************//**
\brief Reads byte from tx buffer and sends it to fifo.
******************************************************************************/
static void usbfifoTxBufferCleaner(void)
{
  HalUsartService_t *control;
  uint16_t           poW;
  uint16_t           poR;

  if (!usbfifoPointDescrip)
  { // unregistered intrrupt is occurred
    HAL_DisableIrq(IRQ_6);
    return;
  }

  control = &usbfifoPointDescrip->service;
  poW = control->txPointOfWrite;
  poR = control->txPointOfRead;

  if (poW != poR)
  {
    // set port as output
    USB_DATA_DDR = 0xFF;
    NOP;
    USB_DATA_PORT = usbfifoPointDescrip->txBuffer[poR++];
    GPIO_WR_clr();
    GPIO_WR_set();
    USB_DATA_DDR = 0;

    if (poR == usbfifoPointDescrip->txBufferLength)
      poR = 0;

    control->txPointOfRead = poR;
  }
  else
  { // tx buffer is empty
    HAL_DisableIrq(IRQ_6);
    usbfifoPostTask(USB_FIFO_TASK_TX);
  }
}

/**************************************************************************//**
\brief Transmitting complete handler
******************************************************************************/
static void usbfifoTxComplete(void)
{
  if (NULL == usbfifoPointDescrip)
    return;

  if (0 == usbfifoPointDescrip->txBufferLength)
    usbfifoPointDescrip->txBuffer = NULL; // nulling pointer for callback mode

  if (usbfifoPointDescrip->txCallback)
    usbfifoPointDescrip->txCallback();
}

/**************************************************************************//**
\brief Byte is received handler
******************************************************************************/
static void usbfifoRxComplete(void)
{
  HalUsartService_t *control;
  uint16_t           number;

  if (NULL == usbfifoPointDescrip)
    return;

  control = &usbfifoPointDescrip->service;
  ATOMIC_SECTION_ENTER
    number = control->rxBytesInBuffer;
  ATOMIC_SECTION_LEAVE

  if (number)
    if (NULL != usbfifoPointDescrip->rxCallback)
      usbfifoPointDescrip->rxCallback(number);
}

/**************************************************************************//**
\brief  USB FIFO driver task handler.
******************************************************************************/
void usbfifoHandler(void)
{
  UsbfifoTask_t         handler;
  uint8_t               mask = 1;
  UsbfifoTaskId_t       index = 0;

  for ( ; index < USB_FIFO_TASKS_NUMBER; index++, mask <<= 1)
  {
    if (usbfifoTaskBitMask & mask)
    {
      ATOMIC_SECTION_ENTER
      usbfifoTaskBitMask ^= mask;
      ATOMIC_SECTION_LEAVE
      HANDLERS_GET(&handler, index);
      handler();
    }
  }
}

/**************************************************************************//**
\brief Posts specific USART task.

\param[in]
  taskId - unique identifier of the task to be posted.
******************************************************************************/
static void usbfifoPostTask(UsbfifoTaskId_t taskId)
{
  usbfifoTaskBitMask |= (UsbfifoTaskBitMask_t)1 << taskId;
  halPostTask(HAL_EXT_HANDLER);
}

// eof usbFifoFT245RL.c
