/**************************************************************************//**
  \file  sensors.h

  \brief The header file describes the sensors interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    29/05/07 E. Ivanov - Created
*******************************************************************************/

#ifndef _SENSORS_H
#define _SENSORS_H

/******************************************************************************
                   Includes section
******************************************************************************/
// \cond
#include <sysTypes.h>
// \endcond

/******************************************************************************
                   Define(s) section
******************************************************************************/
// \cond
#define SENSOR_LIGHT       1
#define SENSOR_TEMPERATURE 2
#define SENSOR_BATTERY     3
#define SENSOR_LED         4
// \endcond

typedef void (*BspTemperatureCb_t)(bool result, int16_t data);
typedef void (*BspBatteryCb_t)(int16_t data);

/******************************************************************************
                   Prototypes section
******************************************************************************/
/***************************************************************************//**
\brief Opens temperature sensor.
\return
  BC_FAIL - sensor has been already open. \n
  BC_SUCCESS - otherwise.
*******************************************************************************/
result_t BSP_OpenTemperatureSensor(void);

/***************************************************************************//**
\brief Closes the temperature sensor.
\return
  BC_FAIL - if a hardware error has occured or
            there is uncompleted ReadData request. \n
  BC_SUCCESS - otherwise.
*******************************************************************************/
result_t BSP_CloseTemperatureSensor(void);

/**************************************************************************//**
\brief Reads data from the temperature sensor.
\param[in]
    f - callback.
    result - the result of the requested operation.
             true - operation finished successfully, false - some error has
             occured.
    data - sensor data.
\return
  BC_FAIL - previous request was not completed. \n
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t BSP_ReadTemperatureData(BspTemperatureCb_t f);

/***************************************************************************//**
\brief Opens the light sensor.
\return
  BC_FAIL - sensor has been already open. \n
  BC_SUCCESS - otherwise.
*******************************************************************************/
result_t BSP_OpenLightSensor(void);

/***************************************************************************//**
\brief Closes the light sensor.
\return
  BC_FAIL - if a hardware error has occured or
         there is uncompleted ReadData request. \n
  BC_SUCCESS  - otherwise.
*******************************************************************************/
result_t BSP_CloseLightSensor(void);

/**************************************************************************//**
\brief Reads data from the light sensor.
\param[in]
    f - callback pointer; can be set to NULL \n
    result - the result of the requested operation.
             true - operation finished successfully, false - some error has
             occured.
    data - sensor data.
\return
  BC_FAIL - previous request was not completed. \n
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t BSP_ReadLightData(void (*f)(bool result, int16_t data));

/***************************************************************************//**
\brief Opens the battery sensor.
\return
  BC_FAIL - sensor has been already open. \n
  BC_SUCCESS - otherwise.
*******************************************************************************/
result_t BSP_OpenBatterySensor(void);

/***************************************************************************//**
\brief Closes the battery sensor.
\return
  BC_FAIL - sensor was not opened. \n
  BC_SUCCESS  - otherwise.
*******************************************************************************/
result_t BSP_CloseBatterySensor(void);

/**************************************************************************//**
\brief Reads data from battery sensor.
\param[in]
    callback - callback function.
    data - sensor data.
    Can use (4ul * data * 125ul * 3ul) / (1024ul * 100ul) formula to count \n
    battery data in Volts (for avr).
\return
  BC_FAIL - previous request was not completed, or sensor was not opened. \n
  BC_SUCCESS - otherwise.
******************************************************************************/
result_t BSP_ReadBatteryData(BspBatteryCb_t cb);

#endif/* _SENSORS_H */
// eof sensors.h

