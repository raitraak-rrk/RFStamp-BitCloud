/**************************************************************************//**
  \file   bspUsbVbus.c

  \brief  Implementation of board specific USB Vbus detect pin configuration

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      24/04/15 Parthasarathy G - Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if (APP_INTERFACE == APP_INTERFACE_USBCDC)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <gpio.h>
#include <irq.h>
#include <usb.h>

/*******************************************************************************
                             Local variables section
*******************************************************************************/
#if  BSP_SUPPORT == BOARD_SAMR21_XPRO
static PinConfig_t usbVbusDetect;
#endif

/*******************************************************************************
                             Prototypes section
*******************************************************************************/
extern void uhd_vbus_handler(void);
/******************************************************************************
                   Implementations section
******************************************************************************/
/**************************************************************************//**
\brief Configure the USB Vbus detect pin as external interrupt
******************************************************************************/
void BSP_BoardSpecificUsbVbusInit(void)
{
#if (BSP_SUPPORT == BOARD_SAMR21_XPRO)

  /*SAMR21 Xpro specific pin configuration*/
  usbVbusDetect.portNum = PORT_A;
  usbVbusDetect.pinNum = 7;  /*External Interrupt*/
  usbVbusDetect.functionConfig = 0;

  GPIO_pinfunc_config(&usbVbusDetect);
  HAL_RegisterIrq(IRQ_EIC_EXTINT7, IRQ_ANY_EDGE, uhd_vbus_handler);
#endif
}

/**************************************************************************//**
\brief Reads the status of the VBUS pin
\returns True if pin is logic high otherwise false
******************************************************************************/
uint8_t BSP_isUsbVbusHigh(void)
{
#if (BSP_SUPPORT == BOARD_SAMR21_XPRO)

  return GPIO_read(&usbVbusDetect);
#else
  return false;
#endif

}

#endif // if BSP_SUPPORT == BOARD_SAMR21_XPRO
// eof bspUsbVbus.c

