/**************************************************************************//**
\file  halAppClock.c

\brief Implementation of appTimer hardware-dependent module.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <halAppClock.h>
#include <halMacIsr.h>
#include <halDbg.h>
#include <halInterrupt.h>
#include <sysEvents.h>
#include <halSleepTimerClock.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
// the main cpu clock
#if defined(HAL_4MHz) || defined(HAL_8MHz) || defined(HAL_16MHz) || defined(HAL_24MHz) || defined(HAL_32MHz) || defined(HAL_48MHz)
  #define AMOUNT_TIMER_CLOCK_IN_ONE_USEC       F_PERI/TIMER_FREQUENCY_PRESCALER/1000000ul
#endif

/******************************************************************************
                   Prototypes section
******************************************************************************/
/******************************************************************************
  \brief Returns the time left value for the smallest app timer.
  \return time left for the smallest application timer.
******************************************************************************/
uint32_t halGetTimeToNextAppTimer(void);
static void smallestTimerRequestHandler(SYS_EventId_t eventId, SYS_EventData_t data);

/******************************************************************************
                     Global variables section
******************************************************************************/
static uint32_t halAppTime = 0ul;     // time of application timer
uint8_t halAppTimeOvfw = 0;
static volatile uint8_t halAppIrqCount = 0;

static SYS_EventReceiver_t appTimerEventListener = {.func = smallestTimerRequestHandler};

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
 Polling the Sync. flag for register access
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halTimerSync(void)
{
  while (TC3_16_STATUS_s.syncbusy);
}
/******************************************************************************
 Interrupt handler signal implementation
 Parameters:
   none
 Returns:
   none
 *****************************************************************************/
INLINE void halInterruptAppClock(void)
{
  halAppIrqCount++;
  halPostTask(HAL_APPTIMER);
  // infinity loop spy
  SYS_InfinityLoopMonitoring();
}

/**************************************************************************//**
\brief Takes account of the sleep interval.

\param[in]
  interval - time of sleep
******************************************************************************/
void halAdjustSleepInterval(uint32_t interval)
{
  halAppTime += interval;
  halPostTask(HAL_APPTIMER);
}

/**************************************************************************//**
Synchronization system time which based on application timer.
******************************************************************************/
void halAppSystemTimeSynchronize(void)
{
  uint8_t tmpCounter;
  uint32_t tmpValue;

  ATOMIC_SECTION_ENTER
    tmpCounter = halAppIrqCount;
    halAppIrqCount = 0;
  ATOMIC_SECTION_LEAVE

  tmpValue = tmpCounter * HAL_APPTIMERINTERVAL;
  halAppTime += tmpValue;
  if (halAppTime < tmpValue)
    halAppTimeOvfw++;
}

/******************************************************************
time counter interrupt handler
 Parameters:
   none
 Returns:
   none
******************************************************************/
void timerHandler(void)
{
  if (TC3_16_INTFLAG_s.mc0 & TC3_16_INTENSET_s.mc0)
  {
    // appTimer handling
    TC3_16_INTFLAG_s.mc0 = 1;
    halTimerSync();
    halInterruptAppClock();
  }
}

/******************************************************************
configure and enable timer counter channel 0
 Parameters:
   none
 Returns:
   none
******************************************************************/
void halStartAppClock(void)
{
  PM_APBCMASK_s.tc3 = 1;

  GCLK_CLKCTRL_s.id = 0x1b;  // enabling clock for TC3
  GCLK_CLKCTRL_s.gen = 3;
  GCLK_CLKCTRL_s.clken = 1;

  TC3_16_CTRLA_s.mode = 0x00;  //16 bit mode
  halTimerSync();
  TC3_16_CTRLA_s.wavegen = 1;  // match frequency
  halTimerSync();
  TC3_16_CTRLA_s.prescaler = 3;  // Prescaler DIV2
  halTimerSync();
  TC3_16_CTRLA_s.prescsync = 0x01; // PRESC
  halTimerSync();
  TC3_16_CTRLBCLR_s.oneshot = 1; // clearing one shot mode
  halTimerSync();
  TC3_16_CTRLBCLR_s.dir = 1;
  halTimerSync();

  TC3_16_COUNT = (uint16_t)0;
  halTimerSync();
  TC3_16_CC0 = (uint16_t)TOP_TIMER_COUNTER_VALUE;  // 30000
  halTimerSync();

  TC3_16_CTRLA_s.enable = 1; //after configuration enable the timer
  halTimerSync();

  TC3_16_INTENSET_s.mc0 = 1; // enabling interrupt MC0
  halTimerSync();

  NVIC_EnableIRQ(TC3_IRQn);
 
  SYS_SubscribeToEvent(BC_EVENT_MAX_SLEEP_TIME_REQUEST, &appTimerEventListener);
}

/******************************************************************
disable timer
 Parameters:
   none
 Returns:
   none
******************************************************************/
void halStopAppClock(void)
{
  /* Disable the clock */
  NVIC_DisableIRQ(TC3_IRQn);
  TC3_16_INTENCLR_s.mc0 = 1; // disabling interrupt MC0
  halTimerSync();
  TC3_16_CTRLA_s.enable = 0;
  halTimerSync();
}

/******************************************************************************
Return time of sleep timer.

Returns:
  time in ms.
******************************************************************************/
uint32_t halGetTimeOfAppTimer(void)
{
  halAppSystemTimeSynchronize();
  return halAppTime;
}

/**************************************************************************//**
\brief System clock.

\return
  system clock in Hz.
******************************************************************************/
uint32_t HAL_ReadFreq(void)
{
  return (uint32_t)F_CPU;
}

/******************************************************************************
 Delay in microseconds.
 Parameters:
   us - delay time in microseconds
******************************************************************************/
void halDelayUs(uint16_t us)
{
  uint32_t startCounter;
  uint32_t delta;

  us *= AMOUNT_TIMER_CLOCK_IN_ONE_USEC;
  // begin counter meaning
  startCounter = TC3_16_COUNT;
  // different between compare regitser and current counter
  delta = TC3_16_CC0 - startCounter;

  if (delta > us)
  {
    while ((TC3_16_COUNT - startCounter) < us);
  }
  else
  {
    us -= delta;
    while ((TC3_16_COUNT > startCounter) || (TC3_16_COUNT < us));
  }

}
/**************************************************************************//**
  \brief Processes BC_EVENT_MAX_SLEEP_TIME_REQUEST request

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
 ******************************************************************************/
static void smallestTimerRequestHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  uint32_t *minValue = (uint32_t *)data;
  uint32_t timeToFire = halGetTimeToNextAppTimer();

  if (*minValue <= HAL_MIN_SLEEP_TIME_ALLOWED)
  {
    *minValue = 0;
    return;
  }

  if (*minValue > timeToFire)
  {
    if (timeToFire > HAL_MIN_SLEEP_TIME_ALLOWED)
      *minValue = timeToFire;
    else
      *minValue = 0;
  }
  (void)eventId;
}


// eof halAppClock.c
