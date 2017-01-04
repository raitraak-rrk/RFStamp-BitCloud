/**************************************************************************//**
  \file  resetReason.h

  \brief The header file describes the the reset reason interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    05/12/07  A. Khromykh - Created.
    02/09/09  M. Gekk     - Platform XMega is supported.
    07/04/14  karthik.p_u - Modified
    14/05/14  karthik.p_u - Modified
 ******************************************************************************/
/******************************************************************************
 *   WARNING: CHANGING THIS FILE MAY AFFECT CORE FUNCTIONALITY OF THE STACK.  *
 *   EXPERT USERS SHOULD PROCEED WITH CAUTION.                                *
 ******************************************************************************/

#ifndef _RESETREASON_H
#define _RESETREASON_H

/* \cond */
/******************************************************************************
                              Define(s) section
 ******************************************************************************/
#define TEMP_WARM_RESET   0x12

/*A known magic number to indicate watchdog reset reason in USB mode*/
#define WDT_USB_WARM_RESET (0xa5a5)

/* \endcond */

/******************************************************************************
                               Types section
 ******************************************************************************/
/** \brief The reset reason types. */
#if defined(AT91SAM7X256)
  typedef enum
  {
    /** \brief VDDCORE rising */
    POWER_ON_RESET   = 0x00,
    /** \brief Watchdog fault occurred */
    WDT_RESET        = 0x02,
    /** \brief Processor reset required by the software */
    WARM_RESET       = 0x03,
    /** \brief NRST pin detected low */
    EXTERNAL_RESET   = 0x04,
    /** \brief BrownOut reset occurred */
    BROWN_OUT_RESET  = 0x05
  } HAL_ResetReason_t;
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) || defined(ATMEGA128RFA1) \
   || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  typedef enum
  {
    /** \brief jump to the NULL pointer was issued. */
    NULL_PTR_DEREF_RESET = 0x0,
    /** \brief the supply voltage was below reset threshold. */
    POWER_ON_RESET       = 0x1,
    /** \brief a low level was present on RESET pin. */
    EXTERNAL_RESET       = 0x2,
    /** \brief the supply voltage was below Brown-out reset threshold. Set by fuses.*/
    BROWN_OUT_RESET      = 0x4,
    /** \brief Watch Dog Timer period expired. */
    WDT_RESET            = 0x8,
    /** \brief MCU was reset by JTAG. */
    JTAG_RESET           = 0x10,
    /** \brief software reset. */
    WARM_RESET           = 0x11
  } HAL_ResetReason_t;
#elif defined(ATXMEGA128A1) || defined(ATXMEGA128B1) || defined(ATXMEGA256A3) || defined(ATXMEGA256D3)
  typedef enum
  {
    /** \brief jump to the NULL pointer was issued. */
    NULL_PTR_DEREF_RESET = 0x00,
    /** \brief Power-on reset is released when the VCC stops rising or when
     * the VCC level has reached the Power-on Threshold Voltage (VPOT) level. */
    POWER_ON_RESET       = (1U << 0),
    /** \brief The external reset will trigger when the RESET pin is driven
     * below the RESET pin threshold voltage, VRST. */
    EXTERNAL_RESET       = (1U << 1),
    /** \brief The VCC level falls bellow the trigger level for a minimum time. */
    BROWN_OUT_RESET      = (1U << 2),
    /** \brief Watchdog reset will be given if the WDT is not reset from t
     * he software within a programmable timout period, */
    WATCHDOG_RESET       = (1U << 3),
    /** \brief The Program and Debug Interface reset contains a separate reset source
     * that is used to reset the device during external programming and debugging. */
    DEBUG_RESET          = (1U << 4),
    /** \brief The Software reset makes it possible to issue a system reset from
     * software by writing to the Software Reset bit in the Reset Control Register. */
    WARM_RESET           = (1U << 5)
  } HAL_ResetReason_t;
#elif defined(AT32UC3A0512)
  typedef enum
  {
    /** \brief The CPU was reset due to the supply voltage
     * being lower than the power-on threshold level. */
    POWER_ON_RESET       = (1U << 0),
    /** \brief The CPU was reset due to the supply voltage
     * being lower than the brown-out threshold level. */
    BROWN_OUT_RESET      = (1U << 1),
    /** \brief The CPU was reset due to the RESET pin being asserted. */
    EXTERNAL_RESET       = (1U << 2),
    /** \brief CPU reset caused by watchdog. */
    WDT_RESET            = (1U << 3),
    /** \brief The CPU was reset by setting the bit RC_CPU in the JTAG reset register. */
    DEBUG_RESET          = (1U << 4),
    /** \brief The CPU was reset because it had detected an illegal access. */
    CPUERR_RESET         = (1U << 7),
    /** \brief The CPU was reset because the RES strobe in
     * the OCD Development Control register has been written to one. */
    OCDRST_RESET         = (1U << 8),
    /** \brief The chip was reset by setting the bit RC_OCD in the
     * JTAG reset register or by using the JTAG HALT instruction.*/
    JTAGHARD_RESET       = (1U << 9),
    /** \brief CPU reset caused by call to halSoftwareReset. */
    WARM_RESET           = (1U << 10),
  } HAL_ResetReason_t;
#elif defined(AT91SAM3S4C) || defined(AT91SAM4S16C)
  typedef enum
  {
    /** \brief A general reset occurs when a Power-on-reset is detected,
      a Brownout or a Voltage regulation loss is detected by the Supply controller. */
    GENERAL_RESET        = 0x00,
    /** \brief A Backup reset occurs when the chip returns from Backup mode.
     The core_backup_reset signal is asserted by the Supply Controller when a Backup reset occurs. */
    BACKUP_RESET         = 0x01,
    /** \brief CPU reset caused by watchdog. */
    WDT_RESET            = 0x02,
    /** \brief CPU reset caused by call to halSoftwareReset. */
    WARM_RESET           = 0x03,
    /** \brief The CPU was reset due to the RESET pin being asserted. */
    EXTERNAL_RESET       = 0x04
  } HAL_ResetReason_t;
#elif defined(ATSAMD20J18) || defined(ATSAMD21J18A) || defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  typedef enum
  {
    /** \brief A general reset occurs when a Power-on-reset is detected,
      This flag is set if a POR occurs. */
    POWER_ON_RESET            = (1U << 0),
    /** \brief This flag is set if a BOD12 reset occurs. */
    BROWNOUT12_DETECTOR_RESET = (1U << 1),
    /** \brief This flag is set if a BOD33 reset occurs. */
    BROWNOUT33_DETECTOR_RESET = (1U << 2),
    /** \brief This flag is set if an external reset occurs. */
    EXTERNAL_RESET            = (1U << 4),
    /** \brief This flag is set if a Watchdog Timer reset occurs. */
    WDT_RESET                 = (1U << 5),
    /** \brief This bit is set if a system reset request has been performed. */
    WARM_RESET                = (1U << 6),
  } HAL_ResetReason_t;
#elif defined(WIN)
  typedef enum
  {
    /** \brief Fake reset reason */
    FAKE_RESET_REASON        = 0x00,
  } HAL_ResetReason_t;
#else
  #error 'unsupported cpu'
#endif

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
\brief Returns the reset reason.

\ingroup hal_misc

\return The reason of reset.
 ******************************************************************************/
HAL_ResetReason_t HAL_ReadResetReason(void);

/**************************************************************************//**
\brief Software reset.

\ingroup hal_misc
 ******************************************************************************/
void HAL_WarmReset(void);

#endif /* _RESETREASON_H */
/* resetReason.h */
