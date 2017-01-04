/**************************************************************************//**
  \file rfBattery.h

  \brief Prototypes of battery monitor functions and corresponding types.
    Powered by RF chip.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      15/01/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _RFBATTERY_H
#define _RFBATTERY_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macCommon.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/

//! Set of possible voltage threshold values
typedef enum
{
  RF_1V70_BAT_VTG = 0x0,
  RF_1V75_BAT_VTG = 0x1,
  RF_1V80_BAT_VTG = 0x2,
  RF_1V85_BAT_VTG = 0x3,
  RF_1V90_BAT_VTG = 0x4,
  RF_1V95_BAT_VTG = 0x5,
  RF_2V00_BAT_VTG = 0x6,
  RF_2V05_BAT_VTG = 0x7,
  RF_2V10_BAT_VTG = 0x8,
  RF_2V15_BAT_VTG = 0x9,
  RF_2V20_BAT_VTG = 0xA,
  RF_2V25_BAT_VTG = 0xB,
  RF_2V30_BAT_VTG = 0xC,
  RF_2V35_BAT_VTG = 0xD,
  RF_2V40_BAT_VTG = 0xE,
  RF_2V45_BAT_VTG = 0xF,
  RF_2V550_BAT_VTG = 0x10 | 0x0,
  RF_2V625_BAT_VTG = 0x10 | 0x1,
  RF_2V700_BAT_VTG = 0x10 | 0x2,
  RF_2V775_BAT_VTG = 0x10 | 0x3,
  RF_2V850_BAT_VTG = 0x10 | 0x4,
  RF_2V925_BAT_VTG = 0x10 | 0x5,
  RF_3V000_BAT_VTG = 0x10 | 0x6,
  RF_3V075_BAT_VTG = 0x10 | 0x7,
  RF_3V150_BAT_VTG = 0x10 | 0x8,
  RF_3V225_BAT_VTG = 0x10 | 0x9,
  RF_3V300_BAT_VTG = 0x10 | 0xA,
  RF_3V375_BAT_VTG = 0x10 | 0xB,
  RF_3V450_BAT_VTG = 0x10 | 0xC,
  RF_3V525_BAT_VTG = 0x10 | 0xD,
  RF_3V600_BAT_VTG = 0x10 | 0xE,
  RF_3V675_BAT_VTG = 0x10 | 0xF,
} RF_BatteryMonVtg_t;

/*! \brief Battery monitor status type */
typedef enum
{
  /*! Voltage threshold has been successfuly set */
  RF_SUCCESS_BAT_MON_STATUS,
  /*! Voltage threshold cannot be set, because supply voltage 
      is already below the provided voltage threshold value */
  RF_FAIL_BAT_MON_STATUS,
} RF_BatteryMonStatus_t;

/*! \brief Battery monitor confirmation structure */
typedef struct
{
  //! The status of the request's execution
  RF_BatteryMonStatus_t status;
} RF_BatteryMonConf_t;

/*! \brief Battery monitor request structure */
typedef struct
{
  //\cond internal
  // Service field - for internal needs
  MAC_Service_t  service;
  //\endcond
  //! Voltage threshold
  RF_BatteryMonVtg_t voltage;
  //! Confirmation structure containing status
  RF_BatteryMonConf_t confirm;
  /*! Callback on request's execution - indicates that the threshold
      has been set or the operation has failed */
  void (*RF_BatteryMonConf)(RF_BatteryMonConf_t *conf);
  //! Inidicates that the supply voltage has dropped below the configured threshold
  void (*RF_BatteryMonInd)(void);
} RF_BatteryMonReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Sets battery monitor voltage threshold

If the supply voltage on one of the radio chip pins falls below
the provided voltage threshold value, the indication callback set 
in the RF_BatteryMonReq_t::RF_BatteryMonInd field is called.
The other callback function, set in the RF_BatteryMonReq_t::RF_BatteryMonConf
field is called on completion of the request to indicate that the
request's executed successfully and the threshold has been set or that
the threshold has not been set (a possible reason is that the supply
voltage is already below the provided threshold value).

\param[in] reqParams - request parameters structure pointer
\return none
*****************************************************************************/
void RF_BatteryMonReq(RF_BatteryMonReq_t *reqParams);

#endif /*_RFBATTERY_H*/

// eof rfBattery.h
