/**************************************************************************//**
  \file N_ConnectionCommon.c

  \brief Implementation of common ZigBee network connection creation and maintainance.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    11.12.12 D. Kolmakov - created
******************************************************************************/
#if defined(ZIGBEE_END_DEVICE)
  #include <N_ConnectionEndDevice_Bindings.h>
#else
  #include <N_ConnectionRouter_Bindings.h>
#endif
#include <N_Connection.h>
#include <N_Connection_Private.h>
#include <sysQueue.h>
#include <sysUtils.h>
#include <N_ErrH.h>
#include <N_Cmi.h>
#include <N_Radio_Internal.h>
#include <N_ReconnectHandler.h>
#include <N_Connection_Internal.h>
#include <N_DeviceInfo.h>

#include <zdo.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define COMPID "N_ConnectionCommon"

#define N_INTERPAN_MODE_OWNER_NONE      255u
#define N_INTERPAN_MODE_OWNER_TARGET    0u
#define N_INTERPAN_MODE_OWNER_APP       1u
#define N_INTERPAN_MODE_OWNER_INITIATOR 2u

#define N_INTERPAN_MODE_OWNERS_AMOUNT   3u

#if defined(ZIGBEE_END_DEVICE)
  #define N_CONNECTION_INCREASED_POLL_RATE 300
  #define N_CONNECTION_INCREASED_POLL_RATE_DURATION 5000
#endif

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _PreviousInterPanModeInfo_t
{
  uint8_t channel;
  uint8_t owner;
#if defined(ZIGBEE_END_DEVICE)
  bool polling;
#endif
} PreviousInterPanMode_t;

typedef struct _InterPanModeInfo_t
{
  QueueElement_t                     serviceField;
  PreviousInterPanMode_t             modeToRestore;
  N_Connection_SetInterPanModeDone_t done;
} InterPanModeInfo_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void interPanModePreparation(uint8_t owner, uint8_t channel);
static void interPanModeAfterExitRecovery(uint8_t owner);

static void interPanSetChannelConf(MAC_SetConf_t *conf);
#if defined(ZIGBEE_END_DEVICE)
static void interPanSetRxOnWhenIdleConf(MAC_SetConf_t *conf);
#if defined(_SLEEP_WHEN_IDLE_)
static void isInterPanModeBusy(SYS_EventId_t eventId, SYS_EventData_t data);
#endif
#endif
static void interPanSetRxEnableConf(MAC_RxEnableConf_t *conf);

static void updateNetworkConf(MAC_SetConf_t *conf);

/******************************************************************************
                    Static section
******************************************************************************/
#if defined(ZIGBEE_END_DEVICE)
static MAC_SetReq_t macSetReq;
static bool rxOnWhenIdleInterPandMode;
static bool dataPollDisable = false;
static uint16_t pollRateRestore;
extern uint32_t appPollRate;
#endif
static MAC_RxEnableReq_t macRxEnableReq;

static InterPanModeInfo_t interPanModesInfo[N_INTERPAN_MODE_OWNERS_AMOUNT];
static InterPanModeInfo_t reentrantInterpanModeInfo;
static uint8_t interPanOwner = N_INTERPAN_MODE_OWNER_NONE;
static uint8_t currentChannel;

/* Used to dis-allow change in poll rate from app when poll rate has been
   increased after touchlink for faster polling */
bool isSetPollRateAllowed = true;

/* Update network done callback */
static N_Connection_UpdateNetworkDone_t updateNetworkDoneCallback;
static uint8_t networkUpdateChannel;

static bool updateNetwork_IsBusy = false;

#if defined(ZIGBEE_END_DEVICE) && defined(_SLEEP_WHEN_IDLE_)
static SYS_EventReceiver_t nInterPanModeBusyCheck = { .func = isInterPanModeBusy};
static bool interPanModeFullExit;
#endif

static DECLARE_QUEUE(interPanModeRequestQueue);
static DECLARE_QUEUE(reentrantInterPanModeRequestQueue);
/***********************************************************************************
                    Implementation section
***********************************************************************************/
/** Update network channel/update id.
    \param networkChannel The new network channel
    \param networkUpdateId The new network update id
    \param pfDoneCallback Pointer to the function that will be called when the action is done

    This is an asynchronous call: do not call any other function of this
    component until the callback is received.
*/
void N_Connection_UpdateNetwork_Impl(uint8_t networkChannel,
                                uint8_t networkUpdateId,
                                N_Connection_UpdateNetworkDone_t pfDoneCallback)
{
  N_ERRH_ASSERT_FATAL(!updateNetworkDoneCallback);

#if defined(ZIGBEE_END_DEVICE)
  updateNetwork_IsBusy = true;
  N_Connection_Disconnected();
#endif

  NWK_SetUpdateId(networkUpdateId);
  networkUpdateChannel                   = networkChannel;
  updateNetworkDoneCallback              = pfDoneCallback;
  N_Radio_SetChannel(networkChannel, updateNetworkConf);
}


/** Returns the state of Update Network.
*/
bool N_Connection_UpdateNetwork_IsBusy(void)
{
  return updateNetwork_IsBusy;
}

/**************************************************************************//**
\brief Confirm from MAC_SetReq for update network routine
\param[in] conf - confirmation parameters
******************************************************************************/
static void updateNetworkConf(MAC_SetConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* Channel set error */

  updateNetwork_IsBusy = false;
  NWK_SetLogicalChannel(networkUpdateChannel);
  CS_WriteParameter(CS_NWK_LOGICAL_CHANNEL_ID, &networkUpdateChannel);

  updateNetworkDoneCallback();
  updateNetworkDoneCallback = NULL;

#if defined(ZIGBEE_END_DEVICE)
  N_Connection_ReconnectUrgent();
#endif
}

/** Request to switch channel correctly after leaving inter-PAN mode (asynchronous call),
    having been initiator prior to entering inter-PAN mode as target.

    After the application is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetTargetInterPanMacChannel must be called.
*/
void N_Connection_SetTargetInterPanMacChannel_Impl(void)
{
  if (!N_DeviceInfo_IsFactoryNew()&& N_DeviceInfo_IsEndDevice())
  {
    if(interPanOwner == N_INTERPAN_MODE_OWNER_INITIATOR)
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.channel = currentChannel;
  }
}

/** Request to turn on Inter-PAN mode (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    After the application is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetInterPanModeOff must be called.
*/
void N_Connection_SetInitiatorInterPanModeOn_Impl(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  switch (interPanOwner)
  {
    case N_INTERPAN_MODE_OWNER_NONE:
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.channel = N_CONNECTION_NETWORK_CHANNEL;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.owner   = N_INTERPAN_MODE_OWNER_NONE;
#if defined(ZIGBEE_END_DEVICE)
      ZDO_StopSyncReq();
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.polling = true;
#endif
      break;
    case N_INTERPAN_MODE_OWNER_TARGET:
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.channel = currentChannel;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.owner   = N_INTERPAN_MODE_OWNER_TARGET;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].modeToRestore.polling = false;
#endif
      break;
    case N_INTERPAN_MODE_OWNER_INITIATOR:
      break;
    default:
      // Initiator interPan mode is not used with application interPan mode at the same time
      N_ERRH_FATAL();
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].done);

  interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].done = pConfirmCallback;
  interPanModePreparation(N_INTERPAN_MODE_OWNER_INITIATOR, channel);
}

/** Request to leave Inter-PAN mode (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the application is done
    sending/receiving Inter-PAN messages.
*/
void N_Connection_SetInitiatorInterPanModeOff_Impl(N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  // Initiator interPan mode is not used with the application interPan mode
  // at the same time and it has the highest priority then target mode so
  // at this point there is only one possible owner - N_INTERPAN_MODE_OWNER_INITIATOR
  if (N_INTERPAN_MODE_OWNER_INITIATOR != interPanOwner)
  {
    pConfirmCallback();
    return;
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].done);

  interPanModesInfo[N_INTERPAN_MODE_OWNER_INITIATOR].done = pConfirmCallback;
  interPanModeAfterExitRecovery(N_INTERPAN_MODE_OWNER_INITIATOR);
}

/** Request to turn on Inter-PAN mode (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    After the application is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetInterPanModeOff must be called.
*/
void N_Connection_SetInterPanModeOn_Impl(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  switch (interPanOwner)
  {
    case N_INTERPAN_MODE_OWNER_NONE:
      interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.channel = N_CONNECTION_NETWORK_CHANNEL;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.owner   = N_INTERPAN_MODE_OWNER_NONE;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.polling = (ZDO_StopSyncReq() == ZDO_SUCCESS_STATUS);
#endif
      break;
    case N_INTERPAN_MODE_OWNER_TARGET:
      if(N_DeviceInfo_IsFactoryNew())
        interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.channel = currentChannel;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.owner   = N_INTERPAN_MODE_OWNER_TARGET;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].modeToRestore.polling = false;
#endif
      break;
    case N_INTERPAN_MODE_OWNER_APP:
      break;
    default:
      // Application interPan mode is not used with initiator interPan mode at the same time
      N_ERRH_FATAL();
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].done);

  interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].done = pConfirmCallback;
  interPanModePreparation(N_INTERPAN_MODE_OWNER_APP, channel);
}

/** Request to leave Inter-PAN mode (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the application is done
    sending/receiving Inter-PAN messages.
*/
void N_Connection_SetInterPanModeOff_Impl(N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  // Application interPan mode is not used with the initiator interPan mode
  // at the same time and it has the highest priority then target mode so
  // at this point there is only one possible owner - N_INTERPAN_MODE_OWNER_APP
  if (N_INTERPAN_MODE_OWNER_APP != interPanOwner)
  {
    pConfirmCallback();
    return;
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].done);

  interPanModesInfo[N_INTERPAN_MODE_OWNER_APP].done = pConfirmCallback;
  interPanModeAfterExitRecovery(N_INTERPAN_MODE_OWNER_APP);
}

/** Request to turn on Inter-PAN mode (asynchronous call).
    \param channel The requested ZigBee channel
    \param pConfirmCallback Pointer to the function called when the request has been handled

    After the application is done sending/receiving Inter-PAN messages
    \ref N_Connection_SetInterPanModeOff must be called.
*/
void N_Connection_SetTargetInterPanModeOn_Impl(uint8_t channel, N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
#if defined(ZIGBEE_END_DEVICE)
  if (N_ReconnectHandler_IsBusy())
  {
    N_Connection_PostponeInterPanMode(channel, pConfirmCallback);
    return;
  }
#endif
  switch (interPanOwner)
  {
    case N_INTERPAN_MODE_OWNER_NONE:
#if defined(ZIGBEE_END_DEVICE)
      ZDO_StopSyncReq();
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.polling = true;
#endif
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.channel = N_CONNECTION_NETWORK_CHANNEL;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.owner   = N_INTERPAN_MODE_OWNER_NONE;
      break;
    case N_INTERPAN_MODE_OWNER_APP:
    case N_INTERPAN_MODE_OWNER_INITIATOR:
      /* If We are re-entering into TARGET mode,
      We are pushing it to queue to restore it later while exit from Interpan mode */      
      if (N_INTERPAN_MODE_OWNER_TARGET == interPanModesInfo[interPanOwner].modeToRestore.owner)
      {
        memcpy(&reentrantInterpanModeInfo.modeToRestore, &interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore, sizeof(interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore));
        putQueueElem(&reentrantInterPanModeRequestQueue, &reentrantInterpanModeInfo.serviceField);
      }
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.channel =
                                                interPanModesInfo[interPanOwner].modeToRestore.channel;
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.owner   =
                                                interPanModesInfo[interPanOwner].modeToRestore.owner;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.polling = interPanModesInfo[interPanOwner].modeToRestore.polling;
#endif
      interPanModesInfo[interPanOwner].modeToRestore.channel                = channel;
      interPanModesInfo[interPanOwner].modeToRestore.owner                  = N_INTERPAN_MODE_OWNER_TARGET;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[interPanOwner].modeToRestore.polling = false;
#endif
      pConfirmCallback();
      return;
    case N_INTERPAN_MODE_OWNER_TARGET:
      break;
    default:
      N_ERRH_FATAL();
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].done);
  interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].done = pConfirmCallback;

  interPanModePreparation(N_INTERPAN_MODE_OWNER_TARGET, channel);
}

/** Request to leave Inter-PAN mode (asynchronous call).
    \param pConfirmCallback Pointer to the function called the when request has been handled

    This function must be called is when the application is done
    sending/receiving Inter-PAN messages.
*/
void N_Connection_SetTargetInterPanModeOff_Impl(N_Connection_SetInterPanModeDone_t pConfirmCallback)
{
  switch (interPanOwner)
  {
    case N_INTERPAN_MODE_OWNER_INITIATOR:
    case N_INTERPAN_MODE_OWNER_APP:
      interPanModesInfo[interPanOwner].modeToRestore.channel = interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.channel;
      interPanModesInfo[interPanOwner].modeToRestore.owner   = interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.owner;
#if defined(ZIGBEE_END_DEVICE)
      interPanModesInfo[interPanOwner].modeToRestore.polling = interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.polling;
#endif
      pConfirmCallback();
      return;
    case N_INTERPAN_MODE_OWNER_TARGET:
      if (N_INTERPAN_MODE_OWNER_TARGET == interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore.owner)
      {
        /* Restoring parameters which are queued up during re-entrance */
        QueueElement_t *queueElement;
        InterPanModeInfo_t *modeInfo;

        queueElement = deleteHeadQueueElem(&reentrantInterPanModeRequestQueue);
        N_ERRH_ASSERT_FATAL(NULL != queueElement); /* Unexpected re-entrance */
        modeInfo = GET_STRUCT_BY_FIELD_POINTER(InterPanModeInfo_t,
                                         serviceField,
                                         queueElement);
        memcpy(&interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].modeToRestore, &(modeInfo->modeToRestore), sizeof(modeInfo->modeToRestore));
      }
      break;
    default:
       pConfirmCallback();
       return;
  }

  N_ERRH_ASSERT_FATAL(!interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].done);
  interPanModesInfo[N_INTERPAN_MODE_OWNER_TARGET].done = pConfirmCallback;

  interPanModeAfterExitRecovery(N_INTERPAN_MODE_OWNER_TARGET);
}

#if defined(ZIGBEE_END_DEVICE) && defined(_SLEEP_WHEN_IDLE_)
/**************************************************************************//**
  \brief Processes BC_EVENT_BUSY_REQUEST request

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
 ******************************************************************************/
static void isInterPanModeBusy(SYS_EventId_t eventId, SYS_EventData_t data)
{
  bool *check = (bool *)data;
  *check = false;
  (void)eventId;
}
#endif

/**************************************************************************//**
\brief Stops poll requests on End Device
\param[in] cb - confirmation callback
******************************************************************************/
static void interPanModePreparation(uint8_t owner, uint8_t channel)
{
#if defined(ZIGBEE_END_DEVICE)
  if(!dataPollDisable)
  {
    pollRateRestore = appPollRate;
    appPollRate = 0;
    dataPollDisable = true;
  }
  rxOnWhenIdleInterPandMode = true;
#if defined(_SLEEP_WHEN_IDLE_)
  SYS_SubscribeToEvent(BC_EVENT_BUSY_REQUEST, &nInterPanModeBusyCheck);
#endif
#endif

  putQueueElem(&interPanModeRequestQueue, &interPanModesInfo[owner].serviceField);
  interPanOwner = owner;
  currentChannel = channel;

  N_Radio_SetChannel((channel == N_CONNECTION_NETWORK_CHANNEL) ? 
                     NWK_GetLogicalChannel() : channel, 
                     interPanSetChannelConf);
}

/**************************************************************************//**
\brief Performs recovery after exit interPan mode
\param[in] cb - confirmation callback
******************************************************************************/
static void interPanModeAfterExitRecovery(uint8_t owner)
{
  putQueueElem(&interPanModeRequestQueue, &interPanModesInfo[owner].serviceField);
  interPanOwner = interPanModesInfo[owner].modeToRestore.owner;
  currentChannel = interPanModesInfo[owner].modeToRestore.channel;

#if defined(ZIGBEE_END_DEVICE)
#if defined(_SLEEP_WHEN_IDLE_)
  interPanModeFullExit = true;
#endif
  if (N_INTERPAN_MODE_OWNER_NONE == interPanOwner)
    rxOnWhenIdleInterPandMode = false;
#endif

  
  N_Radio_SetChannel((interPanModesInfo[owner].modeToRestore.channel == N_CONNECTION_NETWORK_CHANNEL) ?
                     NWK_GetLogicalChannel() : interPanModesInfo[owner].modeToRestore.channel,
                     interPanSetChannelConf);
}

/**************************************************************************//**
\brief Confirm for setting new channel request
\param[in] MAC_SetConf - confirmation callback
******************************************************************************/
static void interPanSetChannelConf(MAC_SetConf_t *conf)
{
#if defined(ZIGBEE_END_DEVICE)
  macSetReq.attrValue.macPibAttr.rxOnWhenIdle = rxOnWhenIdleInterPandMode;
  macSetReq.attrId.phyPibId = MAC_PIB_RX_ON_WHEN_IDLE_ID;
  macSetReq.MAC_SetConf = interPanSetRxOnWhenIdleConf;
  MAC_SetReq(&macSetReq);
#else
  macRxEnableReq.confirm.status = MAC_SUCCESS_STATUS;
  interPanSetRxEnableConf(&macRxEnableReq.confirm);
#endif // defined(ZIGBEE_END_DEVICE)
  (void)conf;
}

#if defined(ZIGBEE_END_DEVICE)
/**************************************************************************//**
\brief Confirm for setting rxOnWhenIdle on/off
\param[in] conf - confirmation parameters
******************************************************************************/
static void interPanSetRxOnWhenIdleConf(MAC_SetConf_t *conf)
{
  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status);
  N_ERRH_ASSERT_FATAL(rxOnWhenIdleInterPandMode == macSetReq.attrValue.macPibAttr.rxOnWhenIdle);

  macRxEnableReq.rxOnDuration = rxOnWhenIdleInterPandMode;
  macRxEnableReq.MAC_RxEnableConf = interPanSetRxEnableConf;

  MAC_RxEnableReq(&macRxEnableReq);
}
#endif // defined(ZIGBEE_END_DEVICE)

/**************************************************************************//**
\brief Confirm for enable/disable reception request
\param[in] conf - confirmation parameters
******************************************************************************/
static void interPanSetRxEnableConf(MAC_RxEnableConf_t *conf)
{
  InterPanModeInfo_t *modeInfo;
  QueueElement_t *queueElement;

  N_ERRH_ASSERT_FATAL(MAC_SUCCESS_STATUS == conf->status); /* Channel set error */

  queueElement = deleteHeadQueueElem(&interPanModeRequestQueue);
  N_ERRH_ASSERT_FATAL(NULL != queueElement); /* Unexpected confirm */
  modeInfo = GET_STRUCT_BY_FIELD_POINTER(InterPanModeInfo_t,
                                         serviceField,
                                         queueElement);
#if defined(ZIGBEE_END_DEVICE)
  if(interPanModeFullExit && dataPollDisable)
  {
    dataPollDisable = false;
    appPollRate = pollRateRestore;
  }
  if (modeInfo->modeToRestore.polling)
  {
    /* Increase the poll rate to pass ZDP commands exchange on the touchlink
       completion */
     if (!rxOnWhenIdleInterPandMode)
     {
       N_Cmi_SetPollRateForTimePeriod(N_CONNECTION_INCREASED_POLL_RATE,
         N_CONNECTION_INCREASED_POLL_RATE_DURATION);
       isSetPollRateAllowed = false;
     }
  }
#if defined(_SLEEP_WHEN_IDLE_)
  if (interPanModeFullExit)
  {
    SYS_UnsubscribeFromEvent(BC_EVENT_BUSY_REQUEST, &nInterPanModeBusyCheck);
    interPanModeFullExit = false;
  }
#endif
#endif
  if(modeInfo->done!=NULL)
  {
    modeInfo->done();
    modeInfo->done = NULL;
  }
}

/** Checks if LinkTarget set the InterPan mode to on

    \returns TRUE if LinkTarget set the InterPan mode to on; FALSE - otherwise
*/
bool N_Connection_TargetIsInterpanModeOn_Impl(void)
{
  return (N_INTERPAN_MODE_OWNER_TARGET == interPanOwner);
}
