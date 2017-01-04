/**
 * \file
 *
 * \brief SAM USB Driver.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#if defined(HAL_USE_USB)

#include <usb.h>
#include <udd.h>
#include <udi_cdc.h>
#include <halUsb.h>

#if defined(USB_TEXT_MODE_SUPPORT)
#include <ctype.h>
#include <sysUtils.h>
#endif

#define HANDLERS_GET(A, I) memcpy_P(A, &halUsbHandlers[I], sizeof(HalUsbTask_t))
   
/** Fields definition from a LPM TOKEN  */
#define  USB_LPM_ATTRIBUT_BLINKSTATE_MASK      (0xF << 0)
#define  USB_LPM_ATTRIBUT_BESL_MASK            (0xF << 4)
#define  USB_LPM_ATTRIBUT_REMOTEWAKE_MASK      (1 << 8)
#define  USB_LPM_ATTRIBUT_BLINKSTATE(value)    ((value & 0xF) << 0)
#define  USB_LPM_ATTRIBUT_BESL(value)          ((value & 0xF) << 4)
#define  USB_LPM_ATTRIBUT_REMOTEWAKE(value)    ((value & 1) << 8)
#define  USB_LPM_ATTRIBUT_BLINKSTATE_L1        USB_LPM_ATTRIBUT_BLINKSTATE(1)

#define HAL_USB_ALL_TASKS_ACCEPTED_MASK 0xFF

#define HAL_USB_COMMON_TASKS_MASK   (1u << HAL_USB_TASK_USB_RX_RDY)| \
                                         (1u << HAL_USB_TASK_USB_TX_TXC)
                                       
#if defined(_USE_USB_ERROR_EVENT_)
  #define HAL_USB_ERROR_TASKS_MASK  (1u << HAL_USB_TASK_USB_ERR)
#else
  #define HAL_USB_ERROR_TASKS_MASK  (0)
#endif

#define HAL_USB_TASKS_MASK    (HAL_USB_COMMON_TASKS_MASK | HAL_USB_ERROR_TASKS_MASK)

#if defined(USB_TEXT_MODE_SUPPORT)
  #define CMD_BUF_SIZE        64
  #define FORCE_LOWCASE       0
#endif
/**************************************************************************//**
  \brief HAL Usb task type declaration.
******************************************************************************/
typedef void (* HalUsbTask_t)(void);
/**************************************************************************//**
  \brief HAL USB tasks bit mask.
******************************************************************************/
typedef volatile uint8_t HalUsbTaskBitMask_t;
/******************************************************************************
                   Global functions prototypes section
******************************************************************************/

static HAL_UsbHWDescriptor_t *halUsbPointDescriptor = NULL;

static void halUsbTaskRxReady(void);
static void halUsbTaskTxComplete(void);
static void halUsbInitHandlers(void);


void halSetUSBConfig(HAL_UsbHWDescriptor_t *usbConfig);
void halPostUsbTask(HalUsbTaskId_t taskId);

static volatile HalUsbTaskBitMask_t halUsbTaskBitMask = 0; // HAL Usb tasks' bit mask.
static HalUsbTaskBitMask_t halUsbAcceptedTasks = HAL_USB_ALL_TASKS_ACCEPTED_MASK;

static HalUsbTask_t halUsbHandlers[HAL_USB_TASKS_MAX];

#if defined (USB_TEXT_MODE_SUPPORT)
static void halUsbhandleTextMode(void);
#endif


/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief  HAL Usb task. Exact action depends on Usb internal task.
******************************************************************************/
void halSigUsbHandler(void)
{
  HalUsbTask_t         handler;
  HalUsbTaskBitMask_t  mask = 1;
  HalUsbTaskId_t       index = 0;

  for ( ; index < HAL_USB_TASKS_MAX; index++, mask <<= 1)
  {
    if (halUsbTaskBitMask & mask & halUsbAcceptedTasks)
    {
      ATOMIC_SECTION_ENTER
      halUsbTaskBitMask ^= mask;
      ATOMIC_SECTION_LEAVE
      HANDLERS_GET(&handler, index);
      handler();
    }
  }
}

/**************************************************************************//**
\brief Posts specific USB task.

\param[in]
  taskId - unique identifier of the task to be posted.
******************************************************************************/
void halPostUsbTask(HalUsbTaskId_t taskId)
{
  halUsbTaskBitMask |= (HalUsbTaskBitMask_t)1 << taskId;
  halPostTask(HAL_TASK_USB);
}

/**************************************************************************//**
\brief Wrapper for receive complete handler for USB
******************************************************************************/
static void halUsbTaskRxReady(void)
{
#if defined (USB_TEXT_MODE_SUPPORT)
  if(USB_BINARY_MODE == halUsbPointDescriptor->oprMode)
  {
#endif
    /*String parsing shall be handled in Call back*/
    HAL_UsbRxIndCallback_t usbRxIndication;
    uint16_t rxLength = udi_cdc_get_rx_length();
    usbRxIndication.length = rxLength;
    halUsbPointDescriptor->rxCallback(&usbRxIndication);
#if defined(USB_TEXT_MODE_SUPPORT)
  }
  else if (USB_TEXT_MODE == halUsbPointDescriptor->oprMode)
  {
    halUsbhandleTextMode();
  }
#endif /*USB_TEXT_MODE_SUPPORT*/
}

#if defined (USB_TEXT_MODE_SUPPORT)
static void halUsbhandleTextMode(void)
{
  /*
    Construct string of text until new line character
  */
  static uint8_t *poR = NULL;
  HAL_UsbRxIndCallback_t usbRxIndication;
  uint16_t rxLength = udi_cdc_get_rx_length();

  if(NULL == poR)
  {
    poR = halUsbPointDescriptor->rxBuffer;
  }
  if ((poR + rxLength) >= 
  (halUsbPointDescriptor->rxBuffer + halUsbPointDescriptor->rxBufferLength))
  {
    usbRxIndication.status = RX_BUFFER_FULL;
    poR= NULL; /*To receive next line*/
    if(NULL != halUsbPointDescriptor->rxCallback)
      halUsbPointDescriptor->rxCallback(&usbRxIndication);
    return;
  }
  int16_t bytesRead = HAL_ReadUsb(poR,
                          MIN(halUsbPointDescriptor->rxBufferLength, rxLength));
  for (int16_t i = 0; i < bytesRead; i++)
  {
    switch(*poR)
    {
      case 0xd:  /*/r CR - EOL */
        if(poR > halUsbPointDescriptor->rxBuffer)
        { /* Received a non-empty command*/
          *poR = 0;           // Mark an end of string
          usbRxIndication.status = RX_COMPLETE;
          poR= NULL; /*To receive next line*/
          if(NULL != halUsbPointDescriptor->rxCallback)
            halUsbPointDescriptor->rxCallback(&usbRxIndication);
          /*Discard the remaining data*/
          return;
        }
      break;
      case 0x8:  /* /b - Back space */
        if(poR > halUsbPointDescriptor->rxBuffer)
        {
          *(--poR) = 0;               // Rewind and terminate
        }
      break;
      case 0x1b:  /*ESC:  flush buffer and erase symbols*/
        /*Move the remaining data to start of the buffer*/
        memcpy((void *)halUsbPointDescriptor->rxBuffer,(void *)++poR,(bytesRead - i - 1));
        /*Reset POR to start of buffer*/
        poR = halUsbPointDescriptor->rxBuffer;
      break;
      default:    /*Normal String*/
        if (isprint((int)*poR))
          ++poR;
        else  /*is a no printable char, ignore it*/
        {
          /*Ignore this byte and advance the remaining data */
          memcpy(poR,poR+1,(bytesRead - i - 1));
        }
      break;
    }
  }
}
#endif /*USB_TEXT_MODE_SUPPORT*/
/**************************************************************************//**
\brief Wrapper for receive complete handler for USB
******************************************************************************/
static void halUsbTaskTxComplete(void)
{
  if(NULL != halUsbPointDescriptor->txCallback)
      halUsbPointDescriptor->txCallback();
}

/**************************************************************************//**
\brief Holds execution of all tasks except related to reqiured channel.

\param[in] channel - channel to accept tasks for
******************************************************************************/
void HAL_HoldOnOthersUsbTasks(void)
{
  halUsbAcceptedTasks &= HAL_USB_TASKS_MASK;
}

/**************************************************************************//**
\brief Accepts execution of previously holded tasks.
******************************************************************************/
void HAL_ReleaseAllHeldUsbTasks(void)
{
  halUsbAcceptedTasks = HAL_USB_ALL_TASKS_ACCEPTED_MASK;
  if (halUsbTaskBitMask)
    halPostTask(HAL_TASK_USB);
}

#if defined(_USE_USB_ERROR_EVENT_)
/**************************************************************************//**
\brief Save status register for analyzing of the error reason.

\param[in]
  tty - channel number.
\param[in]
  status - Usb status register.
******************************************************************************/
void halUsbSaveErrorReason(UsbChannel_t tty, uint8_t status)
{
  /* TODO : Need to handle errors */
}
#endif

/**************************************************************************//**
\brief To initialize usb handlers
******************************************************************************/
static void halUsbInitHandlers(void)
{
  halUsbHandlers[HAL_USB_TASK_USB_RX_RDY] = halUsbTaskRxReady;
  halUsbHandlers[HAL_USB_TASK_USB_TX_TXC] = halUsbTaskTxComplete;
}

/**************************************************************************//**
\brief Registers Usb's event handlers. Performs configuration
of Usb registers. D+ and D- pins

\param[in]
  descriptor - pointer to HAL_UsbHWDescriptor_t structure

\return
  Returns positive Usb descriptor on success or -1 in cases: \n
    - bad Usb channel. \n
    - unsupported parameters. \n
    - the channel was already opened. \n
    - there are not enough resources. \n
******************************************************************************/
int HAL_OpenUsb(HAL_UsbHWDescriptor_t *descriptor)
{
  uint32_t flags;  
  if (NULL == descriptor)
    return -1;
  halUsbPointDescriptor = descriptor;

  halUsbInitHandlers();

#if !defined(USB_TEXT_MODE_SUPPORT)
  if(USB_TEXT_MODE == descriptor->oprMode)
  {
    return -1;
  }
#endif
  struct usb_config config_usb;

	/* USB Module configuration */
	
  config_usb.select_host_mode = 0;  // Device Mode
	config_usb.run_in_standby = 1;
	config_usb.speed_mode = descriptor->speed;
	config_usb.source_generator = GCLK_GEN_5;
   
  usb_init(&config_usb);
  
  /* To avoid USB interrupt before end of initialization */	
  halStartAtomic((atomic_t volatile *)&flags);
	/* USB Module Enable */
  if(!(USB_CTRLA & USB_CTRLA_ENABLE))
    usb_enable();
	/* Check clock after enable module, request the clock */
	halDfllReady();  
  halUsbEnableInterrupt();

#if defined (HAL_USB_HOT_PLUG_ENUMERATION)
  if(true == descriptor->usbVbusDetect) // Is VBUS detection available?
  {
    BSP_BoardSpecificUsbVbusInit();
    if (BSP_isUsbVbusHigh())
    {
      /*
        USB cable is connected to host,proceed with enumeration
      */
      /* USB Attach */
      udd_attach();
      halEndAtomic((atomic_t volatile *)&flags);
      while(!udi_cdc_enumeration_complete());
    }
    else
    {
      /*
        USB is still not connected to host.
        So, exit now and attach UDD once it is hot plugged.
      */
      halEndAtomic((atomic_t volatile *)&flags);
    }
  }
  else
#endif //  HAL_USB_HOT_PLUG_ENUMERATION
  {
#if (USB_DEVICE_ATTR == USB_CONFIG_ATTR_BUS_POWERED)
    udd_attach();
    halEndAtomic((atomic_t volatile *)&flags);
    while(!udi_cdc_enumeration_complete());
#endif
  }

  return 1;
}
/**************************************************************************//**
\brief Writes a number of bytes to a USB cdc line.
txCallback function will be used to notify when the transmission is finished.

\param[in]
  buffer - pointer to the application data buffer;

\param[in]
  length - number of bytes to transfer;

\return
   -1 - if bad pointer (or)
   Number of bytes placed to the buffer - success.
******************************************************************************/
int HAL_WriteUsb(uint8_t *data, uint8_t length)
{
  if (NULL != data)
  {
    return (length - udi_cdc_write_buf(data,length));
  }
  else
    return (-1);
}
/*************************************************************************//**
\brief Reads length bytes from usb and places  to buffer.

\param[out]
  buffer - pointer to a application buffer;
\param[in]
  length - the number of bytes which should be placed to buffer

\return
  -1 - if bad pointer (or)
  number of bytes that were placed to buffer.
*****************************************************************************/

int HAL_ReadUsb(uint8_t *data, uint8_t length)
{
  if (NULL != data)
  {
    return (length - udi_cdc_read_buf(data,length));
  }
  else
    return (-1);
}
/**************************************************************************//**
\brief Frees the Usb channel and pins, if hardware flow control was used.

\param[in]
  descriptor - the Usb descriptor.
\return
   +ve value on success, \n
  -1 if bad descriptor or channel is already closed.
******************************************************************************/
int HAL_CloseUsb(void)
{
  udd_detach();
  usb_disable();
  return 1;
}
#endif /* #if defined(HAL_USE_USB) */
