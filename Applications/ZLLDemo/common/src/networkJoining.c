/**************************************************************************//**
  \file networkJoining.c

  \brief
    Application network joining implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    23.11.12 N. Fomin - Created.
******************************************************************************/
/******************************************************************************
                    Include section
******************************************************************************/
#include <debug.h>
#include <lightBindings.h>
#include <N_Connection.h>
#include <N_LinkTarget.h>
#include <zllDemo.h>
#include <uartManager.h>

#include <N_DeviceInfo_Bindings.h>
#include <N_DeviceInfo.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define APP_DEVICE_TYPE_LIGHT            ((APP_ZLL_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT) && (APP_ZLL_DEVICE_TYPE <= APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT))
#define ENABLE_ASSOCIATION_FOR_LIGHT     (APP_DEVICE_TYPE_LIGHT && (APP_SCAN_ON_STARTUP == 1))
#define APP_DEVICE_NOT_LIGHT             ((APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE) || (APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_BRIDGE))
#define ENABLE_ASSOCIATION_FOR_NOT_LIGHT APP_DEVICE_NOT_LIGHT
#define ENABLE_ASSOCIATION               (ENABLE_ASSOCIATION_FOR_LIGHT || ENABLE_ASSOCIATION_FOR_NOT_LIGHT)

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _NetworkJoiningState_t
{
  NETWORK_JOINING_IDLE_STATE,
  NETWORK_JOINING_DISCOVERY_STATE,
  NETWORK_JOINING_JOIN_NETWORK_STATE,
  NETWORK_JOINING_NETWORK_FINISHING_STATE
} NetworkJoiningState_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
#if ENABLE_ASSOCIATION
static void assocDiscoveryDone(N_Connection_Result_t result);
static void assocJoinDone(N_Connection_Result_t result);
#endif // ENABLE_ASSOCIATION

/******************************************************************************
                    Static variables section
******************************************************************************/
#if ENABLE_ASSOCIATION
static N_Beacon_t beacon;
static NetworkJoiningState_t currentState = NETWORK_JOINING_IDLE_STATE;
static void (*startNetworkDoneCb)(void);
#endif // ENABLE_ASSOCIATION

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Performs network joining via association or linking

\param[in] doneCb - callback which is called on the end of start network procedure
******************************************************************************/
void startNetwork(void (*doneCb)(void))
{
#if ENABLE_ASSOCIATION
  sysAssert(NULL == startNetworkDoneCb, NETWORKJOINING_STARTNETWORK_0);
  sysAssert(NETWORK_JOINING_IDLE_STATE == currentState, NETWORKJOINING_STARTNETWORK_1);

  currentState = NETWORK_JOINING_DISCOVERY_STATE;
  N_Connection_AssociateDiscovery(&beacon, N_Connection_AssociateDiscoveryMode_AnyPan, NULL, assocDiscoveryDone);
  startNetworkDoneCb = doneCb;
#else
  doneCb();
#endif // ENABLE_ASSOCIATION
}

/**************************************************************************//**
\brief Performs network joining via association or linking
******************************************************************************/
void startNetworkHandler(void)
{
#if ENABLE_ASSOCIATION
  switch (currentState)
  {
    case NETWORK_JOINING_IDLE_STATE:
      break;
    case NETWORK_JOINING_DISCOVERY_STATE:
      break;
    case NETWORK_JOINING_JOIN_NETWORK_STATE:
      N_Connection_AssociateJoin(&beacon, assocJoinDone);
      break;
    case NETWORK_JOINING_NETWORK_FINISHING_STATE:
      currentState = NETWORK_JOINING_IDLE_STATE;
      startNetworkDoneCb();
      startNetworkDoneCb = NULL;
      break;
    default:
      break;
  }
#endif // ENABLE_ASSOCIATION
}

#if ENABLE_ASSOCIATION
/**************************************************************************//**
\brief Callback function about network discovery

\param[in] result - the result of network discovery
******************************************************************************/
static void assocDiscoveryDone(N_Connection_Result_t result)
{
  sysAssert(NETWORK_JOINING_DISCOVERY_STATE == currentState, NETWORKJOINING_DISCOVERYDONE_0);

  if (N_Connection_Result_Success == result)
  {
    currentState = NETWORK_JOINING_JOIN_NETWORK_STATE;
    LOG_STRING(discoveryDoneStr, "Discovery done\r\n");
    appSnprintf(discoveryDoneStr);
  }
  else
  {
    currentState = NETWORK_JOINING_NETWORK_FINISHING_STATE;
    LOG_STRING(discoveryFailedStr, "Discovery failed\r\n");
    appSnprintf(discoveryFailedStr);
  }

  postStartNetworkTask();
}

/**************************************************************************//**
\brief Callback function about joining via association

\param[in] result - the result of joining
******************************************************************************/
static void assocJoinDone(N_Connection_Result_t result)
{
  ExtPanId_t       csExtPanId;
  ExtPanId_t       csNwkExtPanId;
  sysAssert(NETWORK_JOINING_JOIN_NETWORK_STATE == currentState, NETWORKJOINING_JOINDONE_0);
  currentState = NETWORK_JOINING_NETWORK_FINISHING_STATE;

  if (N_Connection_Result_Success == result)
  {
    LOG_STRING(joinDoneStr, "Association succeded\r\n");
    appSnprintf(joinDoneStr);
    LOG_STRING(ownAddrStr, "Got NWK address 0x%x\r\n");
    appSnprintf(ownAddrStr, N_DeviceInfo_GetNetworkAddress());

#if defined(ATMEL_APPLICATION_SUPPORT)
    CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
    // CS_EXT_PANID is 0 on default, after joining any network its ext panId shall be retained
    if (!csExtPanId)
    {
      CS_ReadParameter(CS_NWK_EXT_PANID_ID, &csNwkExtPanId);
      CS_WriteParameter(CS_EXT_PANID_ID, &csNwkExtPanId);
    }
#endif
  }
  else
  {
    LOG_STRING(joinFailedStr, "Association failed\r\n");
    appSnprintf(joinFailedStr);
  }

#if APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE
  restartActivity();
#endif // APP_ZLL_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_SCENE_REMOTE
  postStartNetworkTask();
}
#endif // ENABLE_ASSOCIATION

// eof networkJoining.c