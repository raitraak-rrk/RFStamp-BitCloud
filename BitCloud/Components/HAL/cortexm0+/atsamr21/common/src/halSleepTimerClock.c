/**************************************************************************//**
  \file  halSleepTimer.c

  \brief Module for count out requested sleep interval.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      08/05/14 ViswanadhaM kotla -  Created
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/
/******************************************************************************
                   Includes section
******************************************************************************/
#include <atsamr21.h>
#include <sleepTimer.h>
#include <halSleepTimerClock.h>
#include <halSleep.h>
#include <halDbg.h>
#include <halDiagnostic.h>
#include <halInterrupt.h>
#include <gpio.h>
#include <halClkCtrl.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/

#define SLEEP_TIMER_ITERATOR  (1000ul * 256ul * SLEEPTIMER_DIVIDER / SLEEPTIMER_CLOCK)
#define NVMCTRL_OSC32K_FUSE_CALIB_ADDR  (NVMCTRL_OTP4 + 4)
#define NVMCTRL_OSC32K_FUSE_CALIB_MASK  (0x1fc0)
#define NVMCTRL_OSC32K_FUSE_CALIB_Pos   (6)

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Prototypes section
******************************************************************************/
void rtcHandler(void);
static void halRtcWaitForSync(void);

/******************************************************************************
                   External global variables section
******************************************************************************/
extern HAL_SleepControl_t halSleepControl;

/******************************************************************************
                   Global variables section
******************************************************************************/
volatile uint32_t halSleepRemainder;
 //upper byte of sleep time
uint8_t halSleepTimerOvfw = 0;
 //interrupt counter
static volatile uint8_t halIrqOvfwCount = 0;
// time of sleep timer in ms.
static uint32_t halSleepTime = 0ul;

/******************************************************************************
                   Implementations section
******************************************************************************/
void halStartSleepTimerClock(void)
{
#if defined(RC_32K)
  // Configure Internal Osc32K as input source
  SYSCTRL_OSC32K_s.runstdby = 1;
  SYSCTRL_OSC32K_s.ondemand = 0;
  SYSCTRL_OSC32K_s.en32k = 1;
  SYSCTRL_OSC32K_s.enable = 1;
  SYSCTRL_OSC32K_s.calib = (((*(uint32_t *)NVMCTRL_OSC32K_FUSE_CALIB_ADDR) \
                             & NVMCTRL_OSC32K_FUSE_CALIB_MASK) >> NVMCTRL_OSC32K_FUSE_CALIB_Pos);

  while(SYSCTRL_PCLKSR_s.osc32krdy == 0);

  GCLK_GENDIV = GCLK_GENDIV_ID(GCLK_GEN_2) | GCLK_GENDIV_DIV(0);
  halGclkSync();

  GCLK_GENCTRL = GCLK_GENCTRL_ID(GCLK_GEN_2) | GCLK_GENCTRL_SRC(OSC32K) | GCLK_GENCTRL_GENEN;
  halGclkSync();

#elif defined (CRYSTAL_32K)
#if defined(HAL_4MHz) || defined(HAL_8MHz) || defined(CLKM)
#ifndef XOSC32K
/*
The XOSC32K is defined in halClkCtrl.c
We call halXOSC32KInit() here because as of now to generate 4MHz and 8MHz
we use the internal OSC8M and we don't initialize the XOSC32K
*/
  halXOSC32KInit();
#endif
#endif

  GCLK_GENDIV = GCLK_GENDIV_ID(GCLK_GEN_2) | GCLK_GENDIV_DIV(0);
  halGclkSync();

  GCLK_GENCTRL = GCLK_GENCTRL_ID(GCLK_GEN_2) | GCLK_GENCTRL_SRC(EXOSC32K) | GCLK_GENCTRL_GENEN;
  halGclkSync();

#endif

 /* Enable RTC APB CLK in the power manager */ 
  PM_APBAMASK |= (1 << 5);

  /* Enable the generic clk to clk the RTC */
  GCLK_CLKCTRL = GCLK_CLKCTRL_ID(GCLK_RTC) | GCLK_CLKCTRL_GEN(GCLK_GEN_2) | GCLK_CLKCTRL_CLKEN;


  RTC_M0_CTRL_s.swrst = 1; //reset the RTC
  halRtcWaitForSync();

  RTC_M0_CTRL_s.enable = 0;
  halRtcWaitForSync();

  RTC_M0_CTRL_s.mode = 0;   //32bit counter mode
  RTC_M0_CTRL_s.prescaler = SLEEPTIMER_PRESCALER;

  RTC_M0_COUNT_s.count = 0;
  halRtcWaitForSync();

  RTC_M0_CTRL_s.enable = 1;
  halRtcWaitForSync();
  
}

void halStopSleepTimerClock(void)
{
    RTC_M0_CTRL_s.enable = 0;
    halRtcWaitForSync();
}

/******************************************************************************
Sets interval.
Parameters:
  value - contains number of ticks which the timer must count out.
Returns:
  none.
******************************************************************************/
void halSetSleepTimerInterval(uint32_t value)
{
  uint32_t currCounter = 0;
  uint32_t remainingTimeBeforeOvf;

  /* Disable and clear the interrupt flags */
  RTC_M0_INTENCLR_s.ovf = 0;
  RTC_M0_INTENCLR_s.cmp0 = 0;
  RTC_M0_INTFLAG_s.cmp0 = 1;

  halRtcWaitForSync();
  currCounter = RTC_M0_COUNT_s.count;

  remainingTimeBeforeOvf = ~currCounter;

  if (remainingTimeBeforeOvf > value)
  {
    value += currCounter;
    RTC_M0_COMP0_s.comp = value;
    halRtcWaitForSync();

    /* Enable compare match interrupt */
    RTC_M0_INTENSET_s.cmp0 = 1;  
    halSleepRemainder = 0;
  }
  else // overflow is going to be occured
  {
    value -= remainingTimeBeforeOvf;
    halSleepRemainder = value;
    /* Enable the ovf interrupt */ 
    RTC_M0_INTENSET_s.ovf = 1;
  }

  HAL_InstallInterruptVector(RTC_IRQn, rtcHandler);

  /* Clear & disable RTC interrupt on NVIC */
  NVIC_DisableIRQ(RTC_IRQn);
  NVIC_ClearPendingIRQ(RTC_IRQn); 

  /* set priority & enable RTC interrupt on NVIC */
  NVIC_EnableIRQ(RTC_IRQn);
}

/******************************************************************
\brief wait for RTC module to sync
********************************************************************/
static void halRtcWaitForSync(void)
{
  while (RTC_M0_STATUS_s.syncbusy == 1);
}

/******************************************************************
\brief RTC interrupt handler
********************************************************************/
void rtcHandler(void)
{
  uint8_t intFlags;

  intFlags = RTC_M0_INTFLAG & RTC_M0_INTENSET;

  if (intFlags & RTC_M0_INTFLAG_OVF) //ovf occured
  {
    RTC_M0_INTFLAG_s.ovf = 1;
    if (0 == halSleepRemainder)
    {
      if (HAL_SLEEP_MODE == halSleepControl.wakeupStation)
        halPowerOn(HAL_SLEEP_TIMER_IS_WAKEUP_SOURCE);
      // post task for task manager
      if (HAL_SLEEP_TIMER_IS_STARTED == halSleepControl.sleepTimerState)
        halInterruptSleepClock();
    }
    else
    {
      // load compared value
      halRtcWaitForSync();
      RTC_M0_COMP0_s.comp = halSleepRemainder;
          
      // clear compare interrupt flag
      RTC_M0_INTENCLR_s.cmp0 = 0;
      // enable compare match interrupt
      RTC_M0_INTENSET_s.cmp0 = 1;
      if (HAL_SLEEP_MODE == halSleepControl.wakeupStation)
        HAL_Sleep();
    }
    halIrqOvfwCount++;
    halSynchronizeSleepTime();
  }

  if (intFlags & RTC_M0_INTFLAG_CMP0) //comp match int ocuured
  {
    RTC_M0_INTFLAG_s.cmp0 = 1;
    /* Disable the interrupt flags */
    RTC_M0_INTENCLR_s.cmp0 = 0;
    // nulling for adjusting
    halSleepRemainder = 0;
    
    if (HAL_SLEEP_MODE == halSleepControl.wakeupStation)
      halPowerOn(HAL_SLEEP_TIMER_IS_WAKEUP_SOURCE);
    // post task for task manager
    if (HAL_SLEEP_TIMER_IS_STARTED == halSleepControl.sleepTimerState)
      halInterruptSleepClock();
  }
}

/**************************************************************************//**
\brief Wake up procedure for all external interrupts
******************************************************************************/
void halWakeupFromIrq(void)
{
  if (HAL_SLEEP_MODE == halSleepControl.wakeupStation)
  {
    halPowerOn(HAL_EXT_IRQ_IS_WAKEUP_SOURCE);

    /* clear the remainder value */
    halSleepRemainder = 0;

    /* Disable the ovf interrupt */
    RTC_M0_INTENCLR_s.cmp0 = 1;

    // stop high sleep timer logic
    halSleepControl.sleepTimerState = HAL_SLEEP_TIMER_IS_STOPPED;
  }
}

/******************************************************************************
Return time of sleep timer.

Returns:
  time in ms.
******************************************************************************/
uint64_t halGetTimeOfSleepTimer(void)
{
  uint32_t currCounter;
  uint32_t tmpCounter;
  uint64_t  countValue;

  ATOMIC_SECTION_ENTER
  // read interrupt counter
  tmpCounter = halIrqOvfwCount;
  ATOMIC_SECTION_LEAVE

  countValue = (uint64_t)tmpCounter << 32;

  // read asynchronous counter 
  halRtcWaitForSync();
  currCounter = RTC_M0_COUNT_s.count;  
  countValue += currCounter;  

  return (halSleepTime + countValue);
}

/******************************************************************************
Returns the sleep timer frequency in Hz.
Parameters:
  none.
Returns:
  frequency.
******************************************************************************/
uint32_t halSleepTimerFrequency(void)
{
  return (SLEEPTIMER_CLOCK / SLEEPTIMER_DIVIDER);
}

/**************************************************************************//**
Synchronization system time which based on sleep timer.
******************************************************************************/
void halSleepSystemTimeSynchronize(void)
{
  uint8_t tmpCounter;
  uint32_t tmpValue;

  ATOMIC_SECTION_ENTER
  tmpCounter = halIrqOvfwCount;
  halIrqOvfwCount = 0;  
  ATOMIC_SECTION_LEAVE

  tmpValue = tmpCounter * SLEEP_TIMER_ITERATOR;
  halSleepTime += tmpValue;
  if (halSleepTime < tmpValue)
    halSleepTimerOvfw++;
}
//eof halSleepTimerClock.c

