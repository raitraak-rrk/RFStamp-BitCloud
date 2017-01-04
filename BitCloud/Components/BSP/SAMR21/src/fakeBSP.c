/***************************************************************************//**
\file  fakeBSP.c

\brief Implementation of fake board-specific periphery.

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    07/04/14 karthik.p_u - Created
*******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysTypes.h>
#include <bspTaskManager.h>
#include <bspDbg.h>
#include <joystick.h>
#include <usart.h>
/******************************************************************************
                   Local variables section
******************************************************************************/
static void(* readTemperatureDataCallback)(bool error, int16_t data);
static void(* readLightDataCallback)(bool error, int16_t data);
static void(* readBatteryDataCallback)(int16_t data);

/******************************************************************************
                   Implementations section
******************************************************************************/
#if BSP_SUPPORT == BOARD_FAKE
/******************************************************************************
 Returns number which was read from external eeprom.
 Parameters:
   id - UID buffer pointer.
 Returns:
   0 - if unique ID has been found without error;
  -1 - if there are some erros during UID discovery.
******************************************************************************/
int BSP_ReadUid(uint64_t *id)
{
  (void)id;
  return 0;
}

/**************************************************************************//**
\brief Configure the usart(sercom,usartPinConfig & PadConfig) based on the board
******************************************************************************/
void BSP_BoardSpecificUsartPinInit(UsartChannel_t tty)
{
  tty->sercom = &(SERCOM0->USART);
  tty->usartPinConfig[USART_TX_SIG].pinNum = 6;
  tty->usartPinConfig[USART_TX_SIG].portNum = PORT_A;
  tty->usartPinConfig[USART_TX_SIG].functionConfig = 3;
  tty->usartPinConfig[USART_RX_SIG].pinNum = 7;
  tty->usartPinConfig[USART_RX_SIG].portNum = PORT_A;
  tty->usartPinConfig[USART_RX_SIG].functionConfig = 3;
  tty->rxPadConfig = 3;
  tty->txPadConfig = 1;
}

/**************************************************************************//**
\brief Configure the antenna diversity pins based on the board
******************************************************************************/
bool BSP_BoardSpecificAntennaDivesityPinInit(uint8_t* antennaSelected)
{
  (void)antennaSelected;
  return 0;
}

/**************************************************************************//**
\brief Checks whether antenna diversity actions required based on the board 
       and antenna divesity enabled or not
******************************************************************************/
bool BSP_IsAntDiversityActionsRequired(void)
{
  return 0;
}

/******************************************************************************
                             Leds
******************************************************************************/
/**************************************************************************//**
\brief Opens leds module to use.

\return
    operation state
******************************************************************************/
result_t BSP_OpenLeds(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Closes leds module.

\return
    operation state
******************************************************************************/
result_t BSP_CloseLeds(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Turns on the LED.

\param[in]
    id - number of led
******************************************************************************/
void BSP_OnLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Turns off the LED.

\param[in]
      id - number of led
******************************************************************************/
void BSP_OffLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Changes the LED state to opposite.

\param[in]
      id - number of led
******************************************************************************/
void BSP_ToggleLed(uint8_t id)
{
  (void)id;
}

/**************************************************************************//**
\brief Initialize LEDs.
******************************************************************************/
void BSP_InitLeds(void)
{
}

/**************************************************************************//**
\brief Set brightness level
\param[in] level - new brightness level
******************************************************************************/
void BSP_SetLedBrightnessLevel(uint8_t level)
{
  (void)level;
}

/**************************************************************************//**
\brief Set the color using Hue and Saturation as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorHS(uint16_t hue, uint8_t saturation)
{
  (void)hue;
  (void)saturation;
}

/**************************************************************************//**
\brief Set the color using X and Y as parameters
\param[in] level - a new brightness level
******************************************************************************/
void BSP_SetLedColorXY(uint16_t x, uint16_t y)
{
  (void)x;
  (void)y;
}

/******************************************************************************
                             Buttons
******************************************************************************/
/**************************************************************************//**
\brief Registers handlers for button events.

\param[in]
    pressed - the handler to process pressing the button
\param[in]
    released - the handler to process releasing the button
\param[in]
    bn - button number.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_OpenButtons(void (*pressed)(uint8_t bn), void (*released)(uint8_t bn))
{
  (void)pressed;
  (void)released;

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Cancel buttons handlers.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_CloseButtons(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads state of buttons.

\return
    Current buttons state in a binary way. \n
    Bit 0 defines state of the button 1, \n
    bit 1 defines state of the button 2.
******************************************************************************/
uint8_t BSP_ReadButtonsState(void)
{
  return 0;
}

/**************************************************************************//**
\brief  BSP's event about KEY has changed state.
******************************************************************************/
void bspButtonsHandler(void)
{
}
#endif

/******************************************************************************
                             Sensors
******************************************************************************/
/***************************************************************************//**
\brief Opens temperature sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_OpenTemperatureSensor(void)
{
  return BC_SUCCESS;
}

/***************************************************************************//**
\brief Closes the temperature sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_CloseTemperatureSensor(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads data from the temperature sensor.
\param[in]
    result - the result of the requested operation.
             true - operation finished successfully, false - some error has
             occured.
\param[in]
    data - sensor data.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_ReadTemperatureData(void (*f)(bool result, int16_t data))
{
  readTemperatureDataCallback = f;

  bspPostTask(BSP_TEMPERATURE);

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief BSP task handler for temperature sensor.
******************************************************************************/
void bspTemperatureSensorHandler(void)
{
  sysAssert(readTemperatureDataCallback, BSPSENSORS_TEMPERATURENULLCALLBACK_0);
  readTemperatureDataCallback(true, 0);
}

#if BSP_SUPPORT == BOARD_FAKE 
/***************************************************************************//**
\brief Opens temperature sensor.
*******************************************************************************/
void BSP_OpenTempSensor(void)
{
}

/***************************************************************************//**
\brief Closes the temperature sensor.
*******************************************************************************/
void BSP_CloseTempSensor(void)
{
}

/**************************************************************************//**
 \brief Reads temperature from internal temperature sensor
 \param value - pointer to the value to be written.
******************************************************************************/
void BSP_ReadTempSensor(int16_t *value)
{
  (void)value;
}
#endif // BSP_SUPPORT == BOARD_FAKE

/***************************************************************************//**
\brief Opens the light sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_OpenLightSensor(void)
{
  return BC_SUCCESS;
}

/***************************************************************************//**
\brief Closes the light sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_CloseLightSensor(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads data from the light sensor.
\param[in]
    result - the result of the requested operation.
             true - operation finished successfully, false - some error has
             occured.
\param[in]
    data - sensor data.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_ReadLightData(void (*f)(bool result, int16_t data))
{
  readLightDataCallback = f;

  bspPostTask(BSP_LIGHT);

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief BSP task handler for light sensor.
******************************************************************************/
void bspLightSensorHandler(void)
{
  sysAssert(readLightDataCallback, BSPSENSORS_LIGHTNULLCALLBACK_0);
  readLightDataCallback(true, 0);
}

/***************************************************************************//**
\brief Opens the battery sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_OpenBatterySensor(void)
{
  return BC_SUCCESS;
}

/***************************************************************************//**
\brief Closes the battery sensor.

\return
  BC_SUCCESS - always.
*******************************************************************************/
result_t BSP_CloseBatterySensor(void)
{
  return BC_SUCCESS;
}

/**************************************************************************//**
\brief Reads data from battery sensor.

\param[in]
    data - sensor data.

\return
  BC_SUCCESS - always.
******************************************************************************/
result_t BSP_ReadBatteryData(void (*f)(int16_t data))
{
  readBatteryDataCallback = f;

  bspPostTask(BSP_BATTERY);

  return BC_SUCCESS;
}

/**************************************************************************//**
\brief BSP task handler for battery sensor.
******************************************************************************/
void bspEmptyBatteryHandler(void)
{
  sysAssert(readBatteryDataCallback, BSPSENSORS_BATTERYNULLCALLBACK_0);
  readBatteryDataCallback(0);
}

/******************************************************************************
                             Sliders
******************************************************************************/
/**************************************************************************//**
\brief Reads the sliders.

\return
   state of 3 on-board DIP-switches.User can uses SLIDER0, SLIDER1, SLIDER2
   defines to test state. Value 1 indicates that slider is on.
******************************************************************************/
uint8_t BSP_ReadSliders(void)
{
  return 0;
}

#if BSP_SUPPORT == BOARD_FAKE
/******************************************************************************
                             Joystick
******************************************************************************/
/******************************************************************************
\brief Registers handler for joystick events.

\param[in]
    state - joystick state.

\return
  BC_FAIL - joystick module is busy,
  BC_SUCCESS in other cases.
******************************************************************************/
result_t BSP_OpenJoystick(void (*generalHandler)(BSP_JoystickState_t state))
{
  (void)generalHandler;

  return BC_SUCCESS;
}

/******************************************************************************
\brief Cancel joystick handlers.

\return
  BC_FAIL - joystick module was not opened,
  BC_SUCCESS in other cases.
******************************************************************************/
result_t BSP_CloseJoystick(void)
{
  return BC_SUCCESS;
}

/******************************************************************************
\brief Reads state of joystick.

\return
    Current joystick state.
******************************************************************************/
BSP_JoystickState_t BSP_ReadJoystickState(void)
{
  return 0;
}
/******************************************************************************
BSP's event about joystick has changed state.
******************************************************************************/
void bspJoystickHandler(void)
{
}

#endif
// eof fakeBSP.c
