/**************************************************************************//**
\file  halVectorTable.c

\brief Cortex-M0+ vector table.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2012, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
    14/05/14 karthik.p_u - Modified
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <atsamr21.h>
#include <halInterrupt.h>
#include <sysUtils.h>

/******************************************************************************
                   Define(s) section
******************************************************************************/
#if defined(HAL_USE_UART)
#  define HAL_UART0_IRQ_HANDLER       uart0Handler
#  define HAL_UART1_IRQ_HANDLER       uart1Handler
#else
#  define HAL_UART0_IRQ_HANDLER       halIrqHandlerNotUsed
#  define HAL_UART1_IRQ_HANDLER       halIrqHandlerNotUsed
#endif

#if defined(HAL_USE_USB)
#  define HAL_USB_IRQ_HANDLER         usb_device_interrupt_handler
#else
#  define HAL_USB_IRQ_HANDLER         halIrqHandlerNotUsed
#endif

#if defined(HAL_USE_WDT)
#  define HAL_WDT_IRQ_HANDLER         wdtTimerHandler
#  define HAL_WDT_TC_IRQ_HANDLER      wdtTCHandler
#else
#  define HAL_WDT_IRQ_HANDLER         halIrqHandlerNotUsed
#  define HAL_WDT_TC_IRQ_HANDLER      halIrqHandlerNotUsed
#endif

/*******************************************************************************
                   Prototypes section
*******************************************************************************/
extern void halLowLevelInit(void);
extern void timerHandler(void);
extern void eicHandler(void);
extern void wdtTimerHandler(void);
extern void wdtTCHandler(void);
static void hardFaultHandler(void);
static void halIrqHandlerNotUsed(void);
extern void usart0Handler(void);
extern void usart1Handler(void);
extern void halMacTimerHandler(void);
extern void rtcHandler(void);
extern void PTC_Handler(void);
extern void usb_device_interrupt_handler (void);

/*******************************************************************************
                     External variables section
*******************************************************************************/
/* Initialize segments */

extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

#pragma weak PTC_Handler                = halIrqHandlerNotUsed
/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);
/******************************************************************************
                              Constants section
******************************************************************************/

// Initial constant vector table. Used at startup only. After reset, vector table is
// relocated (remapped) to a runtime-configurable __ram_vector_table. (located in RAM)
// IAR really wants this variable to be named __vector_table. User-configurable interrupts
// are omitted, since they should not be used until remap and system init .
#ifdef __IAR_SYSTEMS_ICC__
  #pragma section = ".vectors"
  #pragma location = ".vectors"
#else
  __attribute__ ((section(".vectors")))
#endif
const HalIntVector_t __vector_table[] =
{
    {.__ptr = STACK_TOP},
    {RESET_ADDR},

    {0},
    {hardFaultHandler},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},   /* Reserved */
    {0},
    {0}, {0},                      /*  Reserved */
    {0},
    {0}
};

// Real RAM-based vector table. Some positions are already populated by BitCloud handlers
#ifdef  __IAR_SYSTEMS_ICC__
  #pragma section = ".ramvectors"
  #pragma location = ".ramvectors"
#else
  __attribute__ ((section(".ramvectors"), used))
#endif
HalIntVector_t __ram_vector_table[] =
{
    {.__ptr = STACK_TOP},
    {RESET_ADDR},

    {0},
    {hardFaultHandler},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},   /* Reserved */
    {0},
    {0}, {0},                      /*  Reserved */
    {0},
    {0},

    {halIrqHandlerNotUsed},        /*  0  POWER MANAGER (PM) */
    {halIrqHandlerNotUsed},        /*  1  SYSTEM CONTROL (SYSCTRL) */
    {HAL_WDT_IRQ_HANDLER},        /*  2  WATCHDOG TIMER */
    {rtcHandler},        /*  3  REAL TIME CLOCK */
    {eicHandler},        /*  4  EXTERNAL INTERRUPT CONTROLLER (EIC) */
    {halIrqHandlerNotUsed},        /*  5  NON-VOLATILE MEMORY CONTROLLER(NVMCTRL) */
    {halIrqHandlerNotUsed},        /*  6  DMAC_Handler(DMAC) */
    {halIrqHandlerNotUsed},        /*  7  USB_Handler(USB) */
    {halIrqHandlerNotUsed},        /*  8  EVENT SYSTEM INTERFACE (EVSYS) */
    {0},       /*  9  SERIAL COMMUICATION INTERFACE 0 (SERCOM0) */
    {halIrqHandlerNotUsed},       /*  10  SERIAL COMMUICATION INTERFACE 1 (SERCOM1) */
    {halIrqHandlerNotUsed},        /*  11  SERIAL COMMUICATION INTERFACE 2 (SERCOM2) */
    {halIrqHandlerNotUsed},        /*  12  SERIAL COMMUICATION INTERFACE 3 (SERCOM3) */
    {halIrqHandlerNotUsed},        /*  13  SERIAL COMMUICATION INTERFACE 4 (SERCOM4) */
    {halIrqHandlerNotUsed},        /*  14  SERIAL COMMUICATION INTERFACE 5 (SERCOM5) */
    {halIrqHandlerNotUsed},        /*  15 TCC0 */
    {halIrqHandlerNotUsed},        /*  16 TCC1 */
    {halIrqHandlerNotUsed},        /*  17 TCC2 */
    {timerHandler},                /*  18 Timer Counter 3 */
    {halMacTimerHandler},          /*  19 Timer Counter 4 */
    {HAL_WDT_TC_IRQ_HANDLER},      /*  20 Timer Counter 5 */
    {halIrqHandlerNotUsed},        /*  21 Timer Counter 6 */
    {halIrqHandlerNotUsed},        /*  22 Timer Counter 7 */
    {halIrqHandlerNotUsed},        /*  23 ADC controller */
    {halIrqHandlerNotUsed},        /*  24 Analog Comparator */
    {halIrqHandlerNotUsed},        /*  25 DAC controller */
    {PTC_Handler},                 /*  26 PTC controller */
    {halIrqHandlerNotUsed},        /*  27 I2S controller */
    {halIrqHandlerNotUsed}         /*  28 not used */

};

/**************************************************************************//**
\brief Default interrupt handler for not used irq.
******************************************************************************/
static void halIrqHandlerNotUsed(void)
{
  while(1);
}

/**************************************************************************//**
\brief Interrupt Handler for hard fault
******************************************************************************/
static void hardFaultHandler(void)
{
  while(1);  
}

/**************************************************************************//**
\brief Register interrupt handler in vector table

\param[in] num - number of irq vector starting from zeroth IRQ handler (IRQ0)
\param[in] handler - vector handler function. Pass NULL to unregister vector
\return true if succeed, false otherwise
******************************************************************************/
bool HAL_InstallInterruptVector(int32_t num, HalIntFunc_t handler)
{
  num += 16; // Offset to zeroth user interrupt location (IRQ0)

  // Writes to out-of-bound location or to stack pointer position are forbidden
  if (num >= (int32_t) ARRAY_SIZE(__ram_vector_table) || num < 1)
    return false;

  __ram_vector_table[num].__fun = handler ? handler :  halIrqHandlerNotUsed;
  return true;
}

// eof halVectorTable.c
