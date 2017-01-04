/**************************************************************************//**
  \file  halWdt.c

  \brief Implementation of WDT interrupt handler.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    07/04/14 karthik.p_u - Created
    Last Change:
      $Id: halWdt.c 28370 2015-08-25 06:57:37Z parthasarathy.gunase $
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
#if defined(_STATS_ENABLED_)
#include <sysTypes.h>
#include <statStack.h>
#endif

#if defined(HAL_USE_WDT)
/******************************************************************************
                   Includes section
******************************************************************************/
#include <wdtCtrl.h>
#include <atomic.h>
#include <halDbg.h>
#include <halDiagnostic.h>
#include <atsamr21.h>
#include <core_cm0plus.h>

#if defined(HAL_USE_USB)
#include <resetReason.h>
#include <halRfCtrl.h>
#include <Component_usb.h>
#endif

/******************************************************************************
                   Static variables section
******************************************************************************/
#define WDT_TC_COUNT (F_CPU/1024ul)

/******************************************************************************
                   Static variables section
******************************************************************************/
static uint8_t wdt_count = 0;
static uint8_t wdt_tc_interval = 0;
 
/******************************************************************************
                   Global variables section
******************************************************************************/
void (*halWdtCallback)(void) = NULL;
void wdtTimerHandler(void);
void wdtTCHandler(void);

/*******************************************************************************
Registers WDT fired callback.
Parameters:
  wdtCallback - callback.
Returns:
  none.
*******************************************************************************/
void HAL_RegisterWdtCallback(void (*wdtCallback)(void))
{
  halWdtCallback = wdtCallback;
}

/*******************************************************************************
Starts Calculating WDT TC Compare Match Interval
Also sets the wdt_tc_interval
Parameters:
  interval - interval.
Returns:
  rt_value - Compare Match value.
*******************************************************************************/
uint16_t halCalcWdtTimerVal(HAL_WdtInterval_t interval)
{
  uint16_t rt_value;
  // The Formula is ((2^((interval-1)+3))/(2^10))
  uint32_t comp = ((1<<((interval-1)+3))>>10);
  if (comp)
  {
    wdt_tc_interval = comp;
    rt_value =  WDT_TC_COUNT;
  }
  else
  {
    wdt_tc_interval = 1;
    // The Formula is (WDT_TC_COUNT/((2*(10 - ((interval - 1)+3)))))
    rt_value = (WDT_TC_COUNT>>(10-((interval-1)+3)));
  }
  return rt_value;
}

/*******************************************************************//**
\brief Resets the WDT TC Counter
***********************************************************************/
void wdtTCReset(void)
{
  if (halWdtCallback)
  {
    wdtTCSync();
    TC5_16_COUNT = (uint16_t)0;
  }
}

/*******************************************************************//**
\brief Stops the WDT TC Counter
***********************************************************************/
void wdtTCStop(void)
{
  if (halWdtCallback)
  {
    wdtTCSync();
    TC5_16_CTRLA_s.enable = 0; // stop wdt timer
  }
}

/*******************************************************************//**
\brief Enables the WDT TC Counter
***********************************************************************/
void wdtTCEnable(void)
{
  if (halWdtCallback)
  {
    wdtTCSync();
    TC5_16_CTRLA_s.enable = 1; // enable wdt timer
  }
}

/*******************************************************************************
Starts WDT TC timer interval.
Parameters:
  interval - interval.
Returns:
  none.
*******************************************************************************/
void wdtTCStart(HAL_WdtInterval_t interval)
{
  PM_APBCMASK_s.tc5 = 1;
  
  wdtTCSync();
  TC5_16_CTRLA = TC5_16_CTRLA_MODE(0) | TC5_16_CTRLA_WAVEGEN(1) \
                 | TC5_16_CTRLA_PRESCALER(7) ;

  wdtTCSync();
  TC5_16_COUNT = 0;
  wdtTCSync();
  TC5_16_CC0 = halCalcWdtTimerVal(interval);

  wdtTCSync();
  TC5_16_CTRLBCLR = TC5_16_CTRLBCLR_ONESHOT | TC5_16_CTRLBCLR_DIR;

  wdtTCSync();
  TC5_16_INTENSET = TC5_16_INTENSET_MC0; // enabling interrupt MC0

  NVIC_EnableIRQ(TC5_IRQn);
}

/*******************************************************************************
Starts WDT with interval.
Parameters:
  interval - interval.
Returns:
  none.
*******************************************************************************/
void HAL_StartWdt(HAL_WdtInterval_t interval)
{

  GCLK_CLKCTRL_s.id = 0x03; // wdt clock
  GCLK_CLKCTRL_s.gen = 4; // Generic clock generator 4
  GCLK_CLKCTRL_s.clken = 1;

  ATOMIC_SECTION_ENTER
  BEGIN_MEASURE
  while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
  WDT_CTRL_s.enable = 0; // Disabling watchdog
  WDT_CTRL_s.wen = 0; // disabling window mode
  WDT_CTRL_s.alwayson = 0; // disabling alwayson mode

  WDT_INTENCLR_s.ew = 1;
#if !defined(HAL_USE_USB)
  if (halWdtCallback)
  {
#endif
    WDT_INTENSET_s.ew = 1;
    WDT_EWCTRL_s.ewoffset = interval;
    NVIC_EnableIRQ(WDT_IRQn);
    wdtTCStart(interval);
#if !defined(HAL_USE_USB)
  }
  else
  {
    WDT_CONFIG_s.per = interval;
  }
#endif

  while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
  WDT_CTRL_s.enable = 1; // Enabling watchdog
  wdtTCEnable(); // enabling wdt TC timer
  END_MEASURE(HALATOM_WDT_START_MEASURE_CODE)
  ATOMIC_SECTION_LEAVE

}

/*******************************************************************//**
\brief Restart the Watch Dog timer

Note Restart the watchdog timer.
***********************************************************************/
void halRestartWdt(void)
{
  while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
  WDT_CLEAR_s.clear = 0xA5;
  wdtTCReset();
}

/*******************************************************************//**
\brief Resets the WDT Counter
************************************************************************/
void wdt_reset(void)
{
  if(!(WDT_STATUS & WDT_STATUS_SYNCBUSY))
  {
    WDT_CLEAR_s.clear = 0xA5;
    wdtTCReset();
    wdt_count = 0;
  }
  else if(wdt_count >= wdt_tc_interval)
  {
    while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
    WDT_CLEAR_s.clear = 0xA5;
    wdt_count = 0;
    wdtTCReset();
  }
}

/*******************************************************************//**
\brief Stops the Watch Dog timer

Note that function is empty for AT91SAM7X256. WDT for AT91SAM7X256 is
impossible to stop.
***********************************************************************/
void HAL_StopWdt(void)
{
  while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
  WDT_CTRL_s.enable = 0;
  wdtTCStop();
}

/*******************************************************************************
Interrupt handler for WDT TC Handler
*******************************************************************************/
void wdtTCHandler(void)
{
  if (TC5_16_INTFLAG_s.mc0 & TC5_16_INTENSET_s.mc0)
  {
    TC5_16_INTFLAG_s.mc0 = 1;
    wdt_count++;
  }
}

/*******************************************************************************
Interrupt handler for WDT Handler
*******************************************************************************/
void wdtTimerHandler(void)
{
  while(WDT_STATUS & WDT_STATUS_SYNCBUSY);
  WDT_CTRL_s.enable = 0;

  if (halWdtCallback)
    halWdtCallback();

#if defined(HAL_USE_USB)
  /* Perform the following reset sequence only if USB is conneted and enabled 
  to avoid re enumeration (core reset)
  */
  if(USB_CTRLA & USB_CTRLA_ENABLE)
  {
    volatile uint32_t stack_pointer =0;
    void (*halresethandler) (void) = &HAL_WarmReset;
    HAL_Delay(0xff);
    HAL_Delay(0xff);
    HAL_Delay(0xff);
    HAL_Delay(0xff);
    HAL_StopWdt();
    WDT_INTFLAG_s.ew = 1;

    /* After warm reset, Power manager's RCAUSE register will not give result as WDT 
    reset. So the following method is used to save the reset reason as WDT reset in one 
    of the USB register which can be used to retrive the reset reason as WDT.
    */
    USB_DESCADD = WDT_USB_WARM_RESET;

    /* Modify the return address(LR content) in stack to point to 
    HAL_WarmReset Handler to avoid core reset (re enumeration).
    */
    /* Note : The magic number 0x2c gives the offset of Link register value (from the 
    current stack pointer) which gets loaded back to LR on ISR exit.
    This offset may vary on any changes in this function. Developer need to make sure 
    this offset is pointing to LR content in the stack
    */
    #ifdef  __IAR_SYSTEMS_ICC__
      stack_pointer = __get_SP() + 0x30;
    #elif __GNUC__
      stack_pointer = __get_MSP() + 0x30;
    #endif
    memcpy((uint8_t *)stack_pointer, (uint8_t *)&halresethandler, 4);
  }
  else
#endif
  {
    WDT_INTENCLR_s.ew = 0;
    WDT_CONFIG_s.per = 0;
    WDT_CTRL_s.enable = 1;
    for (;;);
  }
}

#endif // defined(HAL_USE_WDT)

#if defined(__GNUC__) && defined(_STATS_ENABLED_)
//void halFillStack(void) __attribute__ ((naked, section (".init1")));
/**************************************************************************//**
\brief Fill cstack with repeated pattern 0x0A0B0C0D
******************************************************************************/

void halFillStack(void)
{
  extern unsigned int __stack_start;
  extern unsigned int __stack;

  unsigned char *start =0;
  volatile int stackLeft = (volatile int) Stat_GetCurrentStackLeft();
  stackLeft -= 16;

  for (start = (unsigned char *)&__stack_start; (start <= ((unsigned char *)&__stack - STACK_INIT_MARGIN) && (stackLeft > 0)); start+=4)
  {
    *(unsigned int *)start = 0x0A0B0C0DU;
    stackLeft -=4;
  }
}

#elif defined (__ICCARM__) && defined(_STATS_ENABLED_)
#pragma section = "CSTACK"
/**************************************************************************//**
\brief Fill cstack with repeated pattern 0x0A0B0C0D
******************************************************************************/
void halFillStack(void)
{
  volatile unsigned char *start;
  unsigned int stackLeft =  Stat_GetCurrentStackLeft();
  for (start = ((unsigned char *)__segment_begin("CSTACK")); (start <= (((unsigned char *)__segment_end("CSTACK"))- STACK_INIT_MARGIN) && stackLeft); start+=4)
  {
    *(unsigned int *)start = 0x0A0B0C0DU;
    stackLeft -=4;
  }
}

#endif

//eof halWdt.c
