/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_LinkInitiator.c 24615 2013-02-27 05:14:24Z arazinkov $

***************************************************************************************************/

/***************************************************************************************************
* LOCAL INCLUDE FILES
***************************************************************************************************/

#include "N_LinkInitiator_Bindings.h"
#include "N_LinkInitiator_Init.h"
#include "N_LinkInitiator.h"

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
#include "N_Zdp.h"
#include "N_Types.h"
#include "N_Util.h"

#include "devconfig.h"

/***************************************************************************************************
* LOCAL MACROS AND CONSTANTS
***************************************************************************************************/

#define COMPID "N_LinkInitiator"

/** Maximum number of subscribers to this component */
#if (!defined(N_LINK_INITIATOR_MAX_SUBSCRIBERS))
#   define N_LINK_INITIATOR_MAX_SUBSCRIBERS   1u
#endif

// N_Task Event identifiers, to be aligned with N_LinkInitiator_Tests.cs
#define EVENT_TIMER                         0u
#define EVENT_JOIN_DEVICE_FAILED            1u
#define EVENT_UPDATE_NETWORK_DONE           2u
#define EVENT_JOIN_NETWORK_SUCCESS          4u
#define EVENT_JOIN_NETWORK_FAILURE          5u
#define EVENT_DEVICE_INFO_FAILURE           6u
#define EVENT_DEVICE_INFO_SUCCESS           7u
#define EVENT_SET_INTERPAN_MODE_DONE        8u

/** The number of scan requests to send on the first channel in the channel mask. */
#define NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL 5u

/** Time to wait after sending a scan request (in milliseconds). */
#define SCAN_REQUEST_WAIT_TIME_MS       250u

/** Time to wait between sending a ZigBee message and changing channels/network. */
#define CHANGE_TIMEOUT_MS               100u

/** Time to wait for a response after sending a request. */
#define RESPONSE_TIMEOUT_MS             5000u

/** Timeout to wait for unicast response, should be short for fast retries. */
#define TIMEOUT_UNICAST_RESPONSE_MS     500u

/** Number of retries for trying to reach the LinkTarget via a ZDP command after linking it. */
#define NUM_UNICAST_RETRIES             6u

/** Time to wait for a message to be sent (in milliseconds). */
#define WAIT_FOR_MESSAGE_SENT           250u

/** Time to wait for the initiator to wait until the target has completed its startup procedure [msec]. */
#define WAIT_MIN_STARTUP_WAIT_TIME_MS   2000u

/***************************************************************************************************
* LOCAL TYPES
***************************************************************************************************/

typedef struct ReceivedScanResponse_t
{
    N_InterPan_ScanResponse_t* pScanResponse;
    int8_t rssi;
    N_Address_Extended_t sourceAddress;
} ReceivedScanResponse_t;

typedef struct ReceivedNetworkJoinResponse_t
{
    N_InterPan_NetworkJoinResponse_t networkJoinResponse;
    N_Address_Extended_t sourceAddress;
} ReceivedNetworkJoinResponse_t;

typedef struct ReceivedNetworkStartResponse_t
{
    N_InterPan_NetworkStartResponse_t networkStartResponse;
    N_Address_Extended_t sourceAddress;
} ReceivedNetworkStartResponse_t;

typedef struct ReceivedDeviceInfoResponse_t
{
    N_InterPan_DeviceInfoResponse_t* pDeviceInfoResponse;
    N_Address_Extended_t sourceAddress;
} ReceivedDeviceInfoResponse_t;

typedef struct ReceivedNetworkJoinRequest_t
{
    N_InterPan_NetworkJoinRequest_t networkJoinRequest;
    N_Address_Extended_t sourceAddress;
} ReceivedNetworkJoinRequest_t;

typedef enum ReceivedMessageType_t
{
    ReceivedMessageType_None,
    ReceivedMessageType_ScanResponse,
    ReceivedMessageType_NetworkJoinResponse,
    ReceivedMessageType_NetworkStartResponse,
    ReceivedMessageType_DeviceInfoResponse,
    ReceivedMessageType_NetworkJoinRequest,
} ReceivedMessageType_t;

typedef union ReceivedInterPanMessage_t
{
    ReceivedScanResponse_t scanResponse;
    ReceivedNetworkJoinResponse_t networkJoinResponse;
    ReceivedNetworkStartResponse_t networkStartResponse;
    ReceivedDeviceInfoResponse_t deviceInfoResponse;
    ReceivedNetworkJoinRequest_t networkJoinRequest;
} ReceivedInterPanMessage_t;

typedef struct _SecurityKeyBuffer_t
{
  N_Security_Key_t key;
  bool busy;
} SecurityKeyBuffer_t;

/***************************************************************************************************
* LOCAL VARIABLES
***************************************************************************************************/

static N_Task_Id_t s_taskId = N_TASK_ID_NONE;

static N_LinkInitiator_ScanDone_t s_scanDoneCallback = NULL;
static N_LinkInitiator_JoinDeviceDone_t s_joinDeviceDoneCallback = NULL;
static N_LinkInitiator_IdentifyRequestDone_t s_identifyRequestDoneCallback = NULL;
static N_LinkInitiator_DeviceInfoRequestDone_t s_deviceInfoRequestDoneCallback = NULL;
static N_LinkInitiator_ResetDeviceToFactoryNewDone_t s_resetToFactoryNewDoneCallback = NULL;

/** Current N_FSM state. */
static uint8_t s_currentState;

/* Timers */
static N_Timer_t s_timer;

N_UTIL_CALLBACK_DECLARE(N_LinkInitiator_Callback_t, s_subscribers, N_LINK_INITIATOR_MAX_SUBSCRIBERS);

/** The list of devices found during a scan, or the single target device of the other actions. */
static N_LinkInitiator_Device_t* s_deviceArray = NULL;
/** The size of the device array used during a scan (not used for the other actions). */
static uint8_t s_deviceArraySize = 0u;

/** The number of scan responses in the device array. Never larger than \ref s_deviceArraySize. */
static uint8_t s_scanResponseCount = 0u;

/** The transaction id of the scan. */
static uint32_t s_scanTransactionId = 0uL;

/** The number of scan requests sent during the current scan. */
static uint8_t s_scanRequestCount = 0u;

/** The type of scan. */
static N_LinkInitiator_ScanType_t s_scanType;

/** The identifyTime to send in the identify request. */
static uint16_t s_identifyTimeoutInSec = 0u;

/** The list of endpoints infomation found during the DeviceInfoRequest. */
static N_InterPan_DeviceInfo_t* s_endpointInfoArray = NULL;
/** The startindex of the deviceInfo requests. */
static uint8_t s_deviceInfoStartIndex = 0u;

static ReceivedMessageType_t s_receivedMessageType = ReceivedMessageType_None;
static ReceivedInterPanMessage_t s_receivedInterPanMessage; // of the type as indicated by s_receivedMessageType

// network parameters of this device.
// used for JoinDevice, valid after sending the network join request
static N_Address_ExtendedPanId_t s_thisExtendedPanId;
static uint8_t s_thisChannel;
static uint16_t s_thisPanId;
static uint16_t s_thisNetworkAddress;
static uint8_t s_thisNetworkUpdateId;
static uint8_t s_thisKeyIndex;
static N_Security_Key_t s_thisEncryptedNetworkKey;

/** Target network address used for sending the dummy ZigBee message. */
static uint16_t s_targetAddress;

static uint8_t s_unicastRetryCounter;
static uint8_t s_unicastLastSentSeqNr;
static uint8_t s_unicastLastReceivedSeqNr;

/** Previous transmit power used for restoring the transmitpower after ScanRequests. */
static uint8_t s_currentTransmitPower;

static bool s_skipCommunicationCheck = FALSE;

/** ZLL info filled during ScanRequest sending */
static uint8_t zllInfo = 0u;

static SecurityKeyBuffer_t keyBuffer =
{
  .busy = false
};


/***************************************************************************************************
* LOCAL FUNCTION DECLARATIONS
***************************************************************************************************/

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2);
static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2);
static void JoinNetworkDone(N_Connection_Result_t result);
static void UpdateNetworkDone(void);
static inline bool IsOtherUpdateIdHigher(void);
static void FindFreePanDone(uint16_t panId, N_Address_ExtendedPanId_t extendedPanId);
static void continueBroadcastScanRequest(void);
static void continueJoinNetwork(void);
static void sendNetworkJoinRequestKeyEncrypted(void);
static void continueSendNetworkJoinRequest(void);
static void sendNetworkStartRequestKeyEncrypted(void);
static void continueSendNetworkStartRequest(void);

/***************************************************************************************************
* STATE MACHINE
***************************************************************************************************/

enum N_LinkInitiator_states // states
{
    sIdle,
    sScanning,
    sScanningDoneWaitForInterPanModeOff,
    sScanningStoppedWaitForInterPanModeOff,
    sIdentifyWaitForInterPanModeOnTargetChannel,
    sIdentifySendingRequest,
    sIdentifyWaitForInterPanModeOff,
    sDeviceInfoWaitForInterPanModeOnTargetChannel,
    sDeviceInfoSendingRequest,
    sDeviceInfoFailWaitForInterPanModeOff,
    sDeviceInfoWaitForInterPanModeOff,
    sJoiningWaitForInterPanModeOnTargetChannel,
    sJoiningWaitForInterPanModeOff,
    sFindingFreePan,
    sStartingNetwork,
    sJoiningWaitForResponse,
    sStartingWaitForResponse,
    sJoiningWaitForStartupDelay,
    sJoiningWaitForJoinNetwork,
    sWaitingForStartupDelay,
    sWaitingForUnicastResponse,
    sJoiningSuccessWaitForInterPanModeOff,
    sJoiningFailWaitForInterPanModeOff,
    sSendingNetworkUpdateRequest,
    sUpdatingWaitForInterPanModeOff,
    sJoiningWaitForUpdateNetwork,    
    sResetToFactoryNewWaitForInterPanModeOnTargetChannel,
    sResetToFactoryNewRequestSending,
    sResetToFactoryNewWaitForInterPanModeOff,
};

enum N_LinkInitiator_events // events
{
    eTimer,
    eScan,
    eJoinDevice,
    eSetInterPanModeDone,
    eReceivedScanResponse,
    eStopScan,
    eIdentifyRequest,
    eDeviceInfoRequest,
    eReceivedNetworkJoinResponse,
    eReceivedNetworkStartResponse,
    eJoinNetworkSuccess,
    eJoinNetworkFail,
    eUpdateNetworkDone,
    eUnicastResponse,
    eReceivedNetworkJoinRouterRequest,
    eReceivedNetworkJoinEndDeviceRequest,
    eReceivedDeviceInfoResponse,
    eResetDeviceToFactoryNew,
    eFoundFreePan,
};

enum N_LinkInitiator_actions // actions
{
    aSetInterPanModeOnFirstChannel_StoreCurrentTransmitPower,
    aSetInterPanModeOnTargetChannel,
    aSetInterPanModeOnNextChannel,
    aSetInterPanModeOff,
    aSetInterPanModeOff_StopTimer,
    aSetInterPanModeOff_RestoreTransmitPower,
    aSetInterPanModeOff_RestoreTransmitPower_StopTimer,
    aBroadcastScanRequest,
    aStoreScanResponse,
    aScanFinishedSuccess,
    aScanFinishedStopped,
    aSendIdentifyRequest,
    aIdentifySent,
    aSendNetworkJoinRequest,
    aSendNetworkStartRequest,
    aStartNetwork,
    aJoinNetwork,
    aJoinDeviceSuccess_StopTimer,
    aJoinDeviceFail,
    aJoinDeviceCommunicationNotEstablished,
    aUseStartParams_SetInterPanModeOff_StopTimer,
    aSendDummyUnicastMessage,
    aReconnectUrgent_SendDummyUnicastMessage_InitRetryCounter,
    aReconnectUrgent_JoinDeviceCommunicationNotEstablished,
    aSendNetworkUpdateRequest,
    aUpdateNetwork,
    aSendDeviceInfoRequest,
    aStoreDeviceInfo_SendDeviceInfoRequest,
    aStoreDeviceInfo_SetInterPanModeOff_StopTimer,
    aDeviceInfoSuccess,
    aDeviceInfoFail,
    aSendResetToFactoryNewRequest,
    aResetToFactoryNewDone,
    aFindFreePan,
    aStartStartupDelayTimer,
    aResetResponseTimeoutTimer,
};

enum N_LinkInitiator_conditions // conditions
{
    cIsScanDone,
    cIsScanResponseValid,
    cIsJoinResponseInvalid,
    cIsJoinResponseError,
    cIsStartResponseInvalid,
    cIsStartResponseError,
    cIsThisFactoryNewEndDevice,
    cIsThisFactoryNewRouter,
    cAnotherRetry,
    cIsSamePan,
    cIsOtherUpdateIdHigher,
    cIsJoinRequestValid,
    cIsDeviceInfoInvalid,
    cIsDeviceInfoComplete,
    cIsResponseToLastMessage,
    cSkipCommunicationCheck,
};

/** \image html N_LinkInitiator_Fsm.png */
/* @Fsm2PlantUml:begin file=N_LinkInitiator_Fsm.png */

static const N_FSM_Transition_t s_transitionTable[] =
{
//     event                                condF                               actF                        newState
N_FSM_STATE( sIdle ),
N_FSM( eScan,                               N_FSM_NONE,                         aSetInterPanModeOnFirstChannel_StoreCurrentTransmitPower,   sScanning ),
N_FSM( eIdentifyRequest,                    N_FSM_NONE,                         aSetInterPanModeOnTargetChannel,  sIdentifyWaitForInterPanModeOnTargetChannel ),
N_FSM( eDeviceInfoRequest,                  N_FSM_NONE,                         aSetInterPanModeOnTargetChannel,  sDeviceInfoWaitForInterPanModeOnTargetChannel ),
N_FSM( eJoinDevice,                         cIsThisFactoryNewRouter,            aFindFreePan,                     sFindingFreePan ),
N_FSM( eJoinDevice,                         N_FSM_ELSE,                         aSetInterPanModeOnTargetChannel,  sJoiningWaitForInterPanModeOnTargetChannel ),
N_FSM( eResetDeviceToFactoryNew,            N_FSM_NONE,                         aSetInterPanModeOnTargetChannel,  sResetToFactoryNewWaitForInterPanModeOnTargetChannel ),

// Scan

N_FSM_STATE( sScanning ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aBroadcastScanRequest,      N_FSM_SAME_STATE ),
N_FSM( eTimer,                              cIsScanDone,                        aSetInterPanModeOff_RestoreTransmitPower,        sScanningDoneWaitForInterPanModeOff ),
N_FSM( eTimer,                              N_FSM_ELSE,                         aSetInterPanModeOnNextChannel, N_FSM_SAME_STATE ),
N_FSM( eReceivedScanResponse,               cIsScanResponseValid,               aStoreScanResponse,         N_FSM_SAME_STATE ),
N_FSM( eStopScan,                           N_FSM_NONE,                         aSetInterPanModeOff_RestoreTransmitPower_StopTimer, sScanningStoppedWaitForInterPanModeOff ),

N_FSM_STATE( sScanningDoneWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aScanFinishedSuccess,       sIdle ),

N_FSM_STATE( sScanningStoppedWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aScanFinishedStopped,       sIdle ),

// Identify

N_FSM_STATE( sIdentifyWaitForInterPanModeOnTargetChannel ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aSendIdentifyRequest,       sIdentifySendingRequest ),

N_FSM_STATE( sIdentifySendingRequest ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sIdentifyWaitForInterPanModeOff ),

N_FSM_STATE( sIdentifyWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aIdentifySent,              sIdle ),

// ResetToFactoryNew

N_FSM_STATE( sResetToFactoryNewWaitForInterPanModeOnTargetChannel ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aSendResetToFactoryNewRequest, sResetToFactoryNewRequestSending ),

N_FSM_STATE( sResetToFactoryNewRequestSending ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sResetToFactoryNewWaitForInterPanModeOff ),

N_FSM_STATE( sResetToFactoryNewWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aResetToFactoryNewDone,     sIdle ),

// DeviceInfo

N_FSM_STATE( sDeviceInfoWaitForInterPanModeOnTargetChannel ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aSendDeviceInfoRequest,     sDeviceInfoSendingRequest ),

N_FSM_STATE( sDeviceInfoSendingRequest ),
N_FSM( eReceivedDeviceInfoResponse,         cIsDeviceInfoInvalid,               aSetInterPanModeOff_StopTimer, sDeviceInfoFailWaitForInterPanModeOff ),
N_FSM( eReceivedDeviceInfoResponse,         cIsDeviceInfoComplete,              aStoreDeviceInfo_SetInterPanModeOff_StopTimer, sDeviceInfoWaitForInterPanModeOff ),
N_FSM( eReceivedDeviceInfoResponse,         N_FSM_ELSE,                         aStoreDeviceInfo_SendDeviceInfoRequest, N_FSM_SAME_STATE ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sDeviceInfoFailWaitForInterPanModeOff ),

N_FSM_STATE( sDeviceInfoWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aDeviceInfoSuccess,         sIdle ),

N_FSM_STATE( sDeviceInfoFailWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aDeviceInfoFail,            sIdle ),

// Join

N_FSM_STATE( sJoiningWaitForInterPanModeOnTargetChannel ),
N_FSM( eSetInterPanModeDone,                cIsThisFactoryNewEndDevice,         aSendNetworkStartRequest,   sStartingWaitForResponse ),
N_FSM( eSetInterPanModeDone,                cIsSamePan,                         aSendNetworkUpdateRequest,  sSendingNetworkUpdateRequest ),
N_FSM( eSetInterPanModeDone,                N_FSM_ELSE,                         aSendNetworkJoinRequest,    sJoiningWaitForResponse ),

N_FSM_STATE( sFindingFreePan ),
N_FSM( eFoundFreePan,                       N_FSM_NONE,                         aStartNetwork,              sStartingNetwork ),

N_FSM_STATE( sStartingNetwork ),
N_FSM( eJoinNetworkSuccess,                 N_FSM_NONE,                         aSetInterPanModeOnTargetChannel, sJoiningWaitForInterPanModeOnTargetChannel ),
N_FSM( eJoinNetworkFail,                    N_FSM_NONE,                         aJoinDeviceFail,            sIdle ),
N_FSM( eTimer,                              N_FSM_NONE,                         aJoinDeviceFail,            sIdle ),

N_FSM_STATE( sSendingNetworkUpdateRequest ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sUpdatingWaitForInterPanModeOff ),

N_FSM_STATE( sJoiningWaitForResponse ),
N_FSM( eReceivedNetworkJoinResponse,        cIsJoinResponseInvalid,             N_FSM_NONE,                 N_FSM_SAME_STATE ),
N_FSM( eReceivedNetworkJoinResponse,        cIsJoinResponseError,               aSetInterPanModeOff_StopTimer, sJoiningFailWaitForInterPanModeOff ),
N_FSM( eReceivedNetworkJoinResponse,        N_FSM_ELSE,                         aSetInterPanModeOff_StopTimer, sJoiningSuccessWaitForInterPanModeOff ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sJoiningFailWaitForInterPanModeOff ),

N_FSM_STATE( sStartingWaitForResponse ),
N_FSM( eReceivedNetworkStartResponse,       cIsStartResponseInvalid,            N_FSM_NONE,                 N_FSM_SAME_STATE ),
N_FSM( eReceivedNetworkStartResponse,       cIsStartResponseError,              aSetInterPanModeOff_StopTimer, sJoiningFailWaitForInterPanModeOff ),
N_FSM( eReceivedNetworkStartResponse,       N_FSM_ELSE,                         aUseStartParams_SetInterPanModeOff_StopTimer, sJoiningWaitForInterPanModeOff ),
N_FSM( eTimer,                              N_FSM_NONE,                         aSetInterPanModeOff,        sJoiningFailWaitForInterPanModeOff ),

N_FSM_STATE( sUpdatingWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                cIsOtherUpdateIdHigher,             aUpdateNetwork,             sJoiningWaitForUpdateNetwork ),
N_FSM( eSetInterPanModeDone,                N_FSM_ELSE,                         aStartStartupDelayTimer,    sWaitingForStartupDelay ),

N_FSM_STATE( sJoiningWaitForUpdateNetwork ),
N_FSM( eUpdateNetworkDone,                  cSkipCommunicationCheck,            aReconnectUrgent_JoinDeviceCommunicationNotEstablished, sIdle ),
N_FSM( eUpdateNetworkDone,                  N_FSM_ELSE,                         aReconnectUrgent_SendDummyUnicastMessage_InitRetryCounter, sWaitingForUnicastResponse ),

// wait for inter-pan mode to be turned off before joining the network
N_FSM_STATE( sJoiningWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aStartStartupDelayTimer,    sJoiningWaitForStartupDelay ),

// wait for the target to complete its startup procedure
N_FSM_STATE( sJoiningWaitForStartupDelay ),
N_FSM( eTimer,                              N_FSM_NONE,                         aJoinNetwork,               sJoiningWaitForJoinNetwork ),

// wait for the initiator to join the network
N_FSM_STATE( sJoiningWaitForJoinNetwork ),
N_FSM( eJoinNetworkSuccess,                 cSkipCommunicationCheck,            aReconnectUrgent_JoinDeviceCommunicationNotEstablished, sIdle ),
N_FSM( eJoinNetworkSuccess,                 N_FSM_ELSE,                         aReconnectUrgent_SendDummyUnicastMessage_InitRetryCounter, sWaitingForUnicastResponse ),
N_FSM( eJoinNetworkFail,                    N_FSM_NONE,                         aJoinDeviceFail,            sIdle ), //Should go to previous settings?

// wait for the target to complete its startup procedure
N_FSM_STATE( sWaitingForStartupDelay ),
N_FSM( eTimer,                              cSkipCommunicationCheck,            aReconnectUrgent_JoinDeviceCommunicationNotEstablished, sIdle ),
N_FSM( eTimer,                              N_FSM_ELSE,                         aReconnectUrgent_SendDummyUnicastMessage_InitRetryCounter, sWaitingForUnicastResponse ),

// wait for a unicast response; retry often
N_FSM_STATE( sWaitingForUnicastResponse ),
N_FSM( eUnicastResponse,                    cIsResponseToLastMessage,           aJoinDeviceSuccess_StopTimer, sIdle ),
N_FSM( eUnicastResponse,                    N_FSM_ELSE,                         aResetResponseTimeoutTimer, N_FSM_SAME_STATE ),
N_FSM( eTimer,                              cAnotherRetry,                      aSendDummyUnicastMessage,   N_FSM_SAME_STATE ),
N_FSM( eTimer,                              N_FSM_ELSE,                         aJoinDeviceCommunicationNotEstablished, sIdle ),

// wait for inter-pan mode to be turned off and return success
N_FSM_STATE( sJoiningSuccessWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aStartStartupDelayTimer,    sWaitingForStartupDelay ),

// wait for inter-pan mode to be turned off and return error
N_FSM_STATE( sJoiningFailWaitForInterPanModeOff ),
N_FSM( eSetInterPanModeDone,                N_FSM_NONE,                         aJoinDeviceFail,            sIdle ),

};

N_FSM_DECLARE(s_fsm,
              s_transitionTable,
              N_FSM_TABLE_SIZE(s_transitionTable),
              NULL,
              0u,
              PerformAction,
              CheckCondition);

/* @Fsm2PlantUml:end  */

/***************************************************************************************************
* LOCAL FUNCTIONS
***************************************************************************************************/

static void FsmHandleEvent(uint8_t e)
{
    (void) N_FSM_PROCESS_EVENT(&s_fsm, &s_currentState, e);
}

/** Helper function to abort the JoinDevice in case of an address allocation error. */
static void AbortJoinDevice(void)
{
    // fake a response timeout.
    N_Task_SetEvent(s_taskId, EVENT_TIMER);
}

static void SetInterPanModeDone(void)
{
    N_Task_SetEvent(s_taskId, EVENT_SET_INTERPAN_MODE_DONE);
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

/***************************************************************************************************
* STATE MACHINE ACTION FUNCTIONS
***************************************************************************************************/

static inline void SetInterPanModeOnFirstChannel(void)
{
    s_scanRequestCount = 0u;
    N_Connection_SetInitiatorInterPanModeOn(N_DeviceInfo_GetChannelForIndex(0u, N_DeviceInfo_GetPrimaryChannelMask()), SetInterPanModeDone);
}

static inline void SetInterPanModeOnTargetChannel(void)
{
    N_Connection_SetInitiatorInterPanModeOn(s_deviceArray->scanResponse.channel, SetInterPanModeDone);
}

static inline void SetInterPanModeOnNextChannel(void)
{
    uint8_t channelIndex;
    uint8_t channel;

    uint8_t primaryChannelCount = N_DeviceInfo_GetNrChannelsInChannelMask(N_DeviceInfo_GetPrimaryChannelMask());
    if ( s_scanRequestCount < (primaryChannelCount + (NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL - 1u)) )
    {
        // get the next primary channel...

        if ( s_scanRequestCount < NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL )
        {
            // use the first primary channel
            channelIndex = 0u;
        }
        else
        {
            // use one of the other primary channels
            channelIndex = s_scanRequestCount - (NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL - 1u);
        }
        channel = N_DeviceInfo_GetChannelForIndex(channelIndex, N_DeviceInfo_GetPrimaryChannelMask());
    }
    else
    {
        // use one of the secondary channels
        channelIndex = (s_scanRequestCount - (NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL - 1u)) - primaryChannelCount;
        channel = N_DeviceInfo_GetChannelForIndex(channelIndex, N_DeviceInfo_GetSecondaryChannelMask());
    }

    N_Connection_SetInitiatorInterPanModeOn(channel, SetInterPanModeDone);
}

static inline void SetInterPanModeOff(void)
{
    N_Connection_SetInitiatorInterPanModeOff(SetInterPanModeDone);
}

static inline void ScanFinishedSuccess(void)
{
    N_ERRH_ASSERT_FATAL(s_scanDoneCallback != NULL);
    s_scanDoneCallback(N_LinkInitiator_Status_Ok, s_scanResponseCount);
    s_scanDoneCallback = NULL;
}

static inline void ScanFinishedStopped(void)
{
    N_ERRH_ASSERT_FATAL(s_scanDoneCallback != NULL);
    s_scanDoneCallback(N_LinkInitiator_Status_Stopped, s_scanResponseCount);
    s_scanDoneCallback = NULL;
}

static inline void BroadcastScanRequest(void)
{
    zllInfo = 0u;

    if(N_DeviceInfo_IsAddressAssignmentCapable())
    {
        zllInfo |= N_INTERPAN_ZLL_INFO_ADDRESS_ASSIGNMENT;
    }

    if (N_DeviceInfo_IsFactoryNew())
    {
        zllInfo |= N_INTERPAN_ZLL_INFO_FACTORY_NEW;
    }

    if((s_scanType & N_LinkInitiator_ScanType_Touchlink) != 0u)
    {
        zllInfo |= N_INTERPAN_ZLL_INFO_TOUCHLINK_INITIATOR;
        // Set transmit power to 0 dBm
        N_Radio_SetTxPower(N_DeviceInfo_GetTouchlinkZerodBmTransmitPower(), continueBroadcastScanRequest);
    }
}

static void continueBroadcastScanRequest(void)
{
  N_InterPan_ScanRequest_t scanRequest;

  scanRequest.transactionId = s_scanTransactionId;

  if (N_DeviceInfo_IsEndDevice())
  {
      scanRequest.zigBeeInfo = N_INTERPAN_ZIGBEE_INFO_END_DEVICE;
  }
  else
  {
      scanRequest.zigBeeInfo = N_INTERPAN_ZIGBEE_INFO_ROUTER | N_INTERPAN_ZIGBEE_INFO_RX_ON_IDLE;
  }

  scanRequest.zllInfo = zllInfo;

  N_InterPan_BroadcastScanRequest(&scanRequest);

  N_Timer_Start16(SCAN_REQUEST_WAIT_TIME_MS, &s_timer);

  s_scanRequestCount++;
}

static inline void StoreScanResponse(void)
{
    uint8_t target = 0u;

    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_ScanResponse);
  
    uint8_t priorityCorrection = 0u;
    // If Touchlink Priority bit is set. corrected rssi increased to give the response a higher priority
    if((s_receivedInterPanMessage.scanResponse.pScanResponse->zllInfo  & N_INTERPAN_ZLL_INFO_TOUCHLINK_PRIORITY_REQUEST) != 0u)
    {
        priorityCorrection = 128u;
    }
    int16_t correctedRssi = s_receivedInterPanMessage.scanResponse.rssi + (int16_t) s_receivedInterPanMessage.scanResponse.pScanResponse->touchlinkRssiCorrection + (int16_t) priorityCorrection;
    while ( target < s_scanResponseCount )
    {
        int16_t correctedRssiTarget = s_deviceArray[target].rssi + (int16_t) s_deviceArray[target].scanResponse.touchlinkRssiCorrection;
        if ( correctedRssi > correctedRssiTarget )
        {
            break;
        }
        target++;
    }

    if ( target < s_deviceArraySize )
    {
        for ( uint8_t i = s_scanResponseCount; i > target; i-- )
        {
            if ( i < s_deviceArraySize )
            {
                s_deviceArray[i] = s_deviceArray[i - 1u];
            }
        }
        memcpy(s_deviceArray[target].ieeeAddress, s_receivedInterPanMessage.scanResponse.sourceAddress, sizeof(N_Address_Extended_t));
        s_deviceArray[target].scanResponse = *s_receivedInterPanMessage.scanResponse.pScanResponse;
        s_deviceArray[target].rssi = s_receivedInterPanMessage.scanResponse.rssi;
        if ( s_scanResponseCount < s_deviceArraySize)
        {
            s_scanResponseCount++;
        }
    }
    else
    {
        // ignore scan response
    }
}

static inline void SendIdentifyRequest(void)
{
    N_InterPan_IdentifyRequest_t identifyRequest;
    identifyRequest.transactionId = s_deviceArray->scanResponse.transactionId;
    identifyRequest.identifyTime = s_identifyTimeoutInSec;
    N_InterPan_SendIdentifyRequest(&identifyRequest, &s_deviceArray->ieeeAddress);

    N_Timer_Start16(CHANGE_TIMEOUT_MS, &s_timer);
}

static inline void IdentifySent(void)
{
    N_ERRH_ASSERT_FATAL(s_identifyRequestDoneCallback != NULL);
    s_identifyRequestDoneCallback(N_LinkInitiator_Status_Ok);
    s_identifyRequestDoneCallback = NULL;
}

static inline void StartNetwork(void)
{
    uint8_t myTotalGroupCount = N_PacketDistributor_GetTotalGroupCount();
    uint16_t myFirstGroupId = 0u;

    N_AddressManager_Status_t addressResult = N_AddressManager_AllocateAddresses(
        myTotalGroupCount, &s_thisNetworkAddress, &myFirstGroupId);

    if ( (addressResult != N_AddressManager_Status_Success) &&
         (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
    {
        AbortJoinDevice();
        return;
    }
    N_UTIL_CALLBACK(N_LinkInitiator_Callback_t, s_subscribers, GroupAddressesAssigned,
        (myFirstGroupId, (myFirstGroupId + myTotalGroupCount) - 1u));

    s_thisNetworkUpdateId = 0u;
    uint32_t channelMask = N_DeviceInfo_GetPrimaryChannelMask();
    uint8_t channelIndex = (uint8_t)(N_Util_Random() % N_DeviceInfo_GetNrChannelsInChannelMask(channelMask));
    s_thisChannel = N_DeviceInfo_GetChannelForIndex(channelIndex, channelMask);

    N_Security_Key_t thisNetworkKey;
    N_Security_GenerateNewNetworkKey(thisNetworkKey);
    N_Connection_JoinNetwork(s_thisExtendedPanId,
                             s_thisChannel,
                             s_thisPanId,
                             s_thisNetworkAddress,
                             s_thisNetworkUpdateId,
                             thisNetworkKey,    // this call will thrash the thisNetworkKey variable when used
                             JoinNetworkDone);
}

static inline void SendNetworkJoinRequest(void)
{
    N_Security_Key_t thisNetworkKey;
    uint8_t *resultKey = getSecurityKeyBuffer();
    N_Cmi_GetNetworkKey(thisNetworkKey);    // key in the clear - keep as short as possible!
    N_Security_EncryptNetworkKey(N_Security_FindSharedKeyIndex(s_deviceArray->scanResponse.keyBitMask), s_deviceArray->scanResponse.transactionId, s_deviceArray->scanResponse.responseId,
                                 thisNetworkKey, resultKey, sendNetworkJoinRequestKeyEncrypted);
}

static void sendNetworkJoinRequestKeyEncrypted(void)
{
  uint8_t *key = extractKeyFromBuffer();

  memcpy(s_thisEncryptedNetworkKey, key, sizeof(N_Security_Key_t));

  continueSendNetworkJoinRequest();
}

static void continueSendNetworkJoinRequest(void)
{
  N_InterPan_NetworkJoinRequest_t networkJoinRequest;
  memset(&networkJoinRequest,0,sizeof(N_InterPan_NetworkJoinRequest_t));

  memcpy(s_thisExtendedPanId, N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(s_thisExtendedPanId));
  s_thisNetworkUpdateId = N_DeviceInfo_GetNetworkUpdateId();
  s_thisChannel = N_DeviceInfo_GetNetworkChannel();
  s_thisPanId = N_DeviceInfo_GetNetworkPanId();
  s_thisNetworkAddress = N_DeviceInfo_GetNetworkAddress();
  s_thisKeyIndex = N_Security_FindSharedKeyIndex(s_deviceArray->scanResponse.keyBitMask);

  // allocate network address and group ids for the target
  if ( (s_deviceArray->scanResponse.zllInfo & N_INTERPAN_ZLL_INFO_ADDRESS_ASSIGNMENT) != 0u )
  {
    uint16_t networkAddress = networkJoinRequest.networkAddress;
    uint16_t groupIdsBegin  = networkJoinRequest.groupIdsBegin;
    uint16_t freeNetworkAddressRangeBegin = networkJoinRequest.freeNetworkAddressRangeBegin;
    uint16_t freeNetworkAddressRangeEnd = networkJoinRequest.freeNetworkAddressRangeEnd;
    uint16_t freeGroupIdRangeBegin = networkJoinRequest.freeGroupIdRangeBegin;
    uint16_t freeGroupIdRangeEnd = networkJoinRequest.freeGroupIdRangeEnd;
    // target is address assignment capable: get dealer addresses as well
    N_AddressManager_Status_t addressResult = N_AddressManager_AllocateAddressesForDealer(
      s_deviceArray->scanResponse.totalGroupIds,
      &networkAddress,
      &groupIdsBegin,
      &freeNetworkAddressRangeBegin,
      &freeNetworkAddressRangeEnd,
      &freeGroupIdRangeBegin,
      &freeGroupIdRangeEnd);

    networkJoinRequest.networkAddress = networkAddress;
    networkJoinRequest.groupIdsBegin = groupIdsBegin;
    networkJoinRequest.freeNetworkAddressRangeBegin = freeNetworkAddressRangeBegin;
    networkJoinRequest.freeNetworkAddressRangeEnd = freeNetworkAddressRangeEnd;
    networkJoinRequest.freeGroupIdRangeBegin = freeGroupIdRangeBegin;
    networkJoinRequest.freeGroupIdRangeEnd = freeGroupIdRangeEnd;

      if ( (addressResult != N_AddressManager_Status_Success) &&
            (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
      {
        AbortJoinDevice();
        freeSecurityKeyBuffer();
        return;
      }
  }
  else
  {
    uint16_t networkAddress = networkJoinRequest.networkAddress;
    uint16_t groupIdsBegin  = networkJoinRequest.groupIdsBegin;
    // target is not address assignment capable
    N_AddressManager_Status_t addressResult = N_AddressManager_AllocateAddresses(
      s_deviceArray->scanResponse.totalGroupIds, &networkAddress, &groupIdsBegin);

    networkJoinRequest.networkAddress = networkAddress;
    networkJoinRequest.groupIdsBegin = groupIdsBegin;

    if ( (addressResult != N_AddressManager_Status_Success) &&
          (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
    {
      AbortJoinDevice();
      freeSecurityKeyBuffer();
      return;
    }

    networkJoinRequest.freeNetworkAddressRangeBegin = 0u;
    networkJoinRequest.freeNetworkAddressRangeEnd = 0u;
    networkJoinRequest.freeGroupIdRangeBegin = 0u;
    networkJoinRequest.freeGroupIdRangeEnd = 0u;
  }
  s_targetAddress = networkJoinRequest.networkAddress;

  if ( s_deviceArray->scanResponse.totalGroupIds == 0u )
  {
    // empty target group id range
    networkJoinRequest.groupIdsBegin = 0u;
    networkJoinRequest.groupIdsEnd = 0u;
  }
  else
  {
    // calculate the end of the group id range
    networkJoinRequest.groupIdsEnd = networkJoinRequest.groupIdsBegin +
        ((uint16_t) s_deviceArray->scanResponse.totalGroupIds - 1u);
  }

  networkJoinRequest.transactionId = s_deviceArray->scanResponse.transactionId;
  memcpy(networkJoinRequest.extendedPanId, s_thisExtendedPanId, sizeof(networkJoinRequest.extendedPanId));
  networkJoinRequest.keyIndex = s_thisKeyIndex;
  memcpy(networkJoinRequest.encryptedNetworkKey, s_thisEncryptedNetworkKey, sizeof(networkJoinRequest.encryptedNetworkKey));
  networkJoinRequest.networkUpdateId = s_thisNetworkUpdateId;
  networkJoinRequest.channel = s_thisChannel;
  networkJoinRequest.panId = s_thisPanId;

  freeSecurityKeyBuffer();

  // send message
  if ((s_deviceArray->scanResponse.zigBeeInfo & N_INTERPAN_ZIGBEE_INFO_LOGICAL_TYPE) == N_INTERPAN_ZIGBEE_INFO_END_DEVICE)
  {
    N_InterPan_SendNetworkJoinEndDeviceRequest(&networkJoinRequest, &s_deviceArray->ieeeAddress);
  }
  else
  {
    N_InterPan_SendNetworkJoinRouterRequest(&networkJoinRequest, &s_deviceArray->ieeeAddress);
  }

  // start timeout;
  N_Timer_Start16(RESPONSE_TIMEOUT_MS, &s_timer);
}

// called only when we are a factory new end device
static inline void SendNetworkStartRequest(void)
{
  N_Security_Key_t thisNetworkKey;
  uint8_t *resultKey = getSecurityKeyBuffer();
  N_Security_GenerateNewNetworkKey(thisNetworkKey);
  s_thisKeyIndex = N_Security_FindSharedKeyIndex(s_deviceArray->scanResponse.keyBitMask);
  N_Security_EncryptNetworkKey(s_thisKeyIndex, s_deviceArray->scanResponse.transactionId, s_deviceArray->scanResponse.responseId,
                               thisNetworkKey, resultKey, sendNetworkStartRequestKeyEncrypted);
}

static void sendNetworkStartRequestKeyEncrypted(void)
{
  uint8_t *key = extractKeyFromBuffer();

  memcpy(s_thisEncryptedNetworkKey, key, sizeof(N_Security_Key_t));

  continueSendNetworkStartRequest();
}

static void continueSendNetworkStartRequest(void)
{
  N_InterPan_NetworkStartRequest_t networkStartRequest;
  memset(&networkStartRequest,0,sizeof(N_InterPan_NetworkStartRequest_t)); 

  uint16_t myFirstGroupId = 0u;
  uint8_t myTotalGroupCount = N_PacketDistributor_GetTotalGroupCount();

  N_AddressManager_Status_t addressResult = N_AddressManager_AllocateAddresses(
    myTotalGroupCount, &s_thisNetworkAddress, &myFirstGroupId);

  if ( (addressResult != N_AddressManager_Status_Success) &&
        (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
  {
    AbortJoinDevice();
    freeSecurityKeyBuffer();
    return;
  }

  for ( size_t i = 0u; i < sizeof(s_thisExtendedPanId); i++ )
  {
    s_thisExtendedPanId[i] = 0u;
  }

  s_thisNetworkUpdateId = 0u;

  uint32_t channelMask = N_DeviceInfo_GetPrimaryChannelMask();
  uint16_t channelIndex = N_Util_Random() % (uint16_t)N_DeviceInfo_GetNrChannelsInChannelMask(channelMask);
  s_thisChannel = N_DeviceInfo_GetChannelForIndex((uint8_t)channelIndex, channelMask);

  s_thisPanId = 0u;

    // allocate network address and group ids for the target
  if ( (s_deviceArray->scanResponse.zllInfo & N_INTERPAN_ZLL_INFO_ADDRESS_ASSIGNMENT) != 0u )
  {
    uint16_t networkAddress = networkStartRequest.networkAddress;
    uint16_t groupIdsBegin  = networkStartRequest.groupIdsBegin;
    uint16_t freeNetworkAddressRangeBegin = networkStartRequest.freeNetworkAddressRangeBegin;
    uint16_t freeNetworkAddressRangeEnd = networkStartRequest.freeNetworkAddressRangeEnd;
    uint16_t freeGroupIdRangeBegin = networkStartRequest.freeGroupIdRangeBegin;
    uint16_t freeGroupIdRangeEnd = networkStartRequest.freeGroupIdRangeEnd;

    // target is address assignment capable: get dealer addresses as well
    addressResult = N_AddressManager_AllocateAddressesForDealer(
      s_deviceArray->scanResponse.totalGroupIds,
      &networkAddress,
      &groupIdsBegin,
      &freeNetworkAddressRangeBegin,
      &freeNetworkAddressRangeEnd,
      &freeGroupIdRangeBegin,
      &freeGroupIdRangeEnd);

    networkStartRequest.networkAddress = networkAddress;
    networkStartRequest.groupIdsBegin = groupIdsBegin;
    networkStartRequest.freeNetworkAddressRangeBegin = freeNetworkAddressRangeBegin;
    networkStartRequest.freeNetworkAddressRangeEnd = freeNetworkAddressRangeEnd;
    networkStartRequest.freeGroupIdRangeBegin = freeGroupIdRangeBegin;
    networkStartRequest.freeGroupIdRangeEnd = freeGroupIdRangeEnd;

    if ( (addressResult != N_AddressManager_Status_Success) &&
          (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
      {
        AbortJoinDevice();
        freeSecurityKeyBuffer();
        return;
      }
  }
  else
  {
    uint16_t networkAddress = networkStartRequest.networkAddress;
    uint16_t groupIdsBegin  = networkStartRequest.groupIdsBegin;
    // target is not address assignment capable
    addressResult = N_AddressManager_AllocateAddresses(
      s_deviceArray->scanResponse.totalGroupIds,
      &networkAddress,
      &groupIdsBegin);

    networkStartRequest.networkAddress = networkAddress;
    networkStartRequest.groupIdsBegin = groupIdsBegin;

    if ( (addressResult != N_AddressManager_Status_Success) &&
          (addressResult != N_AddressManager_Status_RandomValue) ) // Stochastic addressing
    {
      AbortJoinDevice();
      freeSecurityKeyBuffer();
      return;
    }

    networkStartRequest.freeNetworkAddressRangeBegin = 0u;
    networkStartRequest.freeNetworkAddressRangeEnd = 0u;
    networkStartRequest.freeGroupIdRangeBegin = 0u;
    networkStartRequest.freeGroupIdRangeEnd = 0u;
  }
  s_targetAddress = networkStartRequest.networkAddress;

  if ( s_deviceArray->scanResponse.totalGroupIds == 0u )
  {
    // empty target group id range
    networkStartRequest.groupIdsBegin = 0u;
    networkStartRequest.groupIdsEnd = 0u;
  }
  else
  {
    // calculate the end of the group id range
    networkStartRequest.groupIdsEnd = networkStartRequest.groupIdsBegin +
      ((uint16_t) s_deviceArray->scanResponse.totalGroupIds - 1u);
  }

    networkStartRequest.transactionId = s_deviceArray->scanResponse.transactionId;
    memset(networkStartRequest.extendedPanId, 0, sizeof(networkStartRequest.extendedPanId));

  networkStartRequest.keyIndex = s_thisKeyIndex;
  memcpy(networkStartRequest.encryptedNetworkKey, s_thisEncryptedNetworkKey, sizeof(networkStartRequest.encryptedNetworkKey));
  networkStartRequest.channel = 0u;
  networkStartRequest.panId = 0u;
  memcpy(networkStartRequest.endDeviceIeeeAddress, N_DeviceInfo_GetIEEEAddress(), sizeof(networkStartRequest.endDeviceIeeeAddress));
  networkStartRequest.endDeviceNetworkAddress = s_thisNetworkAddress;

  freeSecurityKeyBuffer();

  // send message
  N_InterPan_SendNetworkStartRequest(&networkStartRequest, &s_deviceArray->ieeeAddress);

  N_UTIL_CALLBACK(N_LinkInitiator_Callback_t, s_subscribers, GroupAddressesAssigned, (myFirstGroupId, (myFirstGroupId + myTotalGroupCount) - 1u));

  // start timeout
  N_Timer_Start16(RESPONSE_TIMEOUT_MS, &s_timer);
}

static inline void JoinNetwork(void)
{
    uint8_t *resultKey = getSecurityKeyBuffer();
    N_Security_DecryptNetworkKey(s_thisKeyIndex,
                                 s_deviceArray->scanResponse.transactionId,
                                 s_deviceArray->scanResponse.responseId,
                                 s_thisEncryptedNetworkKey,
                                 resultKey,
                                 continueJoinNetwork);
}

static void continueJoinNetwork(void)
{
  uint8_t *networkKey = extractKeyFromBuffer();
  N_Connection_JoinNetwork(s_thisExtendedPanId,
                           s_thisChannel,
                           s_thisPanId,
                           s_thisNetworkAddress,
                           s_thisNetworkUpdateId,
                           networkKey,
                           JoinNetworkDone);
  freeSecurityKeyBuffer();
}

static inline void JoinDeviceSuccess(void)
{
    N_ERRH_ASSERT_FATAL(s_joinDeviceDoneCallback != NULL);
    s_joinDeviceDoneCallback(N_LinkInitiator_Status_Ok, s_targetAddress);
    s_joinDeviceDoneCallback = NULL;
}

static inline void StopResponseTimeoutTimer(void)
{
    N_Timer_Stop(&s_timer);
}

static inline void ResetResponseTimeoutTimer(void)
{
    N_Timer_Start16(TIMEOUT_UNICAST_RESPONSE_MS, &s_timer);
}

static inline void JoinDeviceFail(void)
{
    N_ERRH_ASSERT_FATAL(s_joinDeviceDoneCallback != NULL);
    s_joinDeviceDoneCallback(N_LinkInitiator_Status_Failure, N_ADDRESS_INVALID_SHORT_ADDRESS);
    s_joinDeviceDoneCallback = NULL;
}

static inline void JoinDeviceCommunicationNotEstablished(void)
{
    N_ERRH_ASSERT_FATAL(s_joinDeviceDoneCallback != NULL);
    s_joinDeviceDoneCallback(N_LinkInitiator_Status_CommunicationNotEstablished, s_targetAddress);
    s_joinDeviceDoneCallback = NULL;
}

static inline void UseStartParams(void)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkStartResponse);
    memcpy(s_thisExtendedPanId, s_receivedInterPanMessage.networkStartResponse.networkStartResponse.extendedPanId, sizeof(s_thisExtendedPanId));
    s_thisChannel = s_receivedInterPanMessage.networkStartResponse.networkStartResponse.channel;
    s_thisPanId = s_receivedInterPanMessage.networkStartResponse.networkStartResponse.panId;
    s_thisNetworkUpdateId = s_receivedInterPanMessage.networkStartResponse.networkStartResponse.networkUpdateId;

    NwkNeighbor_t* neighbor = NWK_AddKnownNeighbor(s_targetAddress, (ExtAddr_t*)s_receivedInterPanMessage.networkStartResponse.sourceAddress, true);
    if (neighbor)
    {
      neighbor->logicalChannel = s_thisChannel;
      neighbor->panId = s_thisPanId;
      memcpy(&neighbor->extPanId, s_thisExtendedPanId, sizeof(s_thisExtendedPanId));
    }
}

static inline void UseStartParams_SetInterPanModeOff(void)
{
    UseStartParams();
    SetInterPanModeOff();
}

static inline void SendDummyUnicastMessage(void)
{
    N_Address_t destAddr = c_Addr16;
    destAddr.address.shortAddress = s_targetAddress;
    // destAddr.endPoint is already set implicitly to '0' (ZDO endpoint)

    s_unicastLastSentSeqNr = N_Zdp_SendIeeeAddrReq(s_targetAddress, &destAddr);
    N_Timer_Start16(TIMEOUT_UNICAST_RESPONSE_MS, &s_timer);
}

static inline void ReconnectUrgent(void)
{
    N_Connection_ReconnectUrgent();
}

static inline void InitRetryCounter(void)
{
    s_unicastRetryCounter = NUM_UNICAST_RETRIES;
}

static inline void SendNetworkUpdateRequest(void)
{
    N_InterPan_NetworkUpdateRequest_t payload;
    payload.transactionId = s_deviceArray->scanResponse.transactionId;
    memcpy(payload.extendedPanId, s_deviceArray->scanResponse.extendedPanId, sizeof(payload.extendedPanId));

    if(IsOtherUpdateIdHigher())
    {
        payload.networkUpdateId = s_deviceArray->scanResponse.networkUpdateId;
        payload.channel = s_deviceArray->scanResponse.channel;
    }
    else
    {
        payload.networkUpdateId = N_DeviceInfo_GetNetworkUpdateId();
        payload.channel = N_DeviceInfo_GetNetworkChannel();
    }

    payload.panId = N_DeviceInfo_GetNetworkPanId();
    payload.networkAddress = s_deviceArray->scanResponse.networkAddress;
    N_InterPan_SendNetworkUpdateRequest(&payload, &s_deviceArray->ieeeAddress);

    s_targetAddress = s_deviceArray->scanResponse.networkAddress;

    N_Timer_Start16(CHANGE_TIMEOUT_MS, &s_timer);
}

static inline void UpdateNetwork(void)
{
    N_Connection_UpdateNetwork(s_deviceArray->scanResponse.channel, s_deviceArray->scanResponse.networkUpdateId, UpdateNetworkDone);
}

static inline void SendDeviceInfoRequest(void)
{
    N_InterPan_DeviceInfoRequest_t deviceInfoRequest;
    deviceInfoRequest.transactionId = s_deviceArray->scanResponse.transactionId;
    deviceInfoRequest.startIndex = s_deviceInfoStartIndex;
    N_InterPan_SendDeviceInfoRequest(&deviceInfoRequest, &s_deviceArray->ieeeAddress);

    N_Timer_Start16(RESPONSE_TIMEOUT_MS, &s_timer);
}

static inline void StoreDeviceInfo(void)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_DeviceInfoResponse);
    for (uint8_t i = 0u ; i < s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->count ; i++)
    {
        s_endpointInfoArray[s_deviceInfoStartIndex + i] = s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->deviceInfo[i];
    }
    s_deviceInfoStartIndex = s_deviceInfoStartIndex + s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->count;
}

static inline void DeviceInfoSuccess(void)
{
    N_ERRH_ASSERT_FATAL(s_deviceInfoRequestDoneCallback != NULL);
    s_deviceInfoRequestDoneCallback(N_LinkInitiator_Status_Ok);
    s_deviceInfoRequestDoneCallback = NULL;
}

static inline void DeviceInfoFail(void)
{
    N_ERRH_ASSERT_FATAL(s_deviceInfoRequestDoneCallback != NULL);
    s_deviceInfoRequestDoneCallback(N_LinkInitiator_Status_Failure);
    s_deviceInfoRequestDoneCallback = NULL;
}

static inline void SendResetToFactoryNewRequest(void)
{
    N_InterPan_ResetToFactoryNewRequest_t resetToFactoryNewRequest;
    resetToFactoryNewRequest.transactionId = s_deviceArray->scanResponse.transactionId;
    N_InterPan_SendResetToFactoryNewRequest(&resetToFactoryNewRequest, &s_deviceArray->ieeeAddress);

    N_Timer_Start16(CHANGE_TIMEOUT_MS, &s_timer);
}

static inline void ResetToFactoryNewDone(void)
{
    N_ERRH_ASSERT_FATAL(s_resetToFactoryNewDoneCallback != NULL);
    s_resetToFactoryNewDoneCallback(N_LinkInitiator_Status_Ok);
    s_resetToFactoryNewDoneCallback = NULL;
}

static inline void RestoreTransmitPower(N_Radio_SetTxPowerDone_t restoreTransmitPowerDone)
{
    N_Radio_SetTxPower(s_currentTransmitPower, restoreTransmitPowerDone);
}

static inline void StoreCurrentTransmitPower(void)
{
    s_currentTransmitPower = N_Radio_GetTxPower();
}

static inline void FindFreePan(void)
{
    N_Connection_FindFreePan(FindFreePanDone);
}

static inline void StartStartupDelayTimer(void)
{
    N_Timer_Start16(WAIT_MIN_STARTUP_WAIT_TIME_MS, &s_timer);
}

static void PerformAction(N_FSM_Action_t action, int32_t arg1, int32_t arg2)
{
    switch(action)
    {
    case aSetInterPanModeOnFirstChannel_StoreCurrentTransmitPower:
        SetInterPanModeOnFirstChannel();
        StoreCurrentTransmitPower();
        break;
    case aSetInterPanModeOnTargetChannel:
        SetInterPanModeOnTargetChannel();
        break;
    case aSetInterPanModeOnNextChannel:
        SetInterPanModeOnNextChannel();
        break;
    case aSetInterPanModeOff:
        SetInterPanModeOff();
        break;
    case aSetInterPanModeOff_StopTimer:
        SetInterPanModeOff();
        StopResponseTimeoutTimer();
        break;
    case aSetInterPanModeOff_RestoreTransmitPower:
        RestoreTransmitPower(SetInterPanModeOff);
        break;
    case aSetInterPanModeOff_RestoreTransmitPower_StopTimer:
        StopResponseTimeoutTimer();
        RestoreTransmitPower(SetInterPanModeOff);
        break;
    case aBroadcastScanRequest:
        BroadcastScanRequest();
        break;
    case aStoreScanResponse:
        StoreScanResponse();
        break;
    case aScanFinishedSuccess:
        ScanFinishedSuccess();
        break;
    case aScanFinishedStopped:
        ScanFinishedStopped();
        break;
    case aSendIdentifyRequest:
        SendIdentifyRequest();
        break;
    case aIdentifySent:
        IdentifySent();
        break;
    case aSendNetworkJoinRequest:
        SendNetworkJoinRequest();
        break;
    case aSendNetworkStartRequest:
        SendNetworkStartRequest();
        break;
    case aJoinNetwork:
        JoinNetwork();
        break;
    case aStartNetwork:
        StartNetwork();
        break;
    case aJoinDeviceSuccess_StopTimer:
        JoinDeviceSuccess();
        StopResponseTimeoutTimer();
        break;
    case aResetResponseTimeoutTimer:
        ResetResponseTimeoutTimer();
        break;
    case aJoinDeviceFail:
        JoinDeviceFail();
        break;
    case aJoinDeviceCommunicationNotEstablished:
        JoinDeviceCommunicationNotEstablished();
        break;
    case aUseStartParams_SetInterPanModeOff_StopTimer:
        UseStartParams_SetInterPanModeOff();
        StopResponseTimeoutTimer();
        break;
    case aSendDummyUnicastMessage:
        SendDummyUnicastMessage();
        break;
    case aReconnectUrgent_SendDummyUnicastMessage_InitRetryCounter:
        ReconnectUrgent();
        SendDummyUnicastMessage();
        InitRetryCounter();
        break;
    case aReconnectUrgent_JoinDeviceCommunicationNotEstablished:
        ReconnectUrgent();
        JoinDeviceCommunicationNotEstablished();
        break;
    case aSendNetworkUpdateRequest:
        SendNetworkUpdateRequest();
        break;
    case aUpdateNetwork:
        UpdateNetwork();
        break;
    case aSendDeviceInfoRequest:
        SendDeviceInfoRequest();
        break;
    case aStoreDeviceInfo_SendDeviceInfoRequest:
        StoreDeviceInfo();
        SendDeviceInfoRequest();
        break;
    case aStoreDeviceInfo_SetInterPanModeOff_StopTimer:
        StoreDeviceInfo();
        SetInterPanModeOff();
        StopResponseTimeoutTimer();
        break;
    case aDeviceInfoSuccess:
        DeviceInfoSuccess();
        break;
    case aDeviceInfoFail:
        DeviceInfoFail();
        break;
    case aSendResetToFactoryNewRequest:
        SendResetToFactoryNewRequest();
        break;
    case aResetToFactoryNewDone:
        ResetToFactoryNewDone();
        break;
    case aFindFreePan:
        FindFreePan();
        break;
    case aStartStartupDelayTimer:
        StartStartupDelayTimer();
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

static inline bool IsScanDone(int32_t arg1, int32_t arg2)
{
    uint8_t totalScanRequestCount = N_DeviceInfo_GetNrChannelsInChannelMask(N_DeviceInfo_GetPrimaryChannelMask()) +
        (NUMBER_OF_SCAN_REQUESTS_ON_FIRST_CHANNEL - 1u);

    uint32_t mask = N_DeviceInfo_GetSecondaryChannelMask();
    if ( ((s_scanType & N_LinkInitiator_ScanType_IncludeSecondaryChannels) != 0u) && (mask != 0uL))
    {
        totalScanRequestCount += N_DeviceInfo_GetNrChannelsInChannelMask(mask);
    }
    (void)arg1;
    (void)arg2;
    return N_UTIL_BOOL(s_scanRequestCount == totalScanRequestCount);
}

static inline bool IsScanResponseValid(int32_t arg1, int32_t arg2)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_ScanResponse);

    if (s_receivedInterPanMessage.scanResponse.pScanResponse->transactionId != s_scanTransactionId )
    {
        return FALSE;
    }

    int16_t correctedRssi = (int16_t) s_receivedInterPanMessage.scanResponse.rssi +
        (int16_t) s_receivedInterPanMessage.scanResponse.pScanResponse->touchlinkRssiCorrection;
    if ( correctedRssi < (int16_t) N_DeviceInfo_GetTouchlinkRssiThreshold())
    {
        return FALSE;
    }

    if ((s_scanType & N_LinkInitiator_ScanType_OwnPanOnly) != 0u)
    {
        if (s_receivedInterPanMessage.scanResponse.pScanResponse->panId != N_DeviceInfo_GetNetworkPanId())
        {
            return FALSE;
        }
    }

    if (!N_Security_IsCompatibleWithKeyMask(s_receivedInterPanMessage.scanResponse.pScanResponse->keyBitMask))
    {
        return FALSE;
    }
    (void)arg1;
    (void)arg2;
    return TRUE;
}

static inline bool IsJoinResponseInvalid(int32_t arg1, int32_t arg2)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkJoinResponse);

    // check source address
    if ( memcmp(s_receivedInterPanMessage.networkJoinResponse.sourceAddress, s_deviceArray->ieeeAddress, sizeof(N_Address_Extended_t)) != 0 )
    {
        return TRUE;
    }
    // check transaction id
    if ( s_receivedInterPanMessage.networkJoinResponse.networkJoinResponse.transactionId != s_deviceArray->scanResponse.transactionId )
    {
        return TRUE;
    }
    (void)arg1;
    (void)arg2;
    return FALSE;
}

static inline bool IsJoinResponseError(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkJoinResponse);
    return N_UTIL_BOOL(s_receivedInterPanMessage.networkJoinResponse.networkJoinResponse.status != 0u);
}

static inline bool IsStartResponseInvalid(int32_t arg1, int32_t arg2)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkStartResponse);
    // check source address
    if ( memcmp(s_receivedInterPanMessage.networkStartResponse.sourceAddress, s_deviceArray->ieeeAddress, sizeof(N_Address_Extended_t)) != 0 )
    {
        return TRUE;
    }
    // check transaction id
    if ( s_receivedInterPanMessage.networkStartResponse.networkStartResponse.transactionId != s_deviceArray->scanResponse.transactionId )
    {
        return TRUE;
    }

    if ( memcmp(s_receivedInterPanMessage.networkStartResponse.networkStartResponse.extendedPanId, c_ExtPanId00, sizeof(s_receivedInterPanMessage.networkStartResponse.networkStartResponse.extendedPanId)) == 0)
    {
        return TRUE;
    }

    if (!N_DeviceInfo_IsChannelInMask(s_receivedInterPanMessage.networkStartResponse.networkStartResponse.channel, N_DeviceInfo_GetPrimaryChannelMask()))
    {
        return TRUE;
    }

    if ((s_receivedInterPanMessage.networkStartResponse.networkStartResponse.panId == 0uL) ||
        (s_receivedInterPanMessage.networkStartResponse.networkStartResponse.panId == 0xFFFFuL))
    {
        return TRUE;
    }
    (void)arg1;
    (void)arg2;
    return FALSE;
}

static inline bool IsStartResponseError(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkStartResponse);
    return N_UTIL_BOOL(s_receivedInterPanMessage.networkStartResponse.networkStartResponse.status != 0u);
}

static inline bool IsThisFactoryNewEndDevice(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    bool isEndDevice = N_DeviceInfo_IsEndDevice();
    return N_UTIL_BOOL((N_DeviceInfo_IsFactoryNew()) && isEndDevice);
}

static inline bool IsThisFactoryNewRouter(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    bool isRouter = N_UTIL_BOOL(!N_DeviceInfo_IsEndDevice());
    return N_UTIL_BOOL((N_DeviceInfo_IsFactoryNew()) && isRouter);
}

static inline bool AnotherRetry(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    if (s_unicastRetryCounter > 0u)
    {
        s_unicastRetryCounter--;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static inline bool IsSamePan(int32_t arg1, int32_t arg2)
{
    (void)arg1;
    (void)arg2;
    return N_UTIL_BOOL(memcmp(s_deviceArray->scanResponse.extendedPanId, N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(N_Address_ExtendedPanId_t)) == 0);
}

static inline bool IsOtherUpdateIdHigher(void)
{
    uint8_t myNetworkUpdateId = N_DeviceInfo_GetNetworkUpdateId();
    return N_UTIL_BOOL(N_Util_HighestNetworkUpdateId(s_deviceArray->scanResponse.networkUpdateId, myNetworkUpdateId) != myNetworkUpdateId);
}

static inline bool IsJoinRequestValid(int32_t arg1, int32_t arg2)
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_NetworkJoinRequest);
    (void)arg1;
    (void)arg2;
    // check transaction id
    if ( s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.transactionId != s_deviceArray->scanResponse.transactionId )
    {
        return FALSE;
    }

    if (!N_DeviceInfo_IsChannelInMask(s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.channel, N_DeviceInfo_GetPrimaryChannelMask()))
    {
        return FALSE;
    }
    if ((s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.panId == 0x0000u) ||
        (s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.panId == 0xFFFFu))
    {
        return FALSE;
    }
    if ((s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.networkAddress == 0x0000u) ||
        (s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.networkAddress > 0xFFF7u))
    {
        return FALSE;
    }
    if (memcmp(s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.extendedPanId, c_ExtPanId00, sizeof(N_Address_ExtendedPanId_t)) == 0)
    {
        return FALSE;
    }
    if (memcmp(s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest.extendedPanId, c_ExtPanIdFF, sizeof(N_Address_ExtendedPanId_t)) == 0)
    {
        return FALSE;
    }
    return TRUE;
}

static inline bool IsDeviceInfoInvalid()
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_DeviceInfoResponse);

    if ( s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->count == 0u )
    {
        return TRUE;
    }
    if ( s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->numberSubDevices != s_deviceArray->scanResponse.numberSubDevices )
    {
        return TRUE;
    }
    if ( s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->startIndex != s_deviceInfoStartIndex )
    {
        return TRUE;
    }
    if ( (s_deviceInfoStartIndex + s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->count) > s_deviceArray->scanResponse.numberSubDevices )
    {
        return TRUE;
    }
    return FALSE;
}

static inline bool IsDeviceInfoComplete()
{
    N_ERRH_ASSERT_FATAL(s_receivedMessageType == ReceivedMessageType_DeviceInfoResponse);

    return N_UTIL_BOOL((s_deviceInfoStartIndex + s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse->count) ==
        s_deviceArray->scanResponse.numberSubDevices);
}

static inline bool IsResponseToLastMessage()
{
    return N_UTIL_BOOL(s_unicastLastSentSeqNr == s_unicastLastReceivedSeqNr);
}

static inline bool SkipCommunicationCheck()
{
    return s_skipCommunicationCheck;
}

static bool CheckCondition(N_FSM_Condition_t condition, int32_t arg1, int32_t arg2)
{
    switch (condition)
    {
    case cIsScanDone:
        return IsScanDone(arg1, arg2);
    case cIsScanResponseValid:
        return IsScanResponseValid(arg1, arg2);
    case cIsJoinResponseInvalid:
        return IsJoinResponseInvalid(arg1, arg2);
    case cIsJoinResponseError:
        return IsJoinResponseError(arg1, arg2);
    case cIsStartResponseInvalid:
        return IsStartResponseInvalid(arg1, arg2);
    case cIsStartResponseError:
        return IsStartResponseError(arg1, arg2);
    case cIsThisFactoryNewEndDevice:
        return IsThisFactoryNewEndDevice(arg1, arg2);
    case cIsThisFactoryNewRouter:
        return IsThisFactoryNewRouter(arg1, arg2);
    case cAnotherRetry:
        return AnotherRetry(arg1, arg2);
    case cIsSamePan:
        return IsSamePan(arg1, arg2);
    case cIsOtherUpdateIdHigher:
        return IsOtherUpdateIdHigher();
    case cIsJoinRequestValid:
        return IsJoinRequestValid(arg1, arg2);
    case cIsDeviceInfoInvalid:
        return IsDeviceInfoInvalid();
    case cIsDeviceInfoComplete:
        return IsDeviceInfoComplete();
    case cIsResponseToLastMessage:
        return IsResponseToLastMessage();
    case cSkipCommunicationCheck:
        return SkipCommunicationCheck();
    default:
        return TRUE;
    }
}

/***************************************************************************************************
* CALLBACK FUNCTIONS
***************************************************************************************************/

/** Callback function, see \ref N_Connection_Callback_t::JoinNetworkDone. */
static void JoinNetworkDone(N_Connection_Result_t result)
{
    if (result == N_Connection_Result_Success)
    {
        N_Task_SetEvent(s_taskId, EVENT_JOIN_NETWORK_SUCCESS);
    }
    else
    {
        N_Task_SetEvent(s_taskId, EVENT_JOIN_NETWORK_FAILURE);
    }
}

/** Callback function, see \ref N_Connection_Callback_t::UpdateNetworkDone. */
static void UpdateNetworkDone(void)
{
    N_Task_SetEvent(s_taskId, EVENT_UPDATE_NETWORK_DONE);
}

/** Callback function, see \ref N_Connection_FindFreePanDone_t. */
static void FindFreePanDone(uint16_t panId, N_Address_ExtendedPanId_t extendedPanId)
{
#if defined(DEVCFG_FIXED_PANID)
    #warning USING FIXED PANID!
    panId = DEVCFG_FIXED_PANID;
#endif

#if defined(DEVCFG_FIXED_EXT_PANID)
    #warning USING FIXED EXTENDED PANID!
    extendedPanId = (N_Address_ExtendedPanId_t)DEVCFG_FIXED_EXT_PANID;
#endif

    s_thisPanId = panId;
    memcpy(s_thisExtendedPanId, extendedPanId, sizeof(s_thisExtendedPanId));

    FsmHandleEvent((uint8_t)eFoundFreePan);
}

/** Callback function, see \ref N_InterPan::ReceivedScanResponse. */
static void ReceivedScanResponse(N_InterPan_ScanResponse_t* pPayload, int8_t rssi, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_ScanResponse;
    s_receivedInterPanMessage.scanResponse.rssi = rssi;
    s_receivedInterPanMessage.scanResponse.pScanResponse = pPayload;
    memcpy(&s_receivedInterPanMessage.scanResponse.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));

    // Note: clipping pPayload->touchlinkRssiCorrection to at most 0x20u must be done in
    // N_LinkInitiator instead of in N_InterPan, to be able to remain compatible with
    // Louvre/Actilume factory tests. See #111.
    if (pPayload->touchlinkRssiCorrection > 0x20u)
    {
        pPayload->touchlinkRssiCorrection = 0x20u;
    }
    FsmHandleEvent((uint8_t)eReceivedScanResponse);
}

/** Callback function, see \ref N_InterPan::ReceivedDeviceInfoResponse. */
static void ReceivedDeviceInfoResponse(N_InterPan_DeviceInfoResponse_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_DeviceInfoResponse;
    s_receivedInterPanMessage.deviceInfoResponse.pDeviceInfoResponse = pPayload;
    memcpy(&s_receivedInterPanMessage.deviceInfoResponse.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent((uint8_t)eReceivedDeviceInfoResponse);
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkJoinRouterResponse.
    Used for both ReceivedNetworkJoinRouterResponse and ReceivedNetworkJoinEndDeviceResponse. */
static void ReceivedNetworkJoinResponse(N_InterPan_NetworkJoinResponse_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_NetworkJoinResponse;
    s_receivedInterPanMessage.networkJoinResponse.networkJoinResponse = *pPayload;
    memcpy(&s_receivedInterPanMessage.networkJoinResponse.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent((uint8_t)eReceivedNetworkJoinResponse);
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkStartResponse. */
static void ReceivedNetworkStartResponse(N_InterPan_NetworkStartResponse_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_NetworkStartResponse;
    s_receivedInterPanMessage.networkStartResponse.networkStartResponse = *pPayload;
    memcpy(&s_receivedInterPanMessage.networkStartResponse.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent((uint8_t)eReceivedNetworkStartResponse);
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkJoinRouterRequest. */
static void ReceivedNetworkJoinRouterRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_NetworkJoinRequest;
    s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest = *pPayload;
    memcpy(&s_receivedInterPanMessage.networkJoinRequest.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent((uint8_t)eReceivedNetworkJoinRouterRequest);
}

/** Callback function, see \ref N_InterPan_Callback_t::ReceivedNetworkJoinEndDeviceRequest. */
static void ReceivedNetworkJoinEndDeviceRequest(N_InterPan_NetworkJoinRequest_t* pPayload, N_Address_Extended_t* pSourceAddress)
{
    s_receivedMessageType = ReceivedMessageType_NetworkJoinRequest;
    s_receivedInterPanMessage.networkJoinRequest.networkJoinRequest = *pPayload;
    memcpy(&s_receivedInterPanMessage.networkJoinRequest.sourceAddress, pSourceAddress, sizeof(N_Address_Extended_t));
    FsmHandleEvent((uint8_t)eReceivedNetworkJoinEndDeviceRequest);
}

/** Callback function, see \ref N_Zdp_Callback_t::IeeeAddrRsp. */
static void IeeeAddrRsp(N_Zdp_AddrRsp_t* pRsp)
{
    if (pRsp->status == N_ZDP_STATUS_SUCCESS)
    {
        s_unicastLastReceivedSeqNr = pRsp->sequenceNumber;
        FsmHandleEvent((uint8_t)eUnicastResponse);
    }
    // else ignore, a retry will be done
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

bool N_LinkInitiator_EventHandler(N_Task_Event_t evt)
{
    switch (evt)
    {
    case EVENT_TIMER:
        FsmHandleEvent((uint8_t)eTimer);
        break;
    case EVENT_JOIN_DEVICE_FAILED:
        N_ERRH_ASSERT_FATAL(s_joinDeviceDoneCallback != NULL);
        s_joinDeviceDoneCallback(N_LinkInitiator_Status_Failure, N_ADDRESS_INVALID_SHORT_ADDRESS);
        s_joinDeviceDoneCallback = NULL;
        break;
    case EVENT_UPDATE_NETWORK_DONE:
        FsmHandleEvent((uint8_t)eUpdateNetworkDone);
        break;
    case EVENT_JOIN_NETWORK_SUCCESS:
        FsmHandleEvent((uint8_t)eJoinNetworkSuccess);
        break;
    case EVENT_JOIN_NETWORK_FAILURE:
        FsmHandleEvent((uint8_t)eJoinNetworkFail);
        break;
    case EVENT_DEVICE_INFO_FAILURE:
        DeviceInfoFail();
        break;
    case EVENT_DEVICE_INFO_SUCCESS:
        DeviceInfoSuccess();
        break;
    case EVENT_SET_INTERPAN_MODE_DONE:
        FsmHandleEvent(eSetInterPanModeDone);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void N_LinkInitiator_Init(void)
{
    s_taskId = N_Task_GetIdFromEventHandler(N_LinkInitiator_EventHandler);

    static const N_InterPan_Callback_t s_N_InterPan_Callback =
    {
        NULL,   // ReceivedScanRequest
        ReceivedScanResponse,
        NULL,   // ReceivedDeviceInfoRequest
        ReceivedDeviceInfoResponse,
        NULL,   // ReceivedIdentifyRequest
        NULL,   // ReceivedNetworkStartRequest
        ReceivedNetworkStartResponse,
        ReceivedNetworkJoinRouterRequest,
        ReceivedNetworkJoinResponse,
        ReceivedNetworkJoinEndDeviceRequest,
        ReceivedNetworkJoinResponse,
        NULL,   // ReceivedNetworkUpdateRequest
        NULL,   // ReceivedResetToFactoryNewRequest
        NULL, // ReceivedInterPanCommand
        -1
    };
    N_InterPan_Subscribe(&s_N_InterPan_Callback);

    static const N_Zdp_ClientCallback_t s_N_Zdp_ClientCallback =
    {
        NULL,   // MessageReceived
        NULL,   // ReceivedNwkAddrRsp
        IeeeAddrRsp,
        NULL,   //ReceivedNodeDescRsp
        NULL,   // ReceivedSimpleDescRsp
        NULL,   // ReceivedActiveEndPointRsp
        NULL,   // ReceivedMatchDescRsp
        NULL,   // ReceivedBindRsp
        NULL,   // ReceivedUnbindRsp
        NULL,   // ReceivedMgmtLqiRsp
        NULL,   // ReceivedMgmtBindRsp
        NULL,   // ReceivedMgmtPermitJoiningRsp
        NULL,   // ReceivedMgmtRtgRsp
        NULL,   // ReceivedMgmtLeaveRsp
        -1
    };

    N_Zdp_ClientSubscribe(&s_N_Zdp_ClientCallback);

    s_timer.task = s_taskId;
    s_timer.evt = EVENT_TIMER;

    N_FSM_INITIALIZE(&s_fsm, &s_currentState, sIdle);
}

/** Interface function, see \ref N_LinkInitiator_Scan. */
void N_LinkInitiator_Scan_Impl(N_LinkInitiator_ScanType_t scanType, N_LinkInitiator_Device_t dev[], uint8_t devArraySize, N_LinkInitiator_ScanDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL(pfDoneCallback != NULL);
    s_scanTransactionId = N_Security_GetTransactionId();
    s_scanType = scanType;
    s_deviceArray = dev;
    s_deviceArraySize = devArraySize;
    s_scanResponseCount = 0u;
    s_scanDoneCallback = pfDoneCallback;
    FsmHandleEvent((uint8_t)eScan);
}

/** Interface function, see \ref N_LinkInitiator_StopScan. */
void N_LinkInitiator_StopScan_Impl(void)
{
    FsmHandleEvent((uint8_t)eStopScan);
}

/** Interface function, see \ref N_LinkInitiator_IdentifyStartRequest. */
void N_LinkInitiator_IdentifyStartRequest_Impl(N_LinkInitiator_Device_t* pDev, uint16_t timeoutInSec, N_LinkInitiator_IdentifyRequestDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL(pfDoneCallback != NULL);
    s_deviceArray = pDev; // only a single device
    s_identifyTimeoutInSec = timeoutInSec;
    s_identifyRequestDoneCallback = pfDoneCallback;
    FsmHandleEvent((uint8_t)eIdentifyRequest);
}

/** Interface function, see \ref N_LinkInitiator_IdentifyStopRequest. */
void N_LinkInitiator_IdentifyStopRequest_Impl(N_LinkInitiator_Device_t* pDev, N_LinkInitiator_IdentifyRequestDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL(pfDoneCallback != NULL);
    s_deviceArray = pDev; // only a single device
    s_identifyTimeoutInSec = 0u;
    s_identifyRequestDoneCallback = pfDoneCallback;
    FsmHandleEvent((uint8_t)eIdentifyRequest);
}

/** Interface function, see \ref N_LinkInitiator_DeviceInfoRequest. */
void N_LinkInitiator_DeviceInfoRequest_Impl(N_LinkInitiator_Device_t* pDev, N_InterPan_DeviceInfo_t epInfo[], uint8_t epInfoArraySize, N_LinkInitiator_DeviceInfoRequestDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL((pfDoneCallback != NULL) && (pDev != NULL));
    s_deviceInfoRequestDoneCallback = pfDoneCallback;
    s_deviceArray = pDev; // only a single device
    if ( s_deviceArray->scanResponse.numberSubDevices > epInfoArraySize )
    {
        // the supplied array is too small
        N_Task_SetEvent(s_taskId, EVENT_DEVICE_INFO_FAILURE);
        return;
    }
    s_endpointInfoArray = epInfo;
    s_deviceInfoStartIndex = 0u;
    if ( s_deviceArray->scanResponse.numberSubDevices == 1u )
    {
        // all requested information is present in the scan response
        memcpy(s_endpointInfoArray[0].ieeeAddress, s_deviceArray->ieeeAddress, sizeof(N_Address_Extended_t));
        s_endpointInfoArray[0].endPoint = s_deviceArray->scanResponse.endPoint;
        s_endpointInfoArray[0].profileId = s_deviceArray->scanResponse.profileId;
        s_endpointInfoArray[0].deviceId = s_deviceArray->scanResponse.deviceId;
        s_endpointInfoArray[0].version = s_deviceArray->scanResponse.version;
        s_endpointInfoArray[0].groupIds = s_deviceArray->scanResponse.groupIds;
        s_endpointInfoArray[0].sort = 0u;

        N_Task_SetEvent(s_taskId, EVENT_DEVICE_INFO_SUCCESS);
        return;
    }
    s_deviceInfoRequestDoneCallback = pfDoneCallback;
    FsmHandleEvent((uint8_t)eDeviceInfoRequest);
}

/** Interface function, see \ref N_LinkInitiator_JoinDevice. */
void N_LinkInitiator_JoinDevice_Impl(N_LinkInitiator_Device_t* pTarget, bool skipCommunicationCheck, N_LinkInitiator_JoinDeviceDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL((pfDoneCallback != NULL) && (pTarget != NULL));
    s_joinDeviceDoneCallback = pfDoneCallback;
    s_deviceArray = pTarget;
    s_skipCommunicationCheck = skipCommunicationCheck;

    bool isFactoryNew = N_DeviceInfo_IsFactoryNew();
    if ((N_DeviceInfo_IsEndDevice()) && isFactoryNew)
    {
        if ((pTarget->scanResponse.zigBeeInfo & N_INTERPAN_ZIGBEE_INFO_LOGICAL_TYPE) == N_INTERPAN_ZIGBEE_INFO_END_DEVICE)
        {
            // Factory new end device cannot link another end device
            N_Task_SetEvent(s_taskId, EVENT_JOIN_DEVICE_FAILED);
            return;
        }
    }
    if (!isFactoryNew)
    {
        if (N_DeviceInfo_GetTrustCenterMode() == N_DeviceInfo_TrustCenterMode_Central)
        {
             /// \todo Remove temporary check after split off UpdateDevice
            if (memcmp(s_deviceArray->scanResponse.extendedPanId, N_DeviceInfo_GetNetworkExtendedPanId(), sizeof(N_Address_ExtendedPanId_t)) != 0)
            {
                // [Start/Join][Router/EndDevice]Request not allowed in central trust center mode
                N_Task_SetEvent(s_taskId, EVENT_JOIN_DEVICE_FAILED);
                return;
            }
        }
    }

    FsmHandleEvent((uint8_t)eJoinDevice);
}

/** Interface function, see \ref N_LinkInitiator_ResetDeviceToFactoryNew. */
void N_LinkInitiator_ResetDeviceToFactoryNew(N_LinkInitiator_Device_t* pDev, N_LinkInitiator_ResetDeviceToFactoryNewDone_t pfDoneCallback)
{
    N_ERRH_ASSERT_FATAL(pfDoneCallback != NULL);
    s_resetToFactoryNewDoneCallback = pfDoneCallback;
    N_ERRH_ASSERT_FATAL(pDev != NULL);
    s_deviceArray = pDev;

    FsmHandleEvent((uint8_t)eResetDeviceToFactoryNew);
}

/** Interface function, see \ref N_LinkInitiator_Subscribe. */
void N_LinkInitiator_Subscribe_Impl(const N_LinkInitiator_Callback_t* pCb)
{
    N_ERRH_ASSERT_FATAL(s_taskId != N_TASK_ID_NONE);    // check if initialized
    N_UTIL_CALLBACK_SUBSCRIBE(N_LinkInitiator_Callback_t, s_subscribers, pCb);
}
