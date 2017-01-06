/**************************************************************************//**
  \file zllDemo.h

  \brief
    ZLLDemo application interface

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    13.01.12 A. Razinkov - Created.
    07.04.14 karthik.p_u - Modified.
******************************************************************************/

#ifndef _ZLLDEMO_H_
#define _ZLLDEMO_H_
/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl.h>
#include <pdsMemIds.h>
#include <bspLeds.h>
#include <zclZllColorControlCluster.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define ZLL_APP_MEMORY_MEM_ID APP_DIR3_MEM_ID

#define DEVICE_INFO_TABLE_ENTRIES 1
#define APP_PROFILE_ID            PROFILE_ID_LIGHT_LINK
#define APP_VERSION               2
#define CRLF "\r\n"

#define SOFTWARE_VERSION    CCPU_TO_LE32(0xAABBCCDD);

#define MAX_NUMBER_OF_BOUND_DEVICES 7
#define MAX_NUMBER_OF_SCENES        4

#if APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_ON_OFF_LIGHT
  #define APP_DEVICE_ID ZLL_ON_OFF_LIGHT_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT
  #define APP_DEVICE_ID ZLL_DIMMABLE_LIGHT_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT
  #define APP_DEVICE_ID ZLL_COLOR_LIGHT_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  #define APP_DEVICE_ID ZLL_EXTENDED_COLOR_LIGHT_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
  #define APP_DEVICE_ID ZLL_TEMPERATURE_COLOR_LIGHT_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE
  #define APP_DEVICE_ID ZLL_COLOR_REMOTE_DEVICE_ID
#elif APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_BRIDGE
  #define APP_DEVICE_ID ZLL_CONTROL_BRIDGE_DEVICE_ID
#else
  #error Specified device type is not supported
#endif /* APP_ZLL_DEVICE_TYPE */

#define APP_ENDPOINT_COLOR_SCENE_REMOTE 0x01 // Color Scene Remote endpoint identifier
#define APP_ENDPOINT_LIGHT              0x0A // Light endpoint identifier
#define APP_ENDPOINT_BRIDGE             0x0B // Control bridge endpoint identifier

/* PDS files and directories definitions */
#define APP_COLOR_SCENE_REMOTE_DATA_MEM_ID                     APP_PARAM2_MEM_ID
#define APP_COLOR_SCENE_REMOTE_LINK_INFO_CLUSTER_CLIENT_MEM_ID APP_PARAM3_MEM_ID

#define APP_LIGHT_DATA_MEM_ID                                  APP_PARAM2_MEM_ID
#define APP_LIGHT_COLOR_CONTROL_CLUSTER_SERVER_ATTR_MEM_ID     APP_PARAM3_MEM_ID
#define APP_LIGHT_LEVEL_CONTROL_CLUSTER_SERVER_ATTR_MEM_ID     APP_PARAM4_MEM_ID
#define APP_LIGHT_ONOFF_CLUSTER_SERVER_ATTR_MEM_ID             APP_PARAM5_MEM_ID
#define APP_LIGHT_SCENE_CLUSTER_SERVER_ATTR_MEM_ID             APP_PARAM6_MEM_ID

#define LCD_LINE_NETWORK_INFO   0u
#define LCD_LINE_STATUS         1u
#define LCD_LINE_TARGET         2u

/* Visualization redirect */
#if ((defined(BOARD_SAMR21) && (BSP_SUPPORT == BOARD_SAMR21_ZLLEK))) ||\
      ((defined(BOARD_MEGARF) && (BSP_SUPPORT == BOARD_RCB_KEY_REMOTE)))
#define LCD_INIT()     BSP_LcdInit()
#define LCD_PRINT      BSP_LcdPrint
#define LCD_FILL(fill) BSP_LcdFill(fill)
#define LCD_CLEAR()    BSP_LcdCls()
#else
#define LCD_INIT()     (void)0
#define LCD_PRINT(...) (void)0
#define LCD_FILL(...)  (void)0
#define LCD_CLEAR()    (void)0
#endif /* (defined(BOARD_SAMR21) && (BSP_SUPPORT == BOARD_SAMR21_ZLLEK) || 
            ((defined(BOARD_MEGARF) && (BSP_SUPPORT == BOARD_RCB_KEY_REMOTE)) */

#if ((defined(BOARD_MEGARF) && (BSP_SUPPORT == BOARD_RCB_KEY_REMOTE)))
#define LEDS_INIT()                        (void)0
#define LEDS_SET_COLOR_HS(...)             (void)0
#define LEDS_SET_COLOR_XY(...)             (void)0
#define LEDS_SET_BRIGHTNESS(...)           (void)0
#else
#define LEDS_INIT()                        BSP_InitLeds()
#define LEDS_SET_COLOR_HS(hue, saturation) BSP_SetLedColorHS(hue, saturation)
#define LEDS_SET_COLOR_XY(x, y)            BSP_SetLedColorXY(x, y)
#define LEDS_SET_BRIGHTNESS(level)         BSP_SetLedBrightnessLevel(level)
#endif /* (defined(BOARD_MEGARF) && (BSP_SUPPORT == BOARD_RCB_KEY_REMOTE))*/

#if defined(BOARD_SAMR21) && \
  ((BSP_SUPPORT == BOARD_SAMR21_XPRO) || (BSP_SUPPORT == BOARD_SAMR21_ZLLEK))
  #define TOUCHLINK_LED_INIT()               BSP_InitLeds()
  #define TOUCHLINK_LED_ON()                 BSP_OnLed(LED_FIRST)
  #define TOUCHLINK_LED_OFF()                BSP_OffLed(LED_FIRST)
  #define TOUCHLINK_LED_TOGGLE()             BSP_ToggleLed(LED_FIRST)
#elif defined(BOARD_MEGARF) && (BSP_SUPPORT != BOARD_RCB_KEY_REMOTE)
  #define TOUCHLINK_LED_INIT()               BSP_InitLeds()
  #if BSP_SUPPORT == BOARD_RCB
    #define TOUCHLINK_LED_ON()                 BSP_OnLed(LED_THIRD)
    #define TOUCHLINK_LED_OFF()                BSP_OffLed(LED_THIRD)
    #define TOUCHLINK_LED_TOGGLE()             BSP_ToggleLed(LED_THIRD)
  #elif BSP_SUPPORT == BOARD_ATMEGA256RFR2_XPRO
    #define TOUCHLINK_LED_ON()                 BSP_OnLed(LED_FIRST)
    #define TOUCHLINK_LED_OFF()                BSP_OffLed(LED_FIRST)
    #define TOUCHLINK_LED_TOGGLE()             BSP_ToggleLed(LED_FIRST)
  #elif BSP_SUPPORT == BOARD_FAKE
    #define TOUCHLINK_LED_ON()                 BSP_OnLed(LED_FIRST)
    #define TOUCHLINK_LED_OFF()                BSP_OffLed(LED_FIRST)
    #define TOUCHLINK_LED_TOGGLE()             BSP_ToggleLed(LED_FIRST)
  #endif
#else
  #define TOUCHLINK_LED_INIT()               (void)0
  #define TOUCHLINK_LED_ON()                 (void)0
  #define TOUCHLINK_LED_OFF()                (void)0
  #define TOUCHLINK_LED_TOGGLE()             (void)0
#endif // defined(BOARD_MEGARF) && (BSP_SUPPORT != BOARD_RCB_KEY_REMOTE)

/******************************************************************************
                    Types section
******************************************************************************/
BEGIN_PACK
typedef struct PACK
{
  ExtAddr_t      ieeeAddr;
  ShortAddr_t    nwkAddr;
  Endpoint_t     endpoint;
  bool           isInGroup;
  uint16_t       profileId;
  uint16_t       deviceId;
  uint8_t        version;
  uint8_t        groupIds;
} BoundDevice_t;
END_PACK

typedef struct _AppColorSceneRemoteAppData_t
{
  uint16_t    groupId;
  ShortAddr_t parent;
  bool        busy;
} AppColorSceneRemoteAppData_t;

typedef struct
{
  bool      free;
  uint8_t   sceneId;
  uint16_t  groupId;
  uint16_t  transitionTime;
  uint8_t   transitionTime100ms;

  // On/Off Cluster
  bool      onOff;
#if APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
  // Level Control Cluster
  uint8_t   currentLevel;
#endif // APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
#if APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  // Color Control Cluster
  ZCL_ZllEnhancedColorMode_t colorMode;
  uint16_t  currentX;
  uint16_t  currentY;
  uint16_t  enhancedCurrentHue;
  uint8_t   currentSaturation;
  uint8_t   colorLoopActive;
  uint8_t   colorLoopDirection;
  uint16_t  colorLoopTime;
#endif // APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
} Scene_t;

/**************************************************************************//**
\brief Fill destination addressing information for the request

\param[in] addressing - structure to fill
******************************************************************************/
void fillDstAddressing(ZCL_Addressing_t *addressing);

/**************************************************************************//**
\brief Restarts end device activity (parent polling)
******************************************************************************/
void restartActivity(void);

/**************************************************************************//**
\brief Sends permit join request

\param[in] permitJoin - join permittion value
******************************************************************************/
void sendPermitJoinCommand(bool permitJoin);

/**************************************************************************//**
\brief Posts application task if it in APP_JOINING state.
******************************************************************************/
void postStartNetworkTask(void);

/**************************************************************************//**
\brief Performs network discovery, network join and target type setting.
******************************************************************************/
void initiateStartingNetwork(void);

#endif // _ZLLDEMO_H_

// eof zllDemo.h
