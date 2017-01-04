/**************************************************************************//**
  \file N_ConnectionAssociation.c

  \brief Implementation of Association to ZigBee network.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    08.11.12 A. Razinkov - created
******************************************************************************/

/******************************************************************************
                    Includes section
******************************************************************************/
#if defined(ZIGBEE_END_DEVICE)
#include <N_ConnectionEnddevice_Bindings.h>
#else
#include <N_ConnectionRouter_Bindings.h>
#endif
#include <N_Connection_Private.h>

#include <N_Connection.h>
#include <N_ErrH.h>
#include <N_LinkTarget.h>
#include <N_Cmi.h>
#include <N_Cmi_Init.h>
#include <N_DeviceInfo_Init.h>
#include <N_DeviceInfo.h>
#include <N_Radio_Internal.h>

#include <appTimer.h>
#include <zdo.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_ConnectionAssociation"

#ifndef N_CONNECTION_ASSOCIATION_MAX_SUBSCRIBERS
  #define N_CONNECTION_ASSOCIATION_MAX_SUBSCRIBERS 10u
#endif

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _N_Connection_DiscoveryContext_t
{
  N_Connection_AssociateDiscoveryDone_t pfDoneCallback;
  N_Beacon_t* pBeacon;
  N_Connection_AssociateDiscoveryMode_t mode;
  N_Address_ExtendedPanId_t* extendedPanId;
  uint8_t channelIdx;
  bool aborted;
  HAL_AppTimer_t touchlinkWaitTimer;
  N_Connection_Result_t status;
} N_Connection_DiscoveryContext_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void networkDiscoveryDone(void);
static void touchlinkWaitTimerFired(void);
static uint32_t getMaskForNextChannel(void);
static void switchToFirstPrimaryChannelDone(MAC_SetConf_t *conf);
static void restoreToOriginalChannelDone(MAC_SetConf_t *conf);
static void finishDiscovery(N_Connection_Result_t status);
static void associateJoinConf(N_Cmi_Result_t result);
static void abortDiscovery(void);
static bool secondaryChannelsScanIsRequired(uint32_t channelMask);
static void performDiscoveryOnNextChannel(void);
static void joinCompletionTimerFired(void);
static void breakContextTimerFired(void);
static void finishAssociation(void);

/******************************************************************************
                    Static variables section
******************************************************************************/
static N_Connection_DiscoveryContext_t discoveryContext =
{
  .touchlinkWaitTimer =
  {
    .interval = 300,
    .mode = TIMER_ONE_SHOT_MODE,
    .callback = touchlinkWaitTimerFired
  }
};

static N_Connection_AssociateJoinDone_t associateJoinDone = NULL;
static N_Connection_Result_t associationResult;

/* Timer to let all parameters changed events been registered by N_DeviceInfo */
static HAL_AppTimer_t joinCompletionDelayTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 200,
  .callback = joinCompletionTimerFired
};

/* Timer to break context is case of connection abortion */
static HAL_AppTimer_t breakContextTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 1,
  .callback = breakContextTimerFired
};

/***********************************************************************************
                    Implementation section
***********************************************************************************/

/** Find a beacon of an open ZigBee Pro network.
    \param pBeacon Pointer to memory that is used to return a beacon from an open network.
    \param mode The discovery mode. Can be used to blacklist or specify a network.
    \param extendedPanId Extended pan id to join or to blacklist depending on the mode parameter.
    \param pfDoneCallback Callback function pointer that is called upon completion. \see
           N_Connection_AssociateDiscoveryDone_t.

    This is an asynchronous call: do not call any other function of this
    component except for N_Connection_AbortAssociateDiscovery until the callback is received.
*/
void N_Connection_AssociateDiscovery_Impl(N_Beacon_t* pBeacon, N_Connection_AssociateDiscoveryMode_t mode, N_Address_ExtendedPanId_t* extendedPanId, N_Connection_AssociateDiscoveryDone_t pfDoneCallback)
{
  uint32_t channelMask;

  N_ERRH_ASSERT_FATAL(!discoveryContext.pfDoneCallback); /* Unexpected function call */

  discoveryContext.pfDoneCallback = pfDoneCallback;
  discoveryContext.pBeacon = pBeacon;
  discoveryContext.mode = mode;
  discoveryContext.extendedPanId = extendedPanId;
  discoveryContext.channelIdx = 0;
  discoveryContext.aborted = false;

  /* Clean the beacon buffer */
  memset(pBeacon, 0x00, sizeof(N_Beacon_t));

  /* Obtain mask of next channel to scan */
  channelMask = getMaskForNextChannel();

  /* Scan next channel */
  N_Cmi_NetworkDiscovery(channelMask, N_Beacon_Order_60ms, pBeacon,
    mode, extendedPanId, BEACON_FILTERING_CRITERION_ASSOCIATION, NULL,
    networkDiscoveryDone);
}

/** Abort the discovery of open ZigBee Pro networks.

    This is an asynchronous call: do not call any other function of this
    component until the N_Connection_AssociateDiscoveryDone_t callback is received.
*/
void N_Connection_AbortAssociateDiscovery_Impl(void)
{
  if (discoveryContext.pfDoneCallback)
  {
    /* If touchlink wait timer wasn't started - we should wait until asynchronous process completion */
    if (HAL_StopAppTimer(&discoveryContext.touchlinkWaitTimer))
      discoveryContext.aborted = true;
    /* Immediate abortion - touchlink wait timer is the only activity for Association */
    else
      abortDiscovery();
  }
}

/** Gets a channel mask to continue NWK Discovery procedure on next channel

    \returns channel mask to continue NWK Discovery procedure on next channel
*/
static uint32_t getMaskForNextChannel(void)
{
  uint8_t primaryChannelsAmount =
    N_DeviceInfo_GetNrChannelsInChannelMask(N_DeviceInfo_GetPrimaryChannelMask());
  uint8_t secondaryChannelsAmount = 0;
  uint32_t secondmask = N_DeviceInfo_GetSecondaryChannelMask();
  uint8_t channel = 0;

  if(secondmask != 0uL)
    secondaryChannelsAmount = N_DeviceInfo_GetNrChannelsInChannelMask(secondmask);

  /* All channels have been scanned */
  if (primaryChannelsAmount + secondaryChannelsAmount <= discoveryContext.channelIdx)
    return 0;
  /* Next channel - is one of the primaries */
  else if (discoveryContext.channelIdx < primaryChannelsAmount)
    channel = N_DeviceInfo_GetChannelForIndex(discoveryContext.channelIdx,
      N_DeviceInfo_GetPrimaryChannelMask());
  /* Next channel - is one of the secondaries */
  else
    channel = N_DeviceInfo_GetChannelForIndex(discoveryContext.channelIdx - primaryChannelsAmount,
      N_DeviceInfo_GetSecondaryChannelMask());

  discoveryContext.channelIdx++;
  return 1ul << channel;
}

/** NWK Discovery finished on appropriate channel
*/
static void networkDiscoveryDone(void)
{
  /* Abort further discovery, if required */
  if (discoveryContext.aborted)
    abortDiscovery();
  else if (!N_Connection_TargetIsInterpanModeOn())
    performDiscoveryOnNextChannel();
  /* Switch to the first channel of primary channel set for possible touchlink */
  else
    N_Radio_SetChannel(N_DeviceInfo_GetChannelForIndex(0, N_DeviceInfo_GetPrimaryChannelMask()),
                       switchToFirstPrimaryChannelDone);
}

/** Callback on switching to first channel of primary channels set.
*/
static void switchToFirstPrimaryChannelDone(MAC_SetConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* Channel change error */

  /* Abort further discovery, if required */
  if (discoveryContext.aborted)
    HAL_StartAppTimer(&breakContextTimer);
  /* Start waiting for touchlink */
  else
    HAL_StartAppTimer(&discoveryContext.touchlinkWaitTimer);
}

/** Touchlink awaiting finished callback
*/
static void touchlinkWaitTimerFired(void)
{
  /* Abort further operations, if required */
  if (discoveryContext.aborted)
    abortDiscovery();
  /* Touchlink wasn't performed - return to Discovery procedure */
  else
    performDiscoveryOnNextChannel();
}

/** Perfoms NWK discovery on next channel
*/
static void performDiscoveryOnNextChannel(void)
{
  uint32_t channelMask;

  /* Obtain mask of next channel to scan */
  channelMask = getMaskForNextChannel();

  /* Finish discovery, if there are no channel to scan or
     if suitable beacon was found during the primary channels scan */
  if (!channelMask || !secondaryChannelsScanIsRequired(channelMask))
  {
    /* Return failure, if beacon is empty */
    if (!discoveryContext.pBeacon->logicalChannel)
      finishDiscovery(N_Connection_Result_Failure);
    else
      finishDiscovery(N_Connection_Result_Success);
  }
  /* Continue with next channel */
  else
    N_Cmi_NetworkDiscovery(channelMask, N_Beacon_Order_60ms, discoveryContext.pBeacon,
      discoveryContext.mode, discoveryContext.extendedPanId,
      BEACON_FILTERING_CRITERION_ASSOCIATION, NULL, networkDiscoveryDone);
}

/** Checks, if NWK Discovery should be continued on secondary channels
 *  \param channelMask mask of channels for next scanning
 *
 *  \returns true - if secondary channels scanning is required; false - otherwise
*/
static bool secondaryChannelsScanIsRequired(uint32_t channelMask)
{
  uint8_t channel = N_DeviceInfo_GetChannelForIndex(0, N_DeviceInfo_GetSecondaryChannelMask());

  /* If next channel to scan - is a first of the secondary channels */
  if ((1ul << channel) == channelMask)
  {
    /* And suitable beacon was found during the primary channels scan - omit secondary channels scanning */
    if ((1ul << discoveryContext.pBeacon->logicalChannel) & N_DeviceInfo_GetPrimaryChannelMask())
      return false;
  }

  /* Secondary channels should be scanned */
  return true;
}

/** Abort Discovery procedure.
*/
static void abortDiscovery(void)
{
  discoveryContext.aborted = false;
  finishDiscovery(N_Connection_Result_Aborted);
}

/** Finishes NWK Discovery
 *  \param status NWK Discovery result.
*/
static void finishDiscovery(N_Connection_Result_t status)
{
  discoveryContext.status = status;
  N_Radio_SetChannel(N_DeviceInfo_GetNetworkChannel(), restoreToOriginalChannelDone);
}

/** Callback on switching to original channel after the scanning.
*/
static void restoreToOriginalChannelDone(MAC_SetConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* Channel change error */
  N_ERRH_ASSERT_FATAL(discoveryContext.pfDoneCallback); /* No callback function */

  discoveryContext.pfDoneCallback(discoveryContext.status);
  /* Done callback used as a busy indicator */
  discoveryContext.pfDoneCallback = NULL;
}

/** Associate with an open ZigBee Pro network.
    \param pBeacon Beacon of the device to associate with.
    \param pfDoneCallback Callback function pointer that is called upon completion. \see
           N_Connection_AssociateJoinDone_t.
*/
void N_Connection_AssociateJoin_Impl(N_Beacon_t* pBeacon, N_Connection_AssociateJoinDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(NULL == associateJoinDone); /* Unexpected function call */

#if defined(_ASSERT_ON_NFN_CALLING_ASSOCIATION_)
  N_ERRH_ASSERT_FATAL( NULL == !N_DeviceInfo_IsFactoryNew());
#endif  //#if defined(_ASSERT_ON_NFN_CALLING_ASSOCIATION_)

  associateJoinDone = pfDoneCallback;

  N_Cmi_Join(pBeacon, associateJoinConf);
}

/** Start network confirm handler.
    \param conf start network confirm parameters .
*/
static void associateJoinConf(N_Cmi_Result_t result)
{
  if (N_Cmi_Result_Success == result)
  {
    uint32_t channelMask = N_DeviceInfo_GetPrimaryChannelMask();

#ifndef ATMEL_APPLICATION_SUPPORT
    ExtPanId_t       csExtPanId;
    ExtPanId_t       csNwkExtPanId;

    CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
    // CS_EXT_PANID is 0 on default, after joining any network its ext panId shall be retained
    if (!csExtPanId)
    {
      CS_ReadParameter(CS_NWK_EXT_PANID_ID, &csNwkExtPanId);
      CS_WriteParameter(CS_EXT_PANID_ID, &csNwkExtPanId);
    }
#endif

    associationResult = N_Connection_Result_Success;

    if (N_DeviceInfo_TrustCenterMode_Distributed != N_DeviceInfo_GetTrustCenterMode())
      channelMask |= N_DeviceInfo_GetSecondaryChannelMask();
    CS_WriteParameter(CS_CHANNEL_MASK_ID, &channelMask);

    /* Set Channel in Device Info upon Successful join */
    N_DeviceInfo_SetNetworkChannel(NWK_GetLogicalChannel());

    /* Wait until all parameters changed events will be collected by DeviceInfo */
    HAL_StartAppTimer(&joinCompletionDelayTimer);
    return;
  }
  else if (N_Cmi_Result_NoNetworkKey == result)
  {
    associationResult = N_Connection_Result_NoNetworkKey;
    N_DeviceInfo_SetFactoryNew(true);
  }
  else if (N_Cmi_Result_Invalid_Request == result)
  {
    associationResult = N_Connection_Invalid_Request;
  }
  else
  {
    associationResult = N_Connection_Result_Failure;
    N_DeviceInfo_SetFactoryNew(true);
  }
    finishAssociation();
}

/** Association completion delay timer fired handler
*/
static void joinCompletionTimerFired(void)
{
  finishAssociation();
}

/** Finish association procedure
*/
static void finishAssociation(void)
{
  N_ERRH_ASSERT_FATAL(NULL != associateJoinDone);
  associateJoinDone(associationResult);
  associateJoinDone = NULL;

  if (N_Connection_Result_Success == associationResult)
  {
    N_Connection_NetworkJoined();
    N_Connection_Connected();
  }
}

/** Break context timer fired handler
*/
static void breakContextTimerFired(void)
{
  abortDiscovery();
}

/* eof N_ConnectionAssociation.c */
