/*****************************************************************************
  \file  halReset.c

  \brief Implementation of the software reset

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    07/04/14 karthik.p_u - Created
******************************************************************************/

/*****************************************************************************
                              Includes section
******************************************************************************/
#include <sysTypes.h>
#include <atsamr21.h>
#include <atomic.h>
#include <halReset.h>
#include <halInterrupt.h>
#include <halRfCtrl.h>
#include <halMacIsr.h>
#include <Component_usb.h>

/*****************************************************************************
                              Definitions section
******************************************************************************/

/******************************************************************************
                              Implementations section
******************************************************************************/
/**************************************************************************//**
 \brief Read the reset status register.
 \return Current value of the reset status register.
******************************************************************************/
HAL_ResetReason_t halReadResetReason(void)
{
  return (HAL_ResetReason_t)(PM_RCAUSE & (PM_RCAUSE_POR | PM_RCAUSE_BOD12 | PM_RCAUSE_BOD33 | \
                             PM_RCAUSE_EXT | PM_RCAUSE_WDT | PM_RCAUSE_SYST ));
}

/**************************************************************************//**
 \brief Jump to reset vector address  instead of core reset
******************************************************************************/
#if defined(HAL_USE_USB)
static void halWarmResetUsb (void)
{
  // To avoid USB renumeration on host
  NVIC_DisableIRQ(USB_IRQn);
  NVIC_ClearPendingIRQ(USB_IRQn);
  HAL_StopRtimer();
  /*Reset the RF module*/
  HAL_ClearRfRst();
  // Delay 6 us
  HAL_Delay(6);
  HAL_SetRfRst();

  NVIC_DisableIRQ(EIC_IRQn);
  NVIC_ClearPendingIRQ(EIC_IRQn);

  NVIC_DisableIRQ(ADC_IRQn);
  NVIC_ClearPendingIRQ(ADC_IRQn);

  NVIC_DisableIRQ(TC3_IRQn);
  NVIC_ClearPendingIRQ(TC3_IRQn);

  NVIC_DisableIRQ(TC4_IRQn);
  NVIC_ClearPendingIRQ(TC4_IRQn);

  NVIC_DisableIRQ(RTC_IRQn);
  NVIC_ClearPendingIRQ(RTC_IRQn);

  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);

  /* Initialize stack pointer*/

#ifdef  __IAR_SYSTEMS_ICC__
  __set_SP((uint32_t)STACK_TOP);
#elif __GNUC__
  __set_MSP((uint32_t)STACK_TOP);
#endif
  void (*reset)() = RESET_ADDR;
  reset();
}
#endif
/**************************************************************************//**
 \brief Reset core and peripherals.
******************************************************************************/
void halSoftwareReset(void)
{
  // software reset processor core and peripherals
#if defined(HAL_USE_USB)
if(USB_CTRLA & USB_CTRLA_ENABLE)
  halWarmResetUsb(); 
else
#endif 
  NVIC_SystemReset();
}

// eof halReset.c
