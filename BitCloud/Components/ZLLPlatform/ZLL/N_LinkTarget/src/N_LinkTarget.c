
/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_LinkTarget.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_LinkTarget_Bindings.h"
#include "N_LinkTarget_Init.h"
#include "N_LinkTarget.h"

/***************************************************************************************************
* EXTERNAL INCLUDE FILES
***************************************************************************************************/

#include "N_Address.h"
#include "N_AddressManager.h"
#include "N_Cmi.h"
#include "N_Connection.h"
#include "N_Connection_Internal.h"
#include "N_DeviceInfo.h"
#include "N_ErrH.h"
#include "N_Fsm.h"
#include "N_InterPan.h"
#include "N_Log.h"
#include "N_PacketDistributor.h"
#include "N_Radio.h"
#include "N_Security.h"
#include "N_Task.h"
#include "N_Timer.h"
#include "N_Types.h"
#include "N_Util.h"

#include "devconfig.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_LinkTarget"

/** Maximum number of subscribers to this component */
#if (!defined(N_LINK_TARGET_MAX_SUBSCRIBERS))
#  define N_LINK_TARGET_MAX_SUBSCRIBERS 2u
#endif

#define EVENT_TIMER                     0u
#define EVENT_TRANSACTION_ID_TIMEOUT    1u
#define EVENT_JOIN_NETWORK_DONE         2u
#define EVENT_UPDATE_NETWORK_DONE       3u
#define EVENT_SET_INTERPAN_MODE_DONE    4u
#define EVENT_SCAN_JITTER               5u

/** Time to wait between sending a ZigBee message and changing channels/network. */
#define CHANGE_TIMEOUT_MS               100u

/** Time to wait for a response after sending a request. */
#define RESPONSE_TIMEOUT_MS             5000u

/** Maximum time of a transaction. */
#define TRANSACTION_ID_TIMEOUT_MS       8000u

/** Maximum random delay between ScanRequest and ScanResponse. Should always be a lot smaller than 250 - 2*20 ms! */
#define MAX_RANDOM_DELAY_MS             100u

/** The maximum number of active transaction IDs that is maintained. */
#define RESPONSE_TABLE_SIZE             3u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

typedef struct ReceivedScanRequest_t
{
    N_InterPan_ScanRequest_t scanRequest;   // no pointer as it is decoupled!
    int8_t rssi;
    uint32_t transactionId;
    N_Address_Extended_t sourceAddress;
} ReceivedScanRequest_t;

typedef struct ReceivedCommand_t
{
    union
    {
        N_InterPan_DeviceInfoRequest_t* pDeviceInfoRequest;
        N_InterPan_IdentifyRequest_t* pIdentifyRequest;
        N_InterPan_NetworkUpdateRequest_t* pNetworkUpdateRequest;
        N_InterPan_NetworkJoinResponse_t* pNetworkJoinResponse;
    } payload;
    uint32_t transactionId;
    N_Address_Extended_t sourceAddress;
} ReceivedCommand_t;

typedef struct NetworkParameters_t
{
    N_Address_ExtendedPanId_t extendedPanId;
    uint8_t channel;
    uint16_t panId;
    uint16_t networkAddress;
    uint8_t networkUpdateId;
    uint16_t groupIdsBegin;
    uint16_t groupIdsEnd;
    uint8_t keyIndex;
    uint32_t transactionId;
    uint32_t responseId;
    uint8_t msgSequenceNumber;
    N_Security_Key_t encryptedNetworkKey;
} NetworkParameters_t;

typedef struct N_LinkTarget_ReceivedNetworkJoinResponse_t
{
    N_InterPan_NetworkJoinResponse_t networkJoinResponse;
    N_Address_Extended_t sourceAddress;
} N_LinkTarget_ReceivedNetworkJoinResponse_t;

typedef struct N_LinkTarget_ResponseParameters_t
{
    uint32_t  transactionId;
    uint32_t  responseId;
    uint8_t   zllInfo;
    uint8_t   zigBeeInfo;
} N_LinkTarget_ResponseParameters_t;

typedef enum
{
  ZSuccess,
  ZFailure
} ZStatus_t;

typedef struct _SecurityKeyBuffer_t
{
  N_Security_Key_t key;
  bool busy;
} SecurityKeyBuffer_t;

/***************************************************************************************************
* Prototypes section
***************************************************************************************************/
static void continueSendScanResponse(void);
static void continueJoinNetwork(void);

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;

N_UTIL_CALLBACK_DECLARE(N_LinkTarget_Callback_t, s_subscribers, N_LINK_TARGET_MAX_SUBSCRIBERS);

static N_LinkTarget_SetTargetTypeDone_t s_pfSetTargetTypeDone = NULL;

static N_LinkTarget_TargetType_t s_targetType = N_LinkTarget_TargetType_None;

/** Table with active transactions. */
static N_LinkTarget_ResponseParameters_t s_responseTable[RESPONSE_TABLE_SIZE];

static ReceivedCommand_t s_receivedCommand;
static ReceivedScanRequest_t s_receivedScanRequest;

/** Network parameters determined by NetworkJoinRequest/NetworkStartRequest/NetworkUpdateRequest. */
static NetworkParameters_t s_newNetworkParameters;

/** The network address of the end-device that has to be added as a child.

If zero, this value and the IEEE address are invalid and should not be used.
*/
static uint16_t s_networkAddressEndDevice = 0u;
/** The IEEE address of the end-device that has to be added as a child. */
static N_Address_Extended_t s_ieeeAddressEndDevice;

/** Previous transmit power used for restoring the transmitpower after a ScanResponse. */
static uint8_t s_currentTransmitPower;

/** Set touchlink priority request field in scan response */
static bool s_touchlinkPriorityEnabled = FALSE;

/** Set to FALSE to indicate a non-stealable device */
static bool s_allowStealing = TRUE;

static N_Timer_t s_timer;
static N_Timer_t s_timerTransactionIdTimeout;
static N_Timer_t s_timerScanJitter;

static SecurityKeyBuffer_t keyBuffer =
{
  .busy = false
};

/** Boolean that shows if the radio is busy */
static bool s_radioBusy = FALSE;

/***************************************************************************************************
* STATE MACHINE
***************************************************************************************************/

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2);
static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2);

static void EnabledExit(void);

enum N_LinkTarget_states // states
{
    sDisabled,
    sWaitForInterPanModeOn,
    sEnabled,
    sWaitForInterPanModeOff,
    sJoinNetworkAfterInterPanModeOff,
    sWaitForJoinNetworkDone,
    sUpdateNetworkAfterInterPanModeOff,
    sWaitForUpdateNetworkDone,
    sJoiningWaitForInterPanModeOff,
    sFindingFreePan,
    sWaitForStartOrJoinResponseToBeSent,
};

enum N_LinkTarget_events // events
{
    eTimer,
    eSetTargetType,
    eSetInterPanModeDone,
    eReceivedScanRequest,
    eReceivedDeviceInfoRequest,
    eReceivedIdentifyRequest,
    eReceivedResetToFactoryNewRequest,
    eReceivedValidNetworkStartRequest,
    eReceivedValidNetworkJoinRequest,
    eJoinNetworkDone,
    eUpdateNetwork,
    eUpdateNetworkDone,
    eReceivedNetworkJoinResponse,
    eFoundFreePan,
    eScanResponseJitterDelay,
};

enum N_LinkTarget_actions // actions
{
    aSetInterPanModeOnNetworkChannel,
    aRestoreTransmitPower,
    aConfirmSetTargetTypeDone,
    aStartTransaction_StoreCurrentTransmitPower_SendScanResponse_StartChangeDelay,
    aSendDeviceInfoResponse,
    aIdentifyIndication,
    aStartChangeDelay,
    aJoinNetwork,
    aAddChild_JoinNetworkIndication,
    aUpdateNetwork,
    aUpdateNetworkIndication,
    aDisableTarget_SetInterPanModeOff,
    aDisableTarget_SetInterPanModeOff_StopTimer,
    aResetIndication,
    aSendNetworkStartResponseOk_StartChangeDelay,
    aFindFreePan_StopTimer,
    aStartJitterDelay_ScanIndication,
    aStopTimer,
};

enum N_LinkTarget_conditions // conditions
{
    cIsTargetTypeNone,
    cIsValidScanRequest,
    cIsActiveTransaction,
    cIsJoinResponseError,
    cIsInvalidTransaction,
    cIsStartRequestPanId0OrExtPanId0,
};

/** \image html N_LinkTarget_Fsm.png */
/* @Fsm2PlantUml:begin file=N_LinkTarget_Fsm.png */

static const N_FSM_Transition_t s_transitionTable[] =
{
    //     event                                    condF                                   actF                                newState
    N_FSM_STATE( sDisabled ),
    N_FSM( eSetTargetType,                          cIsTargetTypeNone,                      aConfirmSetTargetTypeDone,          N_FSM_SAME_STATE ),
    N_FSM( eSetTargetType,                          N_FSM_ELSE,                             aSetInterPanModeOnNetworkChannel,   sWaitForInterPanModeOn ),

    N_FSM_STATE( sWaitForInterPanModeOn ),
    N_FSM( eSetInterPanModeDone,                    N_FSM_NONE,                             aConfirmSetTargetTypeDone,          sEnabled ),

    N_FSM_STATE( sEnabled ),
    N_FSM( eSetTargetType,                          cIsTargetTypeNone,                      aDisableTarget_SetInterPanModeOff_StopTimer,  sWaitForInterPanModeOff ),
    N_FSM( eSetTargetType,                          N_FSM_ELSE /*IsTargetTypeTouchlink */,  aSetInterPanModeOnNetworkChannel,   sWaitForInterPanModeOn ),
    N_FSM( eReceivedScanRequest,                    cIsValidScanRequest,                    aStartJitterDelay_ScanIndication,   N_FSM_SAME_STATE ),
    N_FSM( eScanResponseJitterDelay,                N_FSM_NONE,                             aStartTransaction_StoreCurrentTransmitPower_SendScanResponse_StartChangeDelay, N_FSM_SAME_STATE ),
    N_FSM( eTimer,                                  N_FSM_NONE,                             aSetInterPanModeOnNetworkChannel,   N_FSM_SAME_STATE ),
    // The following event can only be caused by aSetInterPanModeOnNetworkChannel in this state (previous event). Decoupling here
    // is needed because both actions are asynchronous.
    N_FSM( eSetInterPanModeDone,                    N_FSM_NONE,                             aRestoreTransmitPower,              N_FSM_SAME_STATE ),
    N_FSM( eReceivedDeviceInfoRequest,              cIsActiveTransaction,                   aSendDeviceInfoResponse,            N_FSM_SAME_STATE ),
    N_FSM( eReceivedIdentifyRequest,                cIsActiveTransaction,                   aIdentifyIndication,                N_FSM_SAME_STATE ),
    N_FSM( eReceivedValidNetworkStartRequest,       cIsStartRequestPanId0OrExtPanId0,       aFindFreePan_StopTimer,             sFindingFreePan ),
    N_FSM( eReceivedValidNetworkStartRequest,       N_FSM_ELSE,                             aSendNetworkStartResponseOk_StartChangeDelay, sWaitForStartOrJoinResponseToBeSent ),
    // The NetworkJoinRouterResponse, NetworkJoinEndDeviceResponse are generated outside the FSM
    N_FSM( eReceivedValidNetworkJoinRequest,        N_FSM_NONE,                             aStartChangeDelay,                  sWaitForStartOrJoinResponseToBeSent ),
    // NetworkUpdateRequest is handled outside the FSM,
    // it generates the eUpdateNetwork event to let the FSM handle the actual update sequence
    N_FSM( eUpdateNetwork,                          N_FSM_NONE,                             aDisableTarget_SetInterPanModeOff_StopTimer,  sUpdateNetworkAfterInterPanModeOff ),
    N_FSM( eReceivedResetToFactoryNewRequest,       N_FSM_NONE,                             aResetIndication,                   N_FSM_SAME_STATE ),

    N_FSM_STATE( sFindingFreePan ),
    N_FSM( eFoundFreePan,                           N_FSM_NONE,                             aSendNetworkStartResponseOk_StartChangeDelay, sWaitForStartOrJoinResponseToBeSent ),

    N_FSM_STATE( sWaitForStartOrJoinResponseToBeSent ),
    N_FSM( eTimer,                                  N_FSM_NONE,                             aDisableTarget_SetInterPanModeOff,  sJoinNetworkAfterInterPanModeOff ),

    N_FSM_STATE( sJoinNetworkAfterInterPanModeOff ),
    N_FSM( eSetInterPanModeDone,                    N_FSM_NONE,                             aJoinNetwork,                       sWaitForJoinNetworkDone ),

    N_FSM_STATE( sWaitForJoinNetworkDone ),
    N_FSM( eJoinNetworkDone,                        N_FSM_NONE,                             aAddChild_JoinNetworkIndication,    sDisabled ), ///< \todo Handle JoinNetwork failure

    N_FSM_STATE( sUpdateNetworkAfterInterPanModeOff ),
    N_FSM( eSetInterPanModeDone,                    N_FSM_NONE,                             aUpdateNetwork,                     sWaitForUpdateNetworkDone ),

    N_FSM_STATE( sWaitForUpdateNetworkDone ),
    N_FSM( eUpdateNetworkDone,                      N_FSM_NONE,                             aUpdateNetworkIndication,           sDisabled ),

    N_FSM_STATE( sWaitForInterPanModeOff ),
    N_FSM( eSetInterPanModeDone,                    N_FSM_NONE,                             aConfirmSetTargetTypeDone,          sDisabled ),

};

static uint8_t s_currentState;

static const N_FSM_StateEntryExit_t s_entryExit[] =
{
    N_FSM_ENTRYEXIT( sEnabled, NULL, EnabledExit ),
};

N_FSM_DECLARE(s_fsm,
              s_transitionTable,
              N_FSM_TABLE_SIZE(s_transitionTable),
              s_entryExit,
              N_FSM_TABLE_SIZE(s_entryExit),
              PerformAction,
              CheckCondition);

/* @Fsm2PlantUml:end  */

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static void FsmHandleEvent(enum N_LinkTarget_events event)
{
    (void) N_FSM_PROCESS_EVENT(&s_fsm, &s_currentState, event);
}

static void EnabledExit(void)
{
    // Disable the jitter timer if running.
    N_Timer_Stop(&s_timerScanJitter);
}

static inline bool IsTargetTypeNone(void)
{
    return N_UTIL_BOOL(s_targetType == N_LinkTarget_TargetType_None);
}

static uint8_t* getSecurityKeyBuffer(void)
{
  N_ERRH_ASSERT_FATAL(!keyBuffer.busy); /* No free security buffer */

  keyBuffer.busy = true;

  return keyBuffer.key;
}

static inline void freeSecurityKeyBuffer(void)
{
  keyBuffer.busy = false;
}

static inline uint8_t* extractKeyFromBuffer(void)
{
  return keyBuffer.key;
}

/** Store the transaction id in the response table.
\param transactionId The transaction id to store in the table
\param responseId The response id to store in the table
\param zllInfo The zll info to store in the table
\param zigBeeInfo The zigBee info to store in the table
*/
static void StoreInResponseTable(uint32_t transactionId, uint32_t responseId, uint8_t zllInfo, uint8_t zigBeeInfo)
{
    for ( uint8_t i = 0u; i < RESPONSE_TABLE_SIZE; i++ )
    {
        if ( s_responseTable[i].transactionId == 0uL )
        {
            s_responseTable[i].transactionId = transactionId;
            s_responseTable[i].responseId = responseId;
            s_responseTable[i].zllInfo = zllInfo;
            s_responseTable[i].zigBeeInfo = zigBeeInfo;
            return;
        }
    }
    // table full - overwrite the first
    N_LOG_NONFATAL();
    s_responseTable[0].transactionId = transactionId;
    s_responseTable[0].responseId = responseId;
    s_responseTable[0].zllInfo = zllInfo;
    s_responseTable[0].zigBeeInfo = zigBeeInfo;
}

static N_LinkTarget_ResponseParameters_t* GetFromResponseTable(uint32_t transactionId)
{
    for ( uint8_t i = 0u; i < RESPONSE_TABLE_SIZE; i++ )
    {
        if ( s_responseTable[i].transactionId == transactionId )
        {
            return &s_responseTable[i];
        }
    }
    return NULL;
}

/** Erase all stored transaction ids. */
static void EraseResponseTable(void)
{
    for ( uint8_t i = 0u; i < RESPONSE_TABLE_SIZE; i++ )
    {
        s_responseTable[i].transactionId = 0uL;
    }
}

/** Check if the transaction id is active.
\note The value zero is already rejected by N_InterPan.
*/
static bool IsTransactionIdActive(uint32_t transactionId)
{
    if (GetFromResponseTable(transactionId) == NULL)
    {
        return FALSE;
    }
    return TRUE;
}

/** Check the authentication, if required.
\param transactionId The transaction ID for which to check the authentication status
\returns TRUE if the initiator has been either authenticated, or this was not required by the target, FALSE otherwise.
*/
static bool IsAuthenticationCorrectOrNotRequired(uint32_t transactionId)
{
    N_LinkTarget_ResponseParameters_t* pEntry = GetFromResponseTable(transactionId);
    if (pEntry == NULL)
    {
        return FALSE;   // transaction ID not active
    }
    else
    {
        return TRUE;
    }
}

/** Callback function, see \ref N_Connection_JoinNetworkDone_t. */
static void JoinNetworkDone(N_Connection_Result_t result)
{
    N_Task_SetEvent(s_taskId, EVENT_JOIN_NETWORK_DONE);
    (void)result;
}

/** Callback function, see \ref N_Connection_UpdateNetworkDone_t. */
static void UpdateNetworkDone(void)
{
    N_Task_SetEvent(s_taskId, EVENT_UPDATE_NETWORK_DONE);
}

/** Callback function, see \ref N_Connection_SetInterPanModeDone_t. */
static void SetInterPanModeDone(void)
{
    s_radioBusy = FALSE;
    N_Task_SetEvent(s_taskId, EVENT_SET_INTERPAN_MODE_DONE);
}

/** Callback function, see \ref N_Connection_FindFreePanDone_t. */
static void FindFreePanDone(uint16_t panId, N_Address_ExtendedPanId_t extendedPanId)
{
    if (s_newNetworkParameters.panId == 0u)
    {
#if defined(DEVCFG_FIXED_PANID)
#warning USING FIXED PANID!
        panId = DEVCFG_FIXED_PANID;
#endif
        s_newNetworkParameters.panId = panId;
    }

    if (memcmp(s_newNetworkParameters.extendedPanId, c_ExtPanId00, sizeof(s_newNetworkParameters.extendedPanId)) == 0)
    {
#if defined(DEVCFG_FIXED_EXT_PANID)
#warning USING FIXED EXTENDED PANID!
        extendedPanId = (N_Address_ExtendedPanId_t)DEVCFG_FIXED_EXT_PANID;
#endif
        memcpy(s_newNetworkParameters.extendedPanId, extendedPanId, sizeof(s_newNetworkParameters.extendedPanId));
    }
    FsmHandleEvent(eFoundFreePan);
}

static void RestoreTransmitPowerDone(void)
{
    s_radioBusy = FALSE;
}

/***************************************************************************************************
* STATE MACHINE ACTION FUNCTIONS
***************************************************************************************************/

static inline void SetInterPanModeOnNetworkChannel(void)
{
    s_radioBusy = TRUE;
    if ( N_DeviceInfo_IsFactoryNew() )
    {
        N_Connection_SetTargetInterPanModeOn(N_DeviceInfo_GetChannelForIndex(0u, N_DeviceInfo_GetPrimaryChannelMask()), SetInterPanModeDone);
    }
    else
    {
       N_Connection_SetTargetInterPanModeOn(N_CONNECTION_NETWORK_CHANNEL, SetInterPanModeDone);
    }
}

static inline void SetInterPanModeOff(void)
{
    s_radioBusy = TRUE;
    N_Connection_SetTargetInterPanModeOff(SetInterPanModeDone);
}

static inline void ConfirmSetTargetTypeDone(void)
{
    if ( s_pfSetTargetTypeDone != NULL )
    {
        s_pfSetTargetTypeDone();
        s_pfSetTargetTypeDone = NULL;
    }
}

/** Store the transaction id and start the transaction timeout. */
static inline void StartTransaction(void)
{
    StoreInResponseTable(
        s_receivedScanRequest.transactionId,
        0uL,
        s_receivedScanRequest.scanRequest.zllInfo,
        s_receivedScanRequest.scanRequest.zigBeeInfo);
    N_Timer_Start16(TRANSACTION_ID_TIMEOUT_MS, &s_timerTransactionIdTimeout);
}

static inline void SendScanResponse(void)
{
   s_radioBusy = TRUE;
   // set 0 dBm transmission power
   N_Radio_SetTxPower(N_DeviceInfo_GetTouchlinkZerodBmTransmitPower(), continueSendScanResponse);
}

static void continueSendScanResponse(void)
{
    s_radioBusy = FALSE;

    N_InterPan_ScanResponse_t payload;
    memset(&payload,0,sizeof(N_InterPan_ScanResponse_t));

    payload.header.transactionSequenceNumber = s_receivedScanRequest.scanRequest.header.transactionSequenceNumber;

    payload.transactionId = s_receivedScanRequest.transactionId;
    if ( N_DeviceInfo_IsEndDevice() )
    {
        payload.zigBeeInfo = N_INTERPAN_ZIGBEE_INFO_END_DEVICE;
    }
    else
    {
        payload.zigBeeInfo = N_INTERPAN_ZIGBEE_INFO_ROUTER
            | N_INTERPAN_ZIGBEE_INFO_RX_ON_IDLE;
    }

    payload.zllInfo = 0u;

    if ( N_DeviceInfo_IsFactoryNew() )
    {
        payload.zllInfo |= N_INTERPAN_ZLL_INFO_FACTORY_NEW;
    }

    if ( N_DeviceInfo_IsAddressAssignmentCapable() )
    {
        payload.zllInfo |= N_INTERPAN_ZLL_INFO_ADDRESS_ASSIGNMENT;
    }

    if ( N_DeviceInfo_CanBeLinkInitiator() )
    {
        payload.zllInfo |= N_INTERPAN_ZLL_INFO_TOUCHLINK_INITIATOR;
    }

    payload.keyBitMask = N_Security_GetKeyBitMask();
    payload.responseId = N_Security_GetResponseId();

    N_LinkTarget_ResponseParameters_t* myResponseParameters = GetFromResponseTable(payload.transactionId);
    if (myResponseParameters != NULL)
    {
        myResponseParameters->responseId = payload.responseId;
    }

   // send touchlink response
   // set touchlink priority request if enabled
   if(s_touchlinkPriorityEnabled)
   {
       payload.zllInfo |= N_INTERPAN_ZLL_INFO_TOUCHLINK_PRIORITY_REQUEST;
   }

    payload.touchlinkRssiCorrection = N_DeviceInfo_GetTouchlinkRssiCorrection();

    if (N_DeviceInfo_IsFactoryNew())
    {
        /// \todo Propose pan id, channel, extended pan id when factory new?
        memset(payload.extendedPanId, 0, sizeof(N_Address_ExtendedPanId_t));
        payload.channel = N_DeviceInfo_GetChannelForIndex(0u, N_DeviceInfo_GetPrimaryChannelMask());
        payload.networkUpdateId = 0u;
        payload.networkAddress = 0xFFFFu;
        payload.panId = 0x0000u;
    }
    else
    {
        memcpy(payload.extendedPanId, N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(N_Address_ExtendedPanId_t));
        payload.channel = N_DeviceInfo_GetNetworkChannel();
        payload.networkUpdateId = N_DeviceInfo_GetNetworkUpdateId();
        payload.networkAddress = N_DeviceInfo_GetNetworkAddress();
        payload.panId = N_DeviceInfo_GetNetworkPanId();
    }

    payload.numberSubDevices = N_PacketDistributor_GetNumberOfZllEndpoints();
    payload.totalGroupIds = N_PacketDistributor_GetTotalGroupCountZll();
    if (payload.numberSubDevices == 1u)
    {
        payload.endPoint = N_PacketDistributor_GetZllEndpointFromIndex(0u);
        ZLL_Endpoint_t* pEndpoint = N_PacketDistributor_FindEndpoint(payload.endPoint);
        payload.profileId = pEndpoint->simpleDescriptor->AppProfileId;
        payload.deviceId = pEndpoint->simpleDescriptor->AppDeviceId;
        payload.version = pEndpoint->simpleDescriptor->AppDeviceVersion;
        payload.groupIds = N_PacketDistributor_GetGroupCount(payload.endPoint);
    }

    N_InterPan_SendScanResponse(&payload, &s_receivedScanRequest.sourceAddress);
}

static inline void ScanIndication(void)
{
    N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, ScanIndication, (&(s_receivedScanRequest.scanRequest)));
}

static inline void StartChangeDelay(void)
{
    N_Timer_Start16(CHANGE_TIMEOUT_MS, &s_timer);
}

static inline void StoreCurrentTransmitPower(void)
{
    s_currentTransmitPower = N_Radio_GetTxPower();
}

static inline void RestoreTransmitPower(void)
{
    s_radioBusy = TRUE;
    N_Radio_SetTxPower(s_currentTransmitPower, RestoreTransmitPowerDone);
}

static inline void StartTransaction_StoreCurrentTransmitPower_SendScanResponse_StartChangeDelay(void)
{
    StartTransaction();
    StoreCurrentTransmitPower();
    SendScanResponse();
    StartChangeDelay();
}

static inline void SendDeviceInfoResponse(void)
{
    N_InterPan_DeviceInfoRequest_t* pPayload = s_receivedCommand.payload.pDeviceInfoRequest;

    uint8_t numberOfZllEndpoints = N_PacketDistributor_GetNumberOfZllEndpoints();
    uint8_t devicesToSend = 0u;

    if(pPayload->startIndex < numberOfZllEndpoints)
    {
        devicesToSend = numberOfZllEndpoints - pPayload->startIndex;
    }

    if(devicesToSend > N_INTERPAN_DEVICE_INFO_RECORD_MAX)
    {
        devicesToSend = N_INTERPAN_DEVICE_INFO_RECORD_MAX;
    }

    N_InterPan_DeviceInfoResponse_t responsePayload;
    memset(&responsePayload, 0, sizeof(responsePayload));

    responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;

    responsePayload.transactionId = pPayload->transactionId;
    responsePayload.numberSubDevices = numberOfZllEndpoints;
    responsePayload.startIndex = pPayload->startIndex;
    responsePayload.count = devicesToSend;

    for(uint8_t i = 0u; i < devicesToSend; i++)
    {
        memcpy(&responsePayload.deviceInfo[i].ieeeAddress, N_DeviceInfo_GetIEEEAddress(), sizeof(responsePayload.deviceInfo[i].ieeeAddress));
        responsePayload.deviceInfo[i].endPoint = N_PacketDistributor_GetZllEndpointFromIndex(pPayload->startIndex + i);
        ZLL_Endpoint_t* pEndpoint = N_PacketDistributor_FindEndpoint(responsePayload.deviceInfo[i].endPoint);
        responsePayload.deviceInfo[i].profileId = pEndpoint->simpleDescriptor->AppProfileId;
        responsePayload.deviceInfo[i].deviceId = pEndpoint->simpleDescriptor->AppDeviceId;
        responsePayload.deviceInfo[i].version = pEndpoint->simpleDescriptor->AppDeviceVersion;
        responsePayload.deviceInfo[i].groupIds = N_PacketDistributor_GetGroupCount(pEndpoint->simpleDescriptor->endpoint);
        responsePayload.deviceInfo[i].sort = 0u;
    }

    N_InterPan_SendDeviceInfoResponse(&responsePayload, &s_receivedCommand.sourceAddress);
}

static inline void IdentifyIndication(void)
{
    uint16_t identifyTime = s_receivedCommand.payload.pIdentifyRequest->identifyTime;
    if ( identifyTime == 0u )
    {
        N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, IdentifyStopIndication, ());
    }
    else
    {
        N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, IdentifyStartIndication, (identifyTime));
    }
}

static inline void JoinNetwork(void)
{
    uint8_t *resultKey = getSecurityKeyBuffer();
    N_Security_DecryptNetworkKey(s_newNetworkParameters.keyIndex,
        s_newNetworkParameters.transactionId,
        s_newNetworkParameters.responseId,
        s_newNetworkParameters.encryptedNetworkKey,
        resultKey,
        continueJoinNetwork);
}

static void continueJoinNetwork(void)
{
  uint8_t *networkKey = extractKeyFromBuffer();
  N_Connection_JoinNetwork(
    s_newNetworkParameters.extendedPanId,
    s_newNetworkParameters.channel,
    s_newNetworkParameters.panId,
    s_newNetworkParameters.networkAddress,
    s_newNetworkParameters.networkUpdateId,
    networkKey,
    JoinNetworkDone);
  freeSecurityKeyBuffer();
}

static inline void JoinNetworkIndication(void)
{
    N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, JoinNetworkIndication, (s_newNetworkParameters.groupIdsBegin, s_newNetworkParameters.groupIdsEnd));
}

static inline void AddChild(void)
{
    if (s_networkAddressEndDevice != 0u)
    {
        N_Cmi_AddChild(s_ieeeAddressEndDevice, s_networkAddressEndDevice);
        s_networkAddressEndDevice = 0u;
    }
}

static inline void AddChild_JoinNetworkIndication(void)
{
    AddChild();
    JoinNetworkIndication();
}

static inline void DisableTarget(void)
{
    s_targetType = N_LinkTarget_TargetType_None;
    EraseResponseTable();
}

static inline void DisableTarget_SetInterPanModeOff(void)
{
    DisableTarget();
    SetInterPanModeOff();
}

static inline void UpdateNetwork(void)
{
    N_Connection_UpdateNetwork(s_newNetworkParameters.channel, s_newNetworkParameters.networkUpdateId, UpdateNetworkDone);
}

static inline void UpdateNetworkIndication(void)
{
    N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, UpdateNetworkIndication, ());
}

static inline void ResetIndication(void)
{
    N_UTIL_CALLBACK(N_LinkTarget_Callback_t, s_subscribers, ResetIndication, ());
}

static inline void FindFreePan(void)
{
    N_Connection_FindFreePan(FindFreePanDone);
}

static inline void SendNetworkStartResponseOk(void)
{
    N_InterPan_NetworkStartResponse_t responsePayload;
    responsePayload.status = ZSuccess;
    responsePayload.header.transactionSequenceNumber = s_newNetworkParameters.msgSequenceNumber;
    responsePayload.transactionId = s_newNetworkParameters.transactionId;
    memcpy(responsePayload.extendedPanId, s_newNetworkParameters.extendedPanId, sizeof(N_Address_ExtendedPanId_t));
    responsePayload.networkUpdateId = 0u;
    responsePayload.channel = s_newNetworkParameters.channel;
    responsePayload.panId = s_newNetworkParameters.panId;
    N_InterPan_SendNetworkStartResponse(&responsePayload, &(s_receivedCommand.sourceAddress));
}

static inline void StartJitterDelay(void)
{
    uint16_t scanResponseDelay = N_Util_Random() % MAX_RANDOM_DELAY_MS;
    N_Timer_Start16(scanResponseDelay, &s_timerScanJitter);
    s_radioBusy = TRUE;
}

static inline void StopTimer(void)
{
    N_Timer_Stop(&s_timer);
}

static inline void changeToMacChannel(void)
{
  N_Connection_SetTargetInterPanMacChannel();
}

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2)
{
    switch(action)
    {
    case aSetInterPanModeOnNetworkChannel:
        SetInterPanModeOnNetworkChannel();
        break;
    case aRestoreTransmitPower:
        RestoreTransmitPower();
        break;
    case aConfirmSetTargetTypeDone:
        ConfirmSetTargetTypeDone();
        break;
    case aStartTransaction_StoreCurrentTransmitPower_SendScanResponse_StartChangeDelay:
        StartTransaction_StoreCurrentTransmitPower_SendScanResponse_StartChangeDelay();
        break;
    case aSendDeviceInfoResponse:
        SendDeviceInfoResponse();
        break;
    case aIdentifyIndication:
        IdentifyIndication();
        break;
    case aStartChangeDelay:
        StartChangeDelay();
        break;
    case aJoinNetwork:
        JoinNetwork();
        break;
    case aAddChild_JoinNetworkIndication:
        AddChild_JoinNetworkIndication();
        break;
    case aUpdateNetwork:
        UpdateNetwork();
        break;
    case aUpdateNetworkIndication:
        UpdateNetworkIndication();
        break;
    case aDisableTarget_SetInterPanModeOff:
        DisableTarget_SetInterPanModeOff();
        break;
    case aDisableTarget_SetInterPanModeOff_StopTimer:
        DisableTarget_SetInterPanModeOff();
        StopTimer();
        break;
    case aResetIndication:
        ResetIndication();
        break;
    case aSendNetworkStartResponseOk_StartChangeDelay:
        SendNetworkStartResponseOk();
        StartChangeDelay();
        break;
    case aFindFreePan_StopTimer:
        FindFreePan();
        StopTimer();
        break;
    case aStartJitterDelay_ScanIndication:
        changeToMacChannel();
        StartJitterDelay();
        ScanIndication();
        break;
    case aStopTimer:
        StopTimer();
        break;
    default:
        break;
    }
    (void)arg1;
    (void)arg2;
}

/***************************************************************************************************
* STATE MACHINE CONDITION FUNCTIONS
***************************************************************************************************/

static inline bool IsActiveTransaction(void)
{
    return IsTransactionIdActive(s_receivedCommand.transactionId);
}

static inline bool IsInvalidTransaction(void)
{
    if ( IsActiveTransaction() == FALSE )
    {
        return TRUE;
    }

    return FALSE;
}

static inline bool IsValidScanRequest(void)
{
    if ( IsTransactionIdActive(s_receivedScanRequest.transactionId) )
    {
        // transaction already active
        return FALSE;
    }
    // use touchlink rssi threshold
    if (s_receivedScanRequest.rssi < N_DeviceInfo_GetTouchlinkRssiThreshold())
    {
        // rssi too low
        return FALSE;
    }
    
    return TRUE;
}

static inline bool IsJoinResponseError(void)
{
    return N_UTIL_BOOL(s_receivedCommand.payload.pNetworkJoinResponse->status != 0u);
}

static inline bool IsStartRequestPanId0OrExtPanId0(void)
{
    if (s_newNetworkParameters.panId == 0x0000u)
    {
        return TRUE;
    }
    else
    {
        return N_UTIL_BOOL(memcmp(s_newNetworkParameters.extendedPanId, c_ExtPanId00, sizeof(s_newNetworkParameters.extendedPanId)) == 0);
    }
}

static inline bool IsJitterDelayRunning(void)
{
    return N_Timer_IsRunning(&s_timerScanJitter);
}

static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    switch (condition)
    {
    case cIsTargetTypeNone:
        return IsTargetTypeNone();
    case cIsActiveTransaction:
        return IsActiveTransaction();
    case cIsValidScanRequest:
        return IsValidScanRequest();
    case cIsJoinResponseError:
        return IsJoinResponseError();
    case cIsInvalidTransaction:
        return IsInvalidTransaction();
    case cIsStartRequestPanId0OrExtPanId0:
        return IsStartRequestPanId0OrExtPanId0();
    default:
        return TRUE;
    }
}

/***************************************************************************************************
* CALLBACK FUNCTIONS
***************************************************************************************************/

/** Callback function, see \ref N_InterPan::ReceivedScanRequest. */
static void ReceivedScanRequest(N_InterPan_ScanRequest_t* pPayload, int8_t rssi, N_Address_Extended_t* pSourceAddress)
{
   // use touchlink rssi threshold
    if (rssi < N_DeviceInfo_GetTouchlinkRssiThreshold())
    {
        // rssi too low
        return;
    }
    /* Check if we are already handling a ScanRequest.
       If so, we shouldn't overwrite the values in s_receivedScanRequest
       and corrupt the ScanReponse.
    */
    if (!IsJitterDelayRunning() && !s_radioBusy)
    {
        s_receivedScanRequest.scanRequest = *pPayload;  // struct copy
        s_receivedScanRequest.transactionId = pPayload->transactionId;
        s_receivedScanRequest.rssi = rssi;
        memcpy(s_receivedScanRequest.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));

        FsmHandleEvent(eReceivedScanRequest);
    }
}

/** Callback function, see \ref N_InterPan::ReceivedDeviceInfoRequest. */
static void ReceivedDeviceInfoRequest(N_InterPan_DeviceInfoRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    // Authentication check not for DeviceInfoRequest.
    s_receivedCommand.payload.pDeviceInfoRequest = pPayload;
    s_receivedCommand.transactionId = pPayload->transactionId;
    memcpy(s_receivedCommand.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent(eReceivedDeviceInfoRequest);
}

/** Callback function, see \ref N_InterPan::ReceivedIdentifyRequest. */
static void ReceivedIdentifyRequest(N_InterPan_IdentifyRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        s_receivedCommand.payload.pIdentifyRequest = pPayload;
        s_receivedCommand.transactionId = pPayload->transactionId;
        memcpy(s_receivedCommand.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
        FsmHandleEvent(eReceivedIdentifyRequest);
    }
}

/** Callback function, see \ref N_InterPan::ReceivedNetworkJoinRouterRequest. */
static void ReceivedNetworkJoinRouterRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        if (N_DeviceInfo_IsEndDevice())
        {
            // Not supported on end devices
            N_InterPan_NetworkJoinResponse_t responsePayload;
            memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
            responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
            responsePayload.transactionId = pPayload->transactionId;
            responsePayload.status = ZFailure;
            N_InterPan_SendNetworkJoinRouterResponse(&responsePayload, pSourceAddress);
        }
        else
        {
            ZStatus_t status = ZSuccess;

            if (!s_allowStealing)
            {
                if (!N_DeviceInfo_IsFactoryNew())
                {
                    status = ZFailure;
                }
            }

            if (!IsTransactionIdActive(pPayload->transactionId))
            {
                // not an active transaction. ignore
                return;
            }

            if (!N_DeviceInfo_IsChannelInMask(pPayload->channel, N_DeviceInfo_GetPrimaryChannelMask()))
            {
                status = ZFailure;
            }
            if ((pPayload->panId == 0x0000u) || (pPayload->panId == 0xFFFFu))
            {
                status = ZFailure;
            }
            if ((pPayload->networkAddress == 0x0000u) || (pPayload->networkAddress > 0xFFF7u))
            {
                status = ZFailure;
            }
            if (memcmp(pPayload->extendedPanId, c_ExtPanId00, sizeof(N_Address_ExtendedPanId_t)) == 0)
            {
                status = ZFailure;
            }
            if (memcmp(pPayload->extendedPanId, c_ExtPanIdFF, sizeof(N_Address_ExtendedPanId_t)) == 0)
            {
                status = ZFailure;
            }

            N_Address_ExtendedPanId_t* myExtPanId = N_DeviceInfo_GetNetworkExtendedPanId();
            if (memcmp(pPayload->extendedPanId, myExtPanId, sizeof(*myExtPanId)) == 0)
            {
                // The NetworkJoinRouterRequest has my extended PAN ID - treat it as an NetworkUpdateRequest for legacy Amanda compatibility (#24)
                s_newNetworkParameters.channel = pPayload->channel;
                s_newNetworkParameters.networkUpdateId = pPayload->networkUpdateId;

                N_InterPan_NetworkJoinResponse_t responsePayload;
                memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
                
                responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
                responsePayload.transactionId = pPayload->transactionId;
                responsePayload.status = status;
                N_InterPan_SendNetworkJoinRouterResponse(&responsePayload, pSourceAddress);

                if (status == ZSuccess)
                {
                    FsmHandleEvent(eUpdateNetwork);
                }
            }
            else
            {
                N_AddressManager_Status_t addressRet = N_AddressManager_AssignAddressRange(
                    pPayload->freeNetworkAddressRangeBegin, pPayload->freeNetworkAddressRangeEnd,
                    pPayload->freeGroupIdRangeBegin, pPayload->freeGroupIdRangeEnd);
                if ( addressRet != N_AddressManager_Status_Success )
                {
                    status = ZFailure;
                }

                N_InterPan_NetworkJoinResponse_t responsePayload;
                memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
                responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
                responsePayload.transactionId = pPayload->transactionId;
                responsePayload.status = status;
                N_InterPan_SendNetworkJoinRouterResponse(&responsePayload, pSourceAddress);

                if (status == ZSuccess)
                {
                    memcpy(s_newNetworkParameters.extendedPanId, pPayload->extendedPanId, sizeof(N_Address_ExtendedPanId_t));
                    s_newNetworkParameters.channel = pPayload->channel;
                    s_newNetworkParameters.panId = pPayload->panId;
                    s_newNetworkParameters.networkAddress = pPayload->networkAddress;
                    s_newNetworkParameters.networkUpdateId = pPayload->networkUpdateId;

                    s_newNetworkParameters.groupIdsBegin = pPayload->groupIdsBegin;
                    s_newNetworkParameters.groupIdsEnd = pPayload->groupIdsEnd;

                    memcpy(s_newNetworkParameters.encryptedNetworkKey, pPayload->encryptedNetworkKey, sizeof(s_newNetworkParameters.encryptedNetworkKey));
                    s_newNetworkParameters.keyIndex = pPayload->keyIndex;
                    s_newNetworkParameters.responseId = GetFromResponseTable(pPayload->transactionId)->responseId;
                    s_newNetworkParameters.transactionId = pPayload->transactionId;

                    FsmHandleEvent(eReceivedValidNetworkJoinRequest);
                }
            }
        }
    }
}

/** Callback function, see \ref N_InterPan::ReceivedNetworkJoinEndDeviceRequest. */
static void ReceivedNetworkJoinEndDeviceRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        if (!N_DeviceInfo_IsEndDevice())
        {
            // Not supported on routers
            N_InterPan_NetworkJoinResponse_t responsePayload;
            memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
            responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
            responsePayload.transactionId = pPayload->transactionId;
            responsePayload.status = ZFailure;
            N_InterPan_SendNetworkJoinEndDeviceResponse(&responsePayload, pSourceAddress);
        }
        else
        {
            ZStatus_t status = ZSuccess;

            if (!s_allowStealing)
            {
                if (!N_DeviceInfo_IsFactoryNew())
                {
                    status = ZFailure;
                }
            }

            if (!IsTransactionIdActive(pPayload->transactionId))
            {
                // not an active transaction. ignore
                return;
            }

            if (!N_DeviceInfo_IsChannelInMask(pPayload->channel, N_DeviceInfo_GetPrimaryChannelMask()))
            {
                status = ZFailure;
            }
            if ((pPayload->panId == 0x0000u) || (pPayload->panId == 0xFFFFu))
            {
                status = ZFailure;
            }
            if ((pPayload->networkAddress == 0x0000u) || (pPayload->networkAddress > 0xFFF7u))
            {
                status = ZFailure;
            }
            if (memcmp(pPayload->extendedPanId, c_ExtPanId00, sizeof(N_Address_ExtendedPanId_t)) == 0)
            {
                status = ZFailure;
            }
            if (memcmp(pPayload->extendedPanId, c_ExtPanIdFF, sizeof(N_Address_ExtendedPanId_t)) == 0)
            {
                status = ZFailure;
            }

            N_Address_ExtendedPanId_t* myExtPanId = N_DeviceInfo_GetNetworkExtendedPanId();
            if (memcmp(pPayload->extendedPanId, myExtPanId, sizeof(*myExtPanId)) == 0)
            {
                // The NetworkJoinRouterRequest has my extended PAN ID - treat it as an NetworkUpdateRequest for legacy Amanda compatibility (#24)
                s_newNetworkParameters.channel = pPayload->channel;
                s_newNetworkParameters.networkUpdateId = pPayload->networkUpdateId;

                N_InterPan_NetworkJoinResponse_t responsePayload;
                memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
                responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
                responsePayload.transactionId = pPayload->transactionId;
                responsePayload.status = status;
                N_InterPan_SendNetworkJoinEndDeviceResponse(&responsePayload, pSourceAddress);

                if (status == ZSuccess)
                {
                    FsmHandleEvent(eUpdateNetwork);
                }
            }
            else
            {
                N_AddressManager_Status_t addressRet = N_AddressManager_AssignAddressRange(
                    pPayload->freeNetworkAddressRangeBegin, pPayload->freeNetworkAddressRangeEnd,
                    pPayload->freeGroupIdRangeBegin, pPayload->freeGroupIdRangeEnd);
                if ( addressRet != N_AddressManager_Status_Success )
                {
                    status = ZFailure;
                }

                N_InterPan_NetworkJoinResponse_t responsePayload;
                memset(&responsePayload,0,sizeof(N_InterPan_NetworkJoinResponse_t));
                responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
                responsePayload.transactionId = pPayload->transactionId;
                responsePayload.status = status;
                N_InterPan_SendNetworkJoinEndDeviceResponse(&responsePayload, pSourceAddress);

                if (status == ZSuccess)
                {
                    memcpy(s_newNetworkParameters.extendedPanId, pPayload->extendedPanId, sizeof(N_Address_ExtendedPanId_t));
                    s_newNetworkParameters.channel = pPayload->channel;
                    s_newNetworkParameters.panId = pPayload->panId;
                    s_newNetworkParameters.networkAddress = pPayload->networkAddress;
                    s_newNetworkParameters.networkUpdateId = pPayload->networkUpdateId;

                    s_newNetworkParameters.groupIdsBegin =  pPayload->groupIdsBegin;
                    s_newNetworkParameters.groupIdsEnd =  pPayload->groupIdsEnd;

                    memcpy(s_newNetworkParameters.encryptedNetworkKey, pPayload->encryptedNetworkKey, sizeof(s_newNetworkParameters.encryptedNetworkKey));
                    s_newNetworkParameters.keyIndex = pPayload->keyIndex;
                    s_newNetworkParameters.responseId = GetFromResponseTable(pPayload->transactionId)->responseId;
                    s_newNetworkParameters.transactionId = pPayload->transactionId;

                    FsmHandleEvent(eReceivedValidNetworkJoinRequest);
                }
            }
        }
    }
}

/** Callback function, see \ref N_InterPan::ReceivedNetworkStartRequest. */
static void ReceivedNetworkStartRequest(N_InterPan_NetworkStartRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        if (N_DeviceInfo_IsEndDevice())
        {
            // Not supported on end devices
            N_InterPan_NetworkStartResponse_t responsePayload;
            memset(&responsePayload,0,sizeof(N_InterPan_NetworkStartResponse_t));
            responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
            responsePayload.transactionId = pPayload->transactionId;
            responsePayload.status = ZFailure;
            N_InterPan_SendNetworkStartResponse(&responsePayload, pSourceAddress);
        }
        else if (!IsTransactionIdActive(pPayload->transactionId))
        {
            // not an active transaction. ignore
            return;
        }
        else
        {
            ZStatus_t status = ZSuccess;

            if (!s_allowStealing)
            {
                if (!N_DeviceInfo_IsFactoryNew())
                {
                    status = ZFailure;
                }
            }

            if (pPayload->channel == 0u)
            {
                // pick a random channel from the mask
                uint16_t index = N_Util_Random() % (uint16_t)N_DeviceInfo_GetNrChannelsInChannelMask(N_DeviceInfo_GetPrimaryChannelMask());
                s_newNetworkParameters.channel = N_DeviceInfo_GetChannelForIndex((uint8_t)index, N_DeviceInfo_GetPrimaryChannelMask());
            }
            else if (!N_DeviceInfo_IsChannelInMask(pPayload->channel, N_DeviceInfo_GetPrimaryChannelMask()))
            {
                status = ZFailure;
            }
            else
            {
                s_newNetworkParameters.channel = pPayload->channel;
            }

            if (pPayload->panId == 0xFFFFu)
            {
                status = ZFailure;
            }
            s_newNetworkParameters.panId = pPayload->panId;

            if ((pPayload->networkAddress == 0x0000u) || (pPayload->networkAddress > 0xFFF7u))
            {
                status = ZFailure;
            }
            s_newNetworkParameters.networkAddress = pPayload->networkAddress;

            if (memcmp(pPayload->extendedPanId, c_ExtPanIdFF, sizeof(N_Address_ExtendedPanId_t)) == 0)
            {
                status = ZFailure;
            }
            memcpy(s_newNetworkParameters.extendedPanId, pPayload->extendedPanId, sizeof(s_newNetworkParameters.extendedPanId));

            if (status == ZSuccess)
            {
                N_AddressManager_Status_t addressRet = N_AddressManager_AssignAddressRange(
                    pPayload->freeNetworkAddressRangeBegin, pPayload->freeNetworkAddressRangeEnd,
                    pPayload->freeGroupIdRangeBegin, pPayload->freeGroupIdRangeEnd);
                if ( addressRet != N_AddressManager_Status_Success )
                {
                    status = ZFailure;
                }
            }

            if (status == ZSuccess)
            {
                // NetworkStartRequest doesn't use s_receivedCommand.payload, only the sourceAddress
                memcpy(s_receivedCommand.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));

                s_newNetworkParameters.msgSequenceNumber = pPayload->header.transactionSequenceNumber;
                s_newNetworkParameters.networkUpdateId = 0u;
                s_newNetworkParameters.groupIdsBegin = pPayload->groupIdsBegin;
                s_newNetworkParameters.groupIdsEnd = pPayload->groupIdsEnd;
                s_newNetworkParameters.keyIndex = pPayload->keyIndex;
                s_newNetworkParameters.transactionId = pPayload->transactionId;
                s_newNetworkParameters.responseId = GetFromResponseTable(pPayload->transactionId)->responseId;
                memcpy(s_newNetworkParameters.encryptedNetworkKey, pPayload->encryptedNetworkKey, sizeof(s_newNetworkParameters.encryptedNetworkKey));

                s_networkAddressEndDevice = pPayload->endDeviceNetworkAddress;
                memcpy(s_ieeeAddressEndDevice, pPayload->endDeviceIeeeAddress, sizeof(s_ieeeAddressEndDevice));

                FsmHandleEvent(eReceivedValidNetworkStartRequest);
            }
            else
            {
                N_InterPan_NetworkStartResponse_t responsePayload;
                memset(&responsePayload,0,sizeof(N_InterPan_NetworkStartResponse_t));
                
                responsePayload.header.transactionSequenceNumber = pPayload->header.transactionSequenceNumber;
                responsePayload.transactionId = pPayload->transactionId;
                responsePayload.status = ZFailure;
                N_InterPan_SendNetworkStartResponse(&responsePayload, pSourceAddress);
            }
        }
    }
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkJoinRouterResponse.
Used for both ReceivedNetworkJoinRouterResponse and ReceivedNetworkJoinEndDeviceResponse. */
static void ReceivedNetworkJoinResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        s_receivedCommand.payload.pNetworkJoinResponse = pPayload;
        s_receivedCommand.transactionId = pPayload->transactionId;
        memcpy(s_receivedCommand.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
        FsmHandleEvent(eReceivedNetworkJoinResponse);
    }
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkUpdateRequest. */
static void ReceivedNetworkUpdateRequest(N_InterPan_NetworkUpdateRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        N_Address_ExtendedPanId_t* thisNetworkExtendedPanId = N_DeviceInfo_GetNetworkExtendedPanId();
        uint16_t thisPanId = N_DeviceInfo_GetNetworkPanId();
        uint8_t thisNetworkUpdateId = N_DeviceInfo_GetNetworkUpdateId();

        if ( memcmp(pPayload->extendedPanId, thisNetworkExtendedPanId, sizeof(N_Address_ExtendedPanId_t)) != 0 )
        {
            // invalid extended pan id. ignore
            return;
        }
        if ( pPayload->panId != thisPanId )
        {
            // invalid pan id. ignore
            return;
        }
        if ( N_Util_HighestNetworkUpdateId(pPayload->networkUpdateId, thisNetworkUpdateId) != pPayload->networkUpdateId )
        {
            // invalid network update id, it must be the received id must be higher or equal to the current one. ignore
            return;
        }

        s_newNetworkParameters.channel = pPayload->channel;
        s_newNetworkParameters.networkUpdateId = pPayload->networkUpdateId;

        FsmHandleEvent(eUpdateNetwork);
    }
    (void)pSourceAddress;
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedResetToFactoryNewRequest. */
static void ReceivedResetToFactoryNewRequest(N_InterPan_ResetToFactoryNewRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    if (IsAuthenticationCorrectOrNotRequired(pPayload->transactionId))
    {
        FsmHandleEvent(eReceivedResetToFactoryNewRequest);
    }
    (void)pSourceAddress;
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Interface function, see \ref N_LinkTarget_Subscribe. */
void N_LinkTarget_Subscribe_Impl(const N_LinkTarget_Callback_t* pCb)
{
    N_ERRH_ASSERT_FATAL(s_taskId != N_TASK_ID_NONE);    // check if initialized
    N_UTIL_CALLBACK_SUBSCRIBE(N_LinkTarget_Callback_t, s_subscribers, pCb);
}

/** Interface function, see \ref N_LinkTarget_SetTargetType. */
void N_LinkTarget_SetTargetType_Impl(N_LinkTarget_TargetType_t targetType, N_LinkTarget_SetTargetTypeDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL(s_pfSetTargetTypeDone == NULL);
    s_pfSetTargetTypeDone = pfDoneCallback;

    s_targetType = targetType;
    FsmHandleEvent(eSetTargetType);
}

/** interface function, see \ref N_LinkTarget_SetTouchlinkPriority. */
void N_LinkTarget_SetTouchlinkPriority_Impl(bool priorityEnabled)
{
    s_touchlinkPriorityEnabled = priorityEnabled;
}

/** Interface function, see \ref N_LinkTarget_AllowStealing. */
void N_LinkTarget_AllowStealing_Impl(bool allowStealing)
{
    s_allowStealing = allowStealing;
}

bool N_LinkTarget_EventHandler(N_Task_Event_t evt)
{
    switch (evt)
    {
    case EVENT_TIMER:
        FsmHandleEvent(eTimer);
        break;
    case EVENT_TRANSACTION_ID_TIMEOUT:
        EraseResponseTable();
        break;
    case EVENT_JOIN_NETWORK_DONE:
        FsmHandleEvent(eJoinNetworkDone);
        break;
    case EVENT_UPDATE_NETWORK_DONE:
        FsmHandleEvent(eUpdateNetworkDone);
        break;
    case EVENT_SET_INTERPAN_MODE_DONE:
        FsmHandleEvent(eSetInterPanModeDone);
        break;
    case EVENT_SCAN_JITTER:
        s_radioBusy = FALSE;
        FsmHandleEvent(eScanResponseJitterDelay);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void N_LinkTarget_Init(void)
{
    s_taskId = N_Task_GetIdFromEventHandler(N_LinkTarget_EventHandler);

    s_timer.task = s_taskId;
    s_timer.evt = EVENT_TIMER;
    s_timerTransactionIdTimeout.task = s_taskId;
    s_timerTransactionIdTimeout.evt = EVENT_TRANSACTION_ID_TIMEOUT;
    s_timerScanJitter.task = s_taskId;
    s_timerScanJitter.evt = EVENT_SCAN_JITTER;

    static const N_InterPan_Callback_t s_N_InterPan_Callback =
    {
        ReceivedScanRequest,
        NULL,   // ReceivedScanResponse
        ReceivedDeviceInfoRequest,
        NULL,   // ReceivedDeviceInfoResponse
        ReceivedIdentifyRequest,
        ReceivedNetworkStartRequest,
        NULL,   // ReceivedNetworkStartResponse
        ReceivedNetworkJoinRouterRequest,
        ReceivedNetworkJoinResponse,
        ReceivedNetworkJoinEndDeviceRequest,
        ReceivedNetworkJoinResponse,
        ReceivedNetworkUpdateRequest,
        ReceivedResetToFactoryNewRequest,
        NULL,   // ReceivedInterPanCommand
        -1
    };
    N_InterPan_Subscribe(&s_N_InterPan_Callback);

    N_FSM_INITIALIZE(&s_fsm, &s_currentState, sDisabled);
}
