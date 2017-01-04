/************************************************************************//**
  \file halIrq.c

  \brief
    HWD interface for external IRQs

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
#include <atsamr21.h>
#include <core_cm0plus.h>
#include <halIrq.h>
#include <halInterrupt.h>
#include <atomic.h>
#include <halDbg.h>
#include <sleep.h>
#include <halSleepTimerClock.h>

/******************************************************************************
                        Defines section
******************************************************************************/
#define HAL_NUMBER_OF_IRQS  16

/******************************************************************************
                             Types section
******************************************************************************/
static void halEicSyncing();
// We introduce system and user handlers for external IRQs to
// allow using them for stack needs (e.g. RF IRQ)
// Only user handlers (and IRQ identifiers) should be visible to user

typedef struct _HalExtIrqHandler_t
{
  uint16_t trigs;     // Bitmask of pins to trigger callback
  IrqCallback_t cb[16];   // Callback on IRQ firing
} HalExtIrqHandler_t;

typedef struct _halExtIrq_t
{
  HalExtIrqHandler_t irqInd;      // IRQ handler
  volatile uint32_t delayedTrigs; // Bitmask of triggered but not processed yet pins
} HalExtIrq_t;

typedef enum _HalExtIrqSense_t
{
  NONE,
  RISE,
  FALL,
  BOTH,
  HIGH,
  LOW
} HalExtIrqSense_t;

/******************************************************************************
                    Static functions prototypes section
******************************************************************************/
static inline void halProcessExtIrq(HalExtIrq_t *me);

/******************************************************************************
                    Local variables section
******************************************************************************/
static HalExtIrq_t halExtIrq;;

/******************************************************************************
                        Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Process external IRQ (to be called only from ISR)

\param[in] me - IRQ context
******************************************************************************/
static inline void halProcessExtIrq(HalExtIrq_t *me)
{
  uint16_t trigs = me->delayedTrigs;
  uint16_t enabledTrigs = EIC_INTENSET_s.extint;
  uint16_t wakeupTrigs = EIC_WAKEUP_s.wakeupen;
  uint8_t id = 0 ;
  // Append new triggers to delayed ones (and clear IRQ by the way)
  trigs |= EIC_INTFLAG_s.extint;
  EIC_INTFLAG_s.extint = 0xFFFF;
  // Store disabled (but used) triggers for later processing
  me->delayedTrigs = trigs & ~enabledTrigs;
  // Leave only triggers we interested at all
  trigs &= me->irqInd.trigs;
  // Process enabled triggers now
  trigs &= enabledTrigs;
  wakeupTrigs = trigs & wakeupTrigs;
  // Execute registered callbacks for matching triggers

  if (trigs)
  {
     for(; trigs!=1;trigs >>= 1,id++);
     me->irqInd.cb[id]();
     /* wake up call for MCU */
     if (wakeupTrigs)
       halWakeupFromIrq();
  }
}

/**************************************************************************//**
\brief Register new handler for external IRQ and do necessary setup

\param[in] id - IRQ id
\param[in] mode - array of triggers each pin
\param[in] cb - callback on IRQ firing
\return 0 if there was an error (handler already registered or collision of
  triggers for different handlers), other value otherwise
******************************************************************************/
uint32_t halRegisterExtIrq(uint32_t id, const HAL_IrqMode_t mode, IrqCallback_t cb)
{
  HalExtIrq_t *ind = &halExtIrq;
  uint16_t eicIntenset = 0;
  uint32_t eicConfiguration[2] = {0,0};

  // Check if callback is already registered
  if (ind->irqInd.cb[id])
    return 0;

  eicIntenset |= 1ul << id;

  switch (mode)
  {
    case IRQ_ANY_EDGE:
      eicConfiguration[id/8] |= BOTH << (4 * (id%8)) ;
      break;

    case IRQ_FALLING_EDGE:
      eicConfiguration[id/8] |= FALL << (4 * (id%8)) ;
      break;

    case IRQ_RISING_EDGE:
      eicConfiguration[id/8] |= RISE << (4 * (id%8));
      break;

    case IRQ_LOW_LEVEL:
      eicConfiguration[id/8] |= LOW << (4 * (id%8));
      break;

    case IRQ_HIGH_LEVEL:
      eicConfiguration[id/8] |= HIGH << (4 * (id%8));
      break;

    case IRQ_IS_DISABLED:
    default:
      break;
  }

  sysAssert(cb, EXT_IRQ_NULLCALLBACK0);
  // Register callback and triggers
  ind->irqInd.cb[id] = cb;
  ind->irqInd.trigs |= eicIntenset;

  // Disable the IRQ
  NVIC_DisableIRQ(EIC_IRQn);

  // Disable the external interrupt
  EIC_CTRL_s.enable = 0;

  // Enable CLK_EIC_APB
  PM_APBAMASK_s.eic = 1;

  // Enable GCLK_EIC
  GCLK_CLKCTRL_s.id = 0x05;
  GCLK_CLKCTRL_s.gen = 2;
  GCLK_CLKCTRL_s.clken = 1;

  //EIC_INTENSET_s.extint |= eicIntenset;
  EIC_CONFIG0 |= eicConfiguration[0];
  EIC_CONFIG1 |= eicConfiguration[1];

  EIC_INTENSET_s.extint = (1 << id);
  if (id)  //wakeup enable for all ext int except rf Irq
    EIC_WAKEUP_s.wakeupen |= (1 << id);

  // Enable the external interrupt
  EIC_CTRL_s.enable = 1;
  halEicSyncing();

  // Enable the IRQ
  NVIC_EnableIRQ(EIC_IRQn);

  return 1;
}

/**************************************************************************//**
\brief Unregister handler for external IRQ

\param[in] id - IRQ id
\return 0 if handler was not registered, other value otherwise
******************************************************************************/
uint32_t halUnregisterExtIrq(uint32_t id)
{
  HalExtIrq_t *ind = &halExtIrq;
  uint16_t clrMask = ind->irqInd.trigs;
  uint16_t delayedTrigs;

  if (!ind->irqInd.cb[id]) // No callback is registered
    return 0;

  ATOMIC_SECTION_ENTER
    // Remove triggers for current handler from delayed triggers
    // and don't track them anymore
    delayedTrigs = (ind->delayedTrigs | EIC_INTFLAG) & ~clrMask;
    ind->delayedTrigs = delayedTrigs;

    // There is a small chance NVIC may miss request. Be on a safe side and
    // request IRQ manually.
    if (delayedTrigs & EIC_INTENSET)
      NVIC_SetPendingIRQ(EIC_IRQn);

    ind->irqInd.trigs &= ~(1 << id);
    ind->irqInd.cb[id] = NULL;
  ATOMIC_SECTION_LEAVE

  // Set pins to default interrupt mode
  EIC_INTENCLR_s.extint = 1 << id;

  return 1;
}

/**************************************************************************//**
\brief Enable external IRQ

\param[in] id - IRQ id
******************************************************************************/
void halEnableExtIrq(uint32_t id)
{
  HalExtIrq_t *ind = &halExtIrq;

  EIC_CTRL_s.enable = 1;
  halEicSyncing();
  EIC_INTENSET_s.extint = ((1 << id) & (ind->irqInd.trigs));
  NVIC_EnableIRQ(EIC_IRQn);
}

/**************************************************************************//**
\brief Disable external IRQ

\param[in] id - IRQ id
\return 0 if IRQ was already disabled, other value otherwise
******************************************************************************/
uint32_t halDisableExtIrq(uint32_t id)
{
  HalExtIrq_t *ind = &halExtIrq;

  if ( EIC_INTENSET_s.extint &  (1 << id )) //ind->irqInd.trigs)
  {
    EIC_INTENCLR_s.extint = ((1 << id) & (ind->irqInd.trigs) );
    return 1;
  }
  EIC_WAKEUP_s.wakeupen &= ~(1 << id);

  return 0;
}

/**************************************************************************//**
\brief Check if user handler for external IRQ is already registered

\param[in] id - IRQ id
\return 0 if not registered, 1 otherwise
******************************************************************************/
uint32_t halIsUserExtIrqRegistered(uint32_t id)
{
  return halExtIrq.irqInd.cb[id] != NULL;
}

/**************************************************************************//**
\brief Check if id for external IRQ is in valid range for user

\param[in] id - IRQ id
\return 0 if not valid, 1 otherwise
******************************************************************************/
uint32_t halIsUserExtIrqValid(uint32_t id)
{
  return id < HAL_NUMBER_OF_IRQS;
}
/**************************************************************************//**
\brief Clear pending external IRQ

\param[in] id - IRQ id
******************************************************************************/
void halClearExtIrq(uint32_t id)
{
  HalExtIrq_t *ind = &halExtIrq;
  uint32_t trigs = ind->irqInd.trigs;
  uint32_t delayedTrigs;

  // Remember triggers for other handler type to process them later, discard triggers for current type
  ATOMIC_SECTION_ENTER
    delayedTrigs = (ind->delayedTrigs | EIC_INTFLAG_s.extint) & ~trigs;
    ind->delayedTrigs = delayedTrigs;

    // There is a small chance NVIC may miss request. Be on a safe side and
    // request IRQ manually.
    if (delayedTrigs & EIC_INTENSET_s.extint)
      NVIC_SetPendingIRQ(EIC_IRQn);

    EIC_INTFLAG_s.extint |= (1<< id);

  ATOMIC_SECTION_LEAVE
}
/**************************************************************************//**
\brief EIC ISR
******************************************************************************/
void eicHandler(void)  //commented acurrently as conflicting with Rf IRQ implemented in spiRfCtr.c
{
  halProcessExtIrq(&halExtIrq);
}
/*****************************************************************************
function to check the EIC BUSY status
*****************************************************************************/
static void halEicSyncing()
{
  while (EIC_STATUS & EIC_STATUS_SYNCBUSY);
}
// eof halIrq.c
