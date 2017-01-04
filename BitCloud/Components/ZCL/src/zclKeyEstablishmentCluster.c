/***************************************************************************//**
  \file zclKeyEstablishmentCluster.c

  \brief ZCL Key Establishment Cluster logic implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-01-27 arazinkov - Changed
   Last change:
    $Id: zclKeyEstablishmentCluster.c 24784 2013-03-19 08:52:51Z akhromykh $
******************************************************************************/
#if ZCL_SUPPORT == 1
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#if CERTICOM_SUPPORT == 1

/*******************************************************************************
                   Includes section
*******************************************************************************/
#include <sysTypes.h>
#include <zcl.h>
#include <clusters.h>
#include <zclKeyEstablishmentCluster.h>
#include <appTimer.h>
#include <sysUtils.h>
#include <zclSecurityManager.h>
#include <sysTimer.h>
#include <sspHash.h>
#include <zclDbg.h>
#include <dbg.h>
#include <genericEcc.h>
#include <eccAux.h>
#include <zdoNotify.h>
#include <aps.h>
#include <sysEvents.h>

/*******************************************************************************
                   Define(s) section
*******************************************************************************/
#define ZCL_KE_INVALID_EPHEMERAL_DATA_GENERATE_TIME   0xff
#define ZCL_KE_INVALID_CONFIRM_KEY_GENERATE_TIME      0xff

#ifdef _ENDDEVICE_
#define ZCL_INDIRECT_POLL_RATE_DURING_KE 2 // 2 sec
#else
#define ZCL_INDIRECT_POLL_RATE_DURING_KE 1 // 1 sec
#endif

#define ZCL_KE_INITIATE_RESPONSE_WAITING_TIME         2 * ZCL_INDIRECT_POLL_RATE_DURING_KE  // sec
#define TERMINATE_BASE_WAIT_TIME                      5 // sec

#define ZCL_UPDATE_RAND_SEQ_TIMEOUT 40 //

#define ZCL_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE sizeof(uint8_t)
#define ZCL_KE_INITATOR_ADDITIONAL_MESSAGE_COMPONENT 0x02
#define ZCL_KE_RESPONDER_ADDITIONAL_MESSAGE_COMPONENT 0x03

#define ZCL_KE_MAC_DATA_SIZE ((2 * SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE) + \
                              (2 * sizeof(ExtAddr_t)) + \
                               ZCL_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE) // 61 bytes

//C.3.1.1 Overview
//For the Initiate Key Establishment Response message, it is recommended the
//initiator wait at least 2 seconds before timing out the operation.

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  INCOMING,
  OUTCOMING
} TrxDirection_t;

typedef enum
{
  ZCL_KE_CLUSTER_INITIAL_STATE                        = 0x00, //There was init
  ZCL_KE_CLUSTER_IDLE_STATE                           = 0x01, //There was reset

  //Discovery states
  ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_SENDING_STATE     = 0x03,
  ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_TERMINATE_STATE   = 0x04,

  //KE procedure states
  ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE    = 0x06,
  ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_WAITING_STATE    = 0x07,
  ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_SENDING_STATE = 0x08,
  ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE = 0x09,

  ZCL_KE_CLUSTER_MAC1_CALCULATING_STATE               = 0x0a,
  ZCL_KE_CLUSTER_MAC2_CALCULATING_STATE               = 0x0b,

  ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_SENDING_STATE    = 0x0c,
  ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE    = 0x0d,

  ZCL_KE_CLUSTER_WAITING_STATE                        = 0x0e, // There was termination with NO_RESOURCES
} ZclKEClusterState_t;

typedef union
{
  ZCL_InitiateKeyEstablishmentCommand_t   initiateKE;
  ZCL_ConfirmKeyCommand_t                 confirmKey;
  ZCL_TerminateKeyEstablishmentCommand_t  terminateKE;
  ZCL_EphemeralDataCommand_t              ephemeralData;
} ZCL_KECommandPayload_t;

BEGIN_PACK
typedef struct PACK _ZclKeMacBuffer_t
{
  TOP_GUARD
  volatile uint8_t keyExpander[SECURITY_KEY_SIZE];
  uint8_t macData[ZCL_KE_MAC_DATA_SIZE];
  volatile uint8_t bitsExpander[AES_MMO_HASH_SIZE];
  BOTTOM_GUARD
} ZclKeMacBuffer_t;
END_PACK

/******************************************************************************
                   Static functions prototype section
******************************************************************************/
static ZCL_Status_t initiateKECommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_InitiateKeyEstablishmentCommand_t *payload);
static ZCL_Status_t ephemeralDataCommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_EphemeralDataCommand_t *payload);
static ZCL_Status_t confirmKeyDataCommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_ConfirmKeyCommand_t *payload);
static ZCL_Status_t terminateKECommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_TerminateKeyEstablishmentCommand_t *payload);

static void keMakeMatchDescReq(void);
static void keSendInitiateKECommand(void);
static void keSendEphemeralDataCommand(void);
static void keSendConfirmKeyDataCommand(void);
static void keSendTerminateKECommand(ShortAddr_t shortAddr, Endpoint_t endpoint, ZCL_TKEStatus_t status);

static void keCalculateMac1(void);
static void keCalculateMac2(void);
static void keGenerateKey(void);
static void keSwitchKey(void);
static void restartKEFired(void);
static void keSetTimeoutInSec(uint16_t timeout, void (*callback)(void));
static void mac1Conf(void);
static void mac2Conf(void);

static void keSendInitiateKECommandRespHandler(ZCL_Notify_t *zclResp);
static void keSendEphemeralDataCommandRespHandler(ZCL_Notify_t *zclResp);
static void keSendConfirmKeyDataCommandRespHandler(ZCL_Notify_t *zclResp);
static void keSendTerminateKECommandRespHandler(ZCL_Notify_t *zclResp);
static void keMatchDescResp(ZDO_ZdpResp_t *zdpResp);

static void keCpyReverse(uint8_t *dst, uint8_t *src, uint8_t size);
static bool keIsRequestInProgress(void);
static void keStopKe(ZCL_SecurityStatus_t status);
static void keTimeoutHandler(void);
static void keCancelTimeout(void);

static int ZCL_GetAnalogRandomSequence(uint8_t *buffer, unsigned long size);

static ZCL_TKEStatus_t keInitiateKeReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_InitiateKeyEstablishmentCommand_t *payload);
static ZCL_TKEStatus_t keInitiateKeRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_InitiateKeyEstablishmentCommand_t *payload);
static ZCL_TKEStatus_t keEphemeralDataReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_EphemeralDataCommand_t *payload);
static ZCL_TKEStatus_t keEphemeralDataRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_EphemeralDataCommand_t *payload);
static ZCL_TKEStatus_t keConfirmKeyReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_ConfirmKeyCommand_t *payload);
static ZCL_TKEStatus_t keConfirmKeyRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_ConfirmKeyCommand_t *payload);

#ifdef _ZSE_CERTIFICATION_
void delayTimerFired(void);
#endif // _ZSE_CERTIFICATION_

/******************************************************************************
                   Global functions prototype section
******************************************************************************/
#ifdef _ZSE_CERTIFICATION_
void setKETimeouts(uint8_t ephemeralTimeout, uint8_t confirmTimeout, uint8_t ephemeralDelay, uint8_t confirmDelay);
void setBadCertificatesProcessing(void);
void setOutOfOrderResponse(void);
#endif // _ZSE_CERTIFICATION_

/******************************************************************************
                   Local variables section
******************************************************************************/
static ZCL_KEClusterServerAttributes_t keClusterServerAttributes = ZCL_DEFINE_EK_CLUSTER_SERVER_ATTRIBUTES();
static ZCL_KEClusterCommands_t keClusterServerCommands =
{
  KE_CLUSTER_COMMANDS(initiateKECommandIndHandler, NULL, ephemeralDataCommandIndHandler, NULL,
                      confirmKeyDataCommandIndHandler, NULL, terminateKECommandIndHandler, NULL)
};
static ZCL_Cluster_t keClusterServer = ZCL_DEFINE_KE_CLUSTER_SERVER(&keClusterServerAttributes, &keClusterServerCommands);

static ZCL_KEClusterClientAttributes_t keClusterClientAttributes = ZCL_DEFINE_EK_CLUSTER_CLIENT_ATTRIBUTES();
static ZCL_KEClusterCommands_t keClusterClientCommands =
{
  KE_CLUSTER_COMMANDS(NULL, initiateKECommandIndHandler, NULL, ephemeralDataCommandIndHandler, NULL,
                      confirmKeyDataCommandIndHandler, NULL, terminateKECommandIndHandler)
};
static ZCL_Cluster_t keClusterClient = ZCL_DEFINE_KE_CLUSTER_CLIENT(&keClusterClientAttributes, &keClusterClientCommands);


static ZCL_Request_t                          keZclReq;
static ZCL_KECommandPayload_t                 keCommandPayloadBuffer;
static ZCL_Request_t                          keTerminateZclReq;
static ZCL_TerminateKeyEstablishmentCommand_t keTerminateKEPayload;

static SYS_Timer_t          keApsTimer;
static ZDO_ZdpReq_t         zdpReq; //Needed for certain discovery
static ZclKEClusterState_t  keState             = ZCL_KE_CLUSTER_INITIAL_STATE;
static bool  keTerminateReqBusy = false;


//For Certicom usage
static unsigned char keLocalEphemeralPrivateKey[SECT163K1_PRIVATE_KEY_SIZE];
static unsigned char keLocalEphemeralPublicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];
static unsigned char keLocalKeyBits[SECT163K1_SHARED_SECRET_SIZE];

static unsigned char keRemoteCertificate[SECT163K1_CERTIFICATE_SIZE];
static unsigned char keRemoteEphemeralPublicKey[SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE];

static uint8_t digest1[AES_MMO_HASH_SIZE];
static uint8_t digest2[AES_MMO_HASH_SIZE];
static uint8_t macvHash[AES_MMO_HASH_SIZE];
static uint8_t macuHash[AES_MMO_HASH_SIZE];
static uint8_t receivedMacuHash[AES_MMO_HASH_SIZE];

static ZclKeMacBuffer_t GUARDED_STRUCT(macBuf);

static SSP_KeyedHashMacReq_t sspKeyedHashReq;

#ifdef _ENDDEVICE_
static uint32_t keIndirectPollRate = ZCL_INDIRECT_POLL_RATE_DURING_KE * 1000;
#endif // _ENDDEVICE_

static ShortAddr_t keRemoteShortAddr = ZCL_KE_INVALID_SHORT_ADDRESS;
static Endpoint_t  keRemoteEndpoint  = ZCL_KE_INVALID_ENDPOINT;
static uint16_t    keRemoteEphemeralDataGenerateTime;
static uint16_t    keRemoteConfirmKeyGenerateTime;

static uint8_t buffForRandSeq[ZCL_KE_INITIATE_RANDOM_SEQ_SIZE];
static bool postponedProcessing = false;
static uint8_t keSeqNum;

#ifdef _ZSE_CERTIFICATION_
static uint8_t ephemeralDataGenerateTime = ZCL_KE_EPHEMERAL_DATA_GENERATE_TIME;
static uint8_t confirmKeyGenerateTime = ZCL_KE_CONFIRM_KEY_GENERATE_TIME;
static uint8_t ephemeralDataGenerateDelay;
static uint8_t confirmKeyGenerateDelay;
static HAL_AppTimer_t delayTimer = {
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = delayTimerFired
};
bool useDelay = false;
bool sendTooLongCertificate = false;
bool passBadCertificates = false;
bool outOfOrder = false;
int8_t certAdditionLength = 0;
#endif // _ZSE_CERTIFICATION_

/******************************************************************************
                   Global variables section
******************************************************************************/
Endpoint_t keLocalEndpoint = ZCL_KE_INVALID_ENDPOINT;
bool keSrvMode = false;
ZCL_KECertificateDescriptor_t keCertificateDescriptor;
ExtAddr_t keRemoteExtAddr = ZCL_KE_INVALID_EXT_ADDRESS;

/******************************************************************************
                   Implementation section
******************************************************************************/
/*************************************************************************************//**
  \brief Validates incoming command.

  \param addressing - command's source address information

  \return true - if command is valid, false - otherwise.
******************************************************************************************/
static inline bool keIncomingCmdIsValid(ZCL_Addressing_t *addressing)
{
  if (!keSrvMode)
    if (addressing->sequenceNumber != keSeqNum)
      return false;

  return ((keRemoteShortAddr == addressing->addr.shortAddress) &&
    ((APS_EXT_ADDRESS != addressing->addrMode) || (keRemoteExtAddr == addressing->addr.extAddress)) &&
    (keRemoteEndpoint == addressing->endpointId));
}

/*************************************************************************************//**
  \brief Resets KE management module.
******************************************************************************************/
void keReset(void)
{
  keRemoteExtAddr   = ZCL_KE_INVALID_EXT_ADDRESS;
  keRemoteShortAddr = ZCL_KE_INVALID_SHORT_ADDRESS;
  keRemoteEndpoint  = ZCL_KE_INVALID_ENDPOINT;
  keSrvMode         = false;

  keLocalEndpoint   = ZCL_KE_INVALID_ENDPOINT;
  memset(&keCertificateDescriptor, 0, sizeof(keCertificateDescriptor));
}

/*************************************************************************************//**
  \brief Inits KE management module.

  \return true if success, false otherwise.
******************************************************************************************/
bool keInitCluster(ExtAddr_t *remoteAddress)
{
  sysAssert(remoteAddress, KE_INIT_CLUSTER_0);
  COPY_EXT_ADDR(keRemoteExtAddr, *remoteAddress);

  //Check the initial settings
  if ((ZCL_KE_INVALID_EXT_ADDRESS == keRemoteExtAddr) ||
      (ZCL_KE_INVALID_ENDPOINT == keLocalEndpoint))
    return false;

  //Generate Random Seq using RF
  SYS_GetRandomSequence(buffForRandSeq, ZCL_KE_INITIATE_RANDOM_SEQ_SIZE);

  keState = ZCL_KE_CLUSTER_IDLE_STATE;

  return true;
}

/**************************************************************************//**
  \brief This function copies size bytes of random data into buffer.

  \param: buffer - This is an unsigned char array of size at least sz to hold
   the random data.
  \param: size - The number of bytes of random data to compute and store.

  \return:  0 Indicates successful completion.
******************************************************************************/
static int ZCL_GetAnalogRandomSequence(uint8_t *buffer, unsigned long size)
{
  memcpy(buffer, buffForRandSeq, size);
  if (MAC_IsOwnExtAddr(&keRemoteExtAddr))
    SYS_GetRandomSequence(buffForRandSeq, ZCL_KE_INITIATE_RANDOM_SEQ_SIZE);
  return 0;
}

/*************************************************************************************//**
  \brief Starts KE management module.

  \return true if success, false otherwise.
******************************************************************************************/
bool keStartKE(void)
{
  if (ZCL_KE_CLUSTER_IDLE_STATE != keState)
    return false;

  if (!MAC_IsOwnExtAddr(&keRemoteExtAddr))
  {
    const ShortAddr_t *shortAddr = NWK_GetShortByExtAddress(&keRemoteExtAddr);

    if (shortAddr)
      keRemoteShortAddr = *shortAddr;
    else
      // this is only an assumption. coordinator is trust center!!!
      keRemoteShortAddr = 0;
    keMakeMatchDescReq();
  }

  return true;
}

/*************************************************************************************//**
  \brief Performs discovery of KE Cluster on ESP device
******************************************************************************************/
static void keMakeMatchDescReq(void)
{
  ZDO_MatchDescReq_t *zdoMatchDescReq = &zdpReq.req.reqPayload.matchDescReq;

  keRemoteEndpoint = ZCL_KE_INVALID_ENDPOINT;

  zdpReq.ZDO_ZdpResp = keMatchDescResp;
  zdpReq.reqCluster = MATCH_DESCRIPTOR_CLID;
  zdpReq.dstAddrMode = APS_SHORT_ADDRESS;
  zdpReq.dstAddress.shortAddress = keRemoteShortAddr;

  zdoMatchDescReq->nwkAddrOfInterest = keRemoteShortAddr;
  zdoMatchDescReq->profileId = PROFILE_ID_SMART_ENERGY;
  zdoMatchDescReq->numInClusters = 1;
  zdoMatchDescReq->numOutClusters = 0;
  zdoMatchDescReq->inClusterList[0] = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;

  keState = ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_SENDING_STATE;

  ZDO_ZdpReq(&zdpReq);
}

/*************************************************************************************//**
  \brief ZDO ZDP Match Descriptor Response handler.

  \param zdpResp - response parameters' structure. See ZDO_ZdpResp_t type declaration
    for details.
******************************************************************************************/
static void keMatchDescResp(ZDO_ZdpResp_t *zdpResp)
{
  ZDO_MatchDescResp_t *zdoMatchResp = &zdpResp->respPayload.matchDescResp;

  if (ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_SENDING_STATE == keState)
  {
    if (!keSrvMode)
    {
      if (ZDO_SUCCESS_STATUS == zdpResp->respPayload.status)
      {
        keRemoteEndpoint = zdoMatchResp->matchList[0];
        keRemoteShortAddr = zdoMatchResp->nwkAddrOfInterest;
      }
      else if (ZDO_CMD_COMPLETED_STATUS == zdpResp->respPayload.status)
      {
        if ((ZCL_KE_INVALID_ENDPOINT != keRemoteEndpoint) &&
            (ZCL_KE_INVALID_SHORT_ADDRESS != keRemoteShortAddr))
        {
          keSendInitiateKECommand();
        }
        else
        { // match discovery was completed but response was not received
          keState = ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_TERMINATE_STATE;
          keStopKe(ZCL_SECURITY_STATUS_DISCOVERY_FAIL);
        }
      }
      else
      {
        keStopKe(ZCL_SECURITY_STATUS_DISCOVERY_FAIL);
      }
    }
  }
  else
  {
    if (ZDO_CMD_COMPLETED_STATUS == zdpResp->respPayload.status)
      keStopKe(ZCL_SECURITY_STATUS_TERMINATED);
  }

  SYS_PostEvent(BC_EVENT_KE_CLUSTER_MATCH_DESC_RESP, (uintptr_t)zdpResp);
}

/*************************************************************************************//**
  \brief Sends Key Establishment command (request or response - depends on keSrvMode).
******************************************************************************************/
static void keSendInitiateKECommand(void)
{
  ZCL_InitiateKeyEstablishmentCommand_t *buf = &keCommandPayloadBuffer.initiateKE;

#if CERTICOM_SUPPORT == 1
  //Generating the ephemeral public and private Key Pair
  ZSE_ECCGenerateKey(keLocalEphemeralPrivateKey, keLocalEphemeralPublicKey, ZCL_GetAnalogRandomSequence, yield, YIELD_LEVEL);
#endif // CERTICOM_SUPPORT == 1

  //Prepare payload
  buf->keyEstablishmentSuite        = ZCL_KE_CBKE_ECMQV_KEY_ESTABLISHMENT_SUITE_ID;
#ifdef _ZSE_CERTIFICATION_
  buf->ephemeralDataGenerateTime    = ephemeralDataGenerateTime;
  buf->confirmKeyGenerateTime       = confirmKeyGenerateTime;
#else
  buf->ephemeralDataGenerateTime    = ZCL_KE_EPHEMERAL_DATA_GENERATE_TIME;
  buf->confirmKeyGenerateTime       = ZCL_KE_CONFIRM_KEY_GENERATE_TIME;
#endif // _ZSE_CERTIFICATION_

  memcpy(buf->identify, &keCertificateDescriptor.certificate, sizeof(ZclCertificate_t));

  if (!keSrvMode)
  {
    //ZCL KE Cluster Initiate Key Establishment Command Request sending
    keZclReq.id = ZCL_KE_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_SERVER;
    keSeqNum = ZCL_GetNextSeqNumber();
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;

    // the crutch to asynchornize data request
#ifdef _ENDDEVICE_
    // store config server's indirect poll rate
    uint32_t oldIndirectPollRate;
    CS_ReadParameter(CS_INDIRECT_POLL_RATE_ID, &oldIndirectPollRate);
    CS_WriteParameter(CS_INDIRECT_POLL_RATE_ID, &keIndirectPollRate);

    keIndirectPollRate = oldIndirectPollRate;

    if (ZDO_StopSyncReq() == ZDO_SUCCESS_STATUS)
      ZDO_StartSyncReq();
#endif /* _ENDDEVICE_ */

  }
  else
  {
    //ZCL KE Cluster Initiate Key Establishment Command Response sending
    keZclReq.id = ZCL_KE_INITIATE_KEY_ESTABLISHMENT_RESPONSE_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT;
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;
  }

  keZclReq.dstAddressing.profileId  = PROFILE_ID_SMART_ENERGY;
  keZclReq.dstAddressing.clusterId  = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
  keZclReq.dstAddressing.endpointId = keRemoteEndpoint;
  keZclReq.dstAddressing.addrMode   = APS_SHORT_ADDRESS;
  keZclReq.dstAddressing.addr.shortAddress = keRemoteShortAddr;
  keZclReq.endpointId = keLocalEndpoint;
  keZclReq.requestPayload = (uint8_t *) buf;
  keZclReq.requestLength  = sizeof(ZCL_InitiateKeyEstablishmentCommand_t);
#ifdef _ZSE_CERTIFICATION_
  if (sendTooLongCertificate)
  {
    keZclReq.requestLength += certAdditionLength;
    sendTooLongCertificate = false;
  }
#endif // _ZSE_CERTIFICATION_

  keZclReq.defaultResponse = 1;
  keZclReq.ZCL_Notify = keSendInitiateKECommandRespHandler;
  keState = ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE;
  ZCL_CommandReq(&keZclReq);
}

/*************************************************************************************//**
  \brief Response handler for KE Initiate KE Command.

  \param zclResp - pointer to response parameters structure. See ZCL_responce_t declaration.
******************************************************************************************/
static void keSendInitiateKECommandRespHandler(ZCL_Notify_t *zclResp)
{
  sysAssert(ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE == keState, KE_WRONG_STATE_0);
  if (ZCL_SUCCESS_STATUS == zclResp->status)
  {
    /* If response was already received - continue Key Establishment */
    if (postponedProcessing)
    {
#ifdef _ZSE_CERTIFICATION_
      if (useDelay)
      {
        delayTimer.interval = (uint32_t)ephemeralDataGenerateDelay * 1000;
        HAL_StartAppTimer(&delayTimer);
      }
      else
#endif // _ZSE_CERTIFICATION_
        keSendEphemeralDataCommand();
      postponedProcessing = false;
      return;
    }

    if (!keSrvMode)
    {
      keState = ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_WAITING_STATE;
      keSetTimeoutInSec(ZCL_KE_INITIATE_RESPONSE_WAITING_TIME, keTimeoutHandler);
    }
    else
    {
      keState = ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE;
      keSetTimeoutInSec(keRemoteEphemeralDataGenerateTime, keTimeoutHandler);
    }
  }
  else
    keStopKe(ZCL_SECURITY_STATUS_SEND_COMMAND_FAIL);
}

/*************************************************************************************//**
  \brief Indicates that KE Initiate Command is received.

  \param addressing - command's source address information
  \param payloadLength - command's payload length
  \param payload - command's payload
  \return command's processing status, in case if not SUCCESS - default response will be
    generated
******************************************************************************************/
static ZCL_Status_t initiateKECommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_InitiateKeyEstablishmentCommand_t *payload)
{
  ZclCertificate_t *zclCertificate = (ZclCertificate_t *) payload->identify;
  ZCL_TKEStatus_t tkeStatus = ZCL_TKE_NO_STATUS;

  if (ZCL_CLUSTER_SIDE_CLIENT == addressing->clusterSide)
    //Server (Responder)
    keSrvMode = true;
  else
    //Client (Initiator)
    keSrvMode = false;

  keRemoteEphemeralDataGenerateTime = (uint16_t)payload->ephemeralDataGenerateTime + 2;
  keRemoteConfirmKeyGenerateTime = (uint16_t)payload->confirmKeyGenerateTime + 2;
  keCancelTimeout();

#ifdef _ZSE_CERTIFICATION_
  if (!passBadCertificates)
#endif // _ZSE_CERTIFICATION_
  {
    if (sizeof(ZCL_InitiateKeyEstablishmentCommand_t) <= payloadLength)
    {
      if (zclCertificate->issuer != keCertificateDescriptor.certificate.issuer)
      {
        tkeStatus = ZCL_TKE_UNKNOWN_ISSUER_STATUS;
      }
      else if (ZCL_KE_CBKE_ECMQV_KEY_ESTABLISHMENT_SUITE_ID != payload->keyEstablishmentSuite)
      {
        tkeStatus = ZCL_TKE_UNSUPPORTED_SUITE_STATUS;
      }
      else if ((ZCL_KE_INVALID_EPHEMERAL_DATA_GENERATE_TIME == payload->ephemeralDataGenerateTime) ||
              (ZCL_KE_INVALID_CONFIRM_KEY_GENERATE_TIME == payload->confirmKeyGenerateTime))
      {
        tkeStatus = ZCL_TKE_BAD_MESSAGE_STATUS;
      }
      else if (keSrvMode)
      {
        keSeqNum = addressing->sequenceNumber;
        tkeStatus = keInitiateKeReqProcessing(addressing, payload);
      }
      else
      {
        tkeStatus = keInitiateKeRespProcessing(addressing, payload);
      }
    }
    else
      tkeStatus = ZCL_TKE_BAD_MESSAGE_STATUS;
  }
#ifdef _ZSE_CERTIFICATION_
  if (passBadCertificates)
  {
    keSeqNum = addressing->sequenceNumber;
    tkeStatus = keInitiateKeReqProcessing(addressing, payload);
  }
#endif // _ZSE_CERTIFICATION_
  if (ZCL_TKE_NO_STATUS != tkeStatus)
  {
    keSendTerminateKECommand(addressing->addr.shortAddress, addressing->endpointId, tkeStatus);
  }

  SYS_PostEvent((keSrvMode) ? BC_EVENT_KE_CLUSTER_INITIATE_KE_REQ : BC_EVENT_KE_CLUSTER_INITIATE_KE_RESP, (uintptr_t)payload);

  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************************//**
  \brief Processes Initiate Key Establishment Request command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keInitiateKeReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_InitiateKeyEstablishmentCommand_t *payload)
{
  ExtAddr_t keRemoteExtAddrTmp;
  ZclCertificate_t *zclCertificate = (ZclCertificate_t *) payload->identify;

  //Server (Responder)
  if (ZCL_KE_CLUSTER_IDLE_STATE == keState)
  {
    //Save the client address (short and ext) and endpoint
    keRemoteShortAddr = addressing->addr.shortAddress;
    keRemoteEndpoint = addressing->endpointId;
    keRemoteExtAddr = addressing->addr.extAddress;

    if (APS_EXT_ADDRESS != addressing->addrMode)
    {
      //There is no information about client in nwkAddrMapTable
      //Let's get it from the Implicit Certificate
      keRemoteExtAddr = zclCertificate->subject;
    }
    else
    {
      keCpyReverse((uint8_t *) &keRemoteExtAddrTmp, (uint8_t *) &keRemoteExtAddr, sizeof(ExtAddr_t));
#ifndef _ZSE_CERTIFICATION_
      if (!IS_EQ_EXT_ADDR(keRemoteExtAddrTmp, zclCertificate->subject))
        return ZCL_TKE_BAD_MESSAGE_STATUS;
#endif
    }

    memcpy(keRemoteCertificate, payload->identify, SECT163K1_CERTIFICATE_SIZE);
#ifdef _ZSE_CERTIFICATION_
    if (outOfOrder)
      keSendEphemeralDataCommand();
    else
#endif // _ZSE_CERTIFICATION
      keSendInitiateKECommand();

  }
  else
    return ZCL_TKE_NO_RESOURCES_STATUS;

  return ZCL_TKE_NO_STATUS;
}

/*************************************************************************************//**
  \brief Processes Initiate Key Establishment Response command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keInitiateKeRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_InitiateKeyEstablishmentCommand_t *payload)
{
  //Client (Initiator)
  postponedProcessing = (ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE == keState);

  if (ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_WAITING_STATE == keState || postponedProcessing)
  {
    if (keIncomingCmdIsValid(addressing))
    {
      ExtAddr_t keRemoteExtAddrTmp;
      ZclCertificate_t *zclCertificate = (ZclCertificate_t *) payload->identify;

      keCpyReverse((uint8_t *) &keRemoteExtAddrTmp, (uint8_t *) &keRemoteExtAddr, sizeof(ExtAddr_t));
      if (!IS_EQ_EXT_ADDR(keRemoteExtAddrTmp, zclCertificate->subject))
        return ZCL_TKE_BAD_MESSAGE_STATUS;

      memcpy(keRemoteCertificate, payload->identify, SECT163K1_CERTIFICATE_SIZE);
      if (!postponedProcessing)
      {
#ifdef _ZSE_CERTIFICATION_
        if (useDelay)
        {
          delayTimer.interval = (uint32_t)ephemeralDataGenerateDelay * 1000;
          HAL_StartAppTimer(&delayTimer);
        }
        else
#endif // _ZSE_CERTIFICATION_
          keSendEphemeralDataCommand();
      }
      return ZCL_TKE_NO_STATUS;
    }
  }

  postponedProcessing = false;
  return ZCL_TKE_BAD_MESSAGE_STATUS;
}

/*************************************************************************************//**
  \brief Sends Ephemeral Data KE Command.
******************************************************************************************/
static void keSendEphemeralDataCommand(void)
{
  ZCL_EphemeralDataCommand_t *buf = &keCommandPayloadBuffer.ephemeralData;

  //Prepare payload
  memcpy(buf->data, keLocalEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  if (!keSrvMode)
  {
    //ZCL KE Cluster Initiate Key Establishment Command Request sending
    keZclReq.id = ZCL_KE_EPHEMERAL_DATA_REQUEST_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_SERVER;
    keSeqNum = ZCL_GetNextSeqNumber();
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;

    // the crutch to asynchornize data request
#ifdef _ENDDEVICE_
    if (ZDO_StopSyncReq() == ZDO_SUCCESS_STATUS)
      ZDO_StartSyncReq();
#endif /* _ENDDEVICE_ */
  }
  else
  {
    //ZCL KE Cluster Initiate Key Establishment Command Response sending
    keZclReq.id = ZCL_KE_EPHEMERAL_DATA_RESPONSE_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT;
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;
  }
  keZclReq.dstAddressing.profileId  = PROFILE_ID_SMART_ENERGY;
  keZclReq.dstAddressing.clusterId  = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
  keZclReq.dstAddressing.endpointId = keRemoteEndpoint;
  keZclReq.dstAddressing.addrMode   = APS_SHORT_ADDRESS;
  keZclReq.dstAddressing.addr.shortAddress = keRemoteShortAddr;
  keZclReq.endpointId = keLocalEndpoint;
  keZclReq.requestPayload = (uint8_t *) buf;
  keZclReq.requestLength  = sizeof(ZCL_EphemeralDataCommand_t);
  keZclReq.defaultResponse = 1;
  keZclReq.ZCL_Notify = keSendEphemeralDataCommandRespHandler;
  keState = ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_SENDING_STATE;

  ZCL_CommandReq(&keZclReq);
}

/*************************************************************************************//**
  \brief Response handler for KE Ephemeral Data Command.

  \param zclResp - pointer to response parameters structure. See ZCL_responce_t declaration.
******************************************************************************************/
static void keSendEphemeralDataCommandRespHandler(ZCL_Notify_t *zclResp)
{
  sysAssert(ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_SENDING_STATE == keState, KE_WRONG_STATE_1);
  if (ZCL_SUCCESS_STATUS == zclResp->status)
  {
    if (!keSrvMode)
    {
      keState = ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE;
      keSetTimeoutInSec(keRemoteEphemeralDataGenerateTime, keTimeoutHandler);
    }
    else
    {
      keSetTimeoutInSec(keRemoteConfirmKeyGenerateTime, keTimeoutHandler);
      keGenerateKey();
    }
  }
  else
    keStopKe(ZCL_SECURITY_STATUS_SEND_COMMAND_FAIL);
}

/*************************************************************************************//**
  \brief Indicates that KE Ephemeral Data Command is received.

  \param addressing - command's source address information
  \param payloadLength - command's payload length
  \param payload - command's payload
  \return command's processing status, in case if not SUCCESS - default response will be
    generated
******************************************************************************************/
static ZCL_Status_t ephemeralDataCommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_EphemeralDataCommand_t *payload)
{
  ZCL_TKEStatus_t tkeStatus = ZCL_TKE_NO_STATUS;
  keCancelTimeout();

  (void) payloadLength;

  if (ZCL_CLUSTER_SIDE_CLIENT == addressing->clusterSide)
    //Server (Responder)
    keSrvMode = true;
  else
    //Client (Initiator)
    keSrvMode = false;

  //Some parameters checking
  if (keIncomingCmdIsValid(addressing))
  {
    if (keSrvMode)
    {
      keSeqNum = addressing->sequenceNumber;
      tkeStatus = keEphemeralDataReqProcessing(addressing, payload);
    }
    else
      tkeStatus = keEphemeralDataRespProcessing(addressing, payload);
  }
  else
    tkeStatus = ZCL_TKE_BAD_MESSAGE_STATUS;

  if (ZCL_TKE_NO_STATUS != tkeStatus)
  {
    keSendTerminateKECommand(addressing->addr.shortAddress, addressing->endpointId, tkeStatus);
  }

  SYS_PostEvent((keSrvMode) ? BC_EVENT_KE_CLUSTER_EPH_DATA_REQ : BC_EVENT_KE_CLUSTER_EPH_DATA_RESP, (uintptr_t)payload);

  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************************//**
  \brief Processes Ephemeral Data Request command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keEphemeralDataReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_EphemeralDataCommand_t *payload)
{
  (void)addressing;
  postponedProcessing = (ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE == keState);

  if (ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE == keState || postponedProcessing)
  {
    //Save the remote node Ephemeral Public Key
    memcpy(keRemoteEphemeralPublicKey, payload->data, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
    if (!postponedProcessing)
    {
#ifdef _ZSE_CERTIFICATION_
      if (useDelay)
      {
        delayTimer.interval = (uint32_t)ephemeralDataGenerateDelay * 1000;
        HAL_StartAppTimer(&delayTimer);
      }
      else
#endif // _ZSE_CERTIFICATION_
      keSendEphemeralDataCommand();
    }
    return ZCL_TKE_NO_STATUS;
  }
  else
    return ZCL_TKE_BAD_MESSAGE_STATUS;
}

/*************************************************************************************//**
  \brief Processes Ephemeral Data Response command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keEphemeralDataRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_EphemeralDataCommand_t *payload)
{
  (void)addressing;
  postponedProcessing = (ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_SENDING_STATE == keState);

  if (ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE == keState || postponedProcessing)
  {
    //Save the remote node Ephemeral Public Key
    memcpy(keRemoteEphemeralPublicKey, payload->data, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
    keGenerateKey();
    return ZCL_TKE_NO_STATUS;
  }
  else
    return ZCL_TKE_BAD_MESSAGE_STATUS;
}

/*************************************************************************************//**
  \brief Sends Confirm Key Data KE Command
******************************************************************************************/
static void keSendConfirmKeyDataCommand(void)
{
  ZCL_ConfirmKeyCommand_t *buf = &keCommandPayloadBuffer.confirmKey;

  //Prepare payload

  if (!keSrvMode)
  {
    memcpy(buf->mac, macuHash, AES_MMO_HASH_SIZE);

    //ZCL KE Cluster Initiate Key Establishment Command Request sending
    keZclReq.id = ZCL_KE_CONFIRM_KEY_DATA_REQUEST_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_SERVER;
    keSeqNum = ZCL_GetNextSeqNumber();
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;

    // the crutch to asynchornize data request
#ifdef _ENDDEVICE_
    if (ZDO_StopSyncReq() == ZDO_SUCCESS_STATUS)
      ZDO_StartSyncReq();
#endif /* _ENDDEVICE_ */

  }
  else
  {
    memcpy(buf->mac, macvHash, AES_MMO_HASH_SIZE);
    //ZCL KE Cluster Initiate Key Establishment Command Response sending
    keZclReq.id = ZCL_KE_CONFIRM_KEY_DATA_RESPONSE_COMMAND_ID;
    keZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT;
    keZclReq.dstAddressing.sequenceNumber = keSeqNum;
  }
  keZclReq.dstAddressing.profileId  = PROFILE_ID_SMART_ENERGY;
  keZclReq.dstAddressing.clusterId  = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
  keZclReq.dstAddressing.endpointId = keRemoteEndpoint;
  keZclReq.dstAddressing.addrMode   = APS_SHORT_ADDRESS;
  keZclReq.dstAddressing.addr.shortAddress = keRemoteShortAddr;
  keZclReq.endpointId = keLocalEndpoint;

  keZclReq.requestPayload = (uint8_t *) buf;
  keZclReq.requestLength  = sizeof(ZCL_ConfirmKeyCommand_t);
  keZclReq.defaultResponse = 1;
  keZclReq.ZCL_Notify = keSendConfirmKeyDataCommandRespHandler;
  keState = ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_SENDING_STATE;
  ZCL_CommandReq(&keZclReq);
}

/*************************************************************************************//**
  \brief Response handler for KE Confirm Key Data Command.

  \param zclResp - pointer to response parameters structure. See ZCL_responce_t declaration.
******************************************************************************************/
static void keSendConfirmKeyDataCommandRespHandler(ZCL_Notify_t *zclResp)
{
  sysAssert(ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_SENDING_STATE == keState, KE_WRONG_STATE_2);
  if (ZCL_SUCCESS_STATUS == zclResp->status)
  {
    if (!keSrvMode)
    {
      if (postponedProcessing)
      {
        keSwitchKey();
        postponedProcessing = false;
      }
      else
      {
        keState = ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE;
        keSetTimeoutInSec(keRemoteConfirmKeyGenerateTime, keTimeoutHandler);
      }
    }
    else
    {
      keState = ZCL_KE_CLUSTER_IDLE_STATE;
      keSwitchKey();
    }
  }
  else
    keStopKe(ZCL_SECURITY_STATUS_SEND_COMMAND_FAIL);
}

/*************************************************************************************//**
  \brief Indicates that KE Key Data Command is received.

  \param addressing - command's source address information
  \param payloadLength - command's payload length
  \param payload - command's payload
  \return command's processing status, in case if not SUCCESS - default response will be
    generated
******************************************************************************************/
static ZCL_Status_t confirmKeyDataCommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_ConfirmKeyCommand_t *payload)
{
  ZCL_TKEStatus_t tkeStatus = ZCL_TKE_NO_STATUS;

  if (ZCL_CLUSTER_SIDE_CLIENT == addressing->clusterSide)
    //Server (Responder)
    keSrvMode = true;
  else
    //Client (Initiator)
    keSrvMode = false;

  keCancelTimeout();
  SYS_PostEvent((keSrvMode) ? BC_EVENT_KE_CLUSTER_CONF_KEY_REQ : BC_EVENT_KE_CLUSTER_CONF_KEY_RESP, (uintptr_t)payload);

  (void) payloadLength;

  //Some parameters checking
  if (keIncomingCmdIsValid(addressing))
  {
    if (keSrvMode)
    {
      keSeqNum = addressing->sequenceNumber;
      tkeStatus = keConfirmKeyReqProcessing(addressing, payload);
    }
    else
      tkeStatus = keConfirmKeyRespProcessing(addressing, payload);
  }
  else
    tkeStatus = ZCL_TKE_BAD_MESSAGE_STATUS;

  if (ZCL_TKE_NO_STATUS != tkeStatus)
  {
    keSendTerminateKECommand(addressing->addr.shortAddress, addressing->endpointId, tkeStatus);
  }

  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************************//**
  \brief Processes Confirm Key Request command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keConfirmKeyReqProcessing(ZCL_Addressing_t *addressing,
  ZCL_ConfirmKeyCommand_t *payload)
{
  (void)addressing;

  if (ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE == keState)
  { // data has already calculated. we can send reply
    if (0 == memcmp(macuHash, payload->mac, AES_MMO_HASH_SIZE))
    {
#ifdef _ZSE_CERTIFICATION_
      if (useDelay)
      {
        delayTimer.interval = (uint32_t)ephemeralDataGenerateDelay * 1000;
        HAL_StartAppTimer(&delayTimer);
      }
      else
#endif // _ZSE_CERTIFICATION_
        keSendConfirmKeyDataCommand();
    }
    else
    {
#ifdef _ZSE_CERTIFICATION_
      if (passBadCertificates)
        keSendConfirmKeyDataCommand();//return ZCL_TKE_NO_STATUS;
      else
#endif // _ZSE_CERTIFICATION_
        return ZCL_TKE_BAD_KEY_CONFIRM_STATUS;
    }
  }
  else
  { // data has not been ready. Store received hash to compare later
    postponedProcessing = true;
    memcpy(receivedMacuHash, payload->mac, AES_MMO_HASH_SIZE);
  }

  return ZCL_TKE_NO_STATUS;
}

/*************************************************************************************//**
  \brief Processes Confirm Key Response command.

  \param addressing - command's source address information
  \param payload - command payload

  \return processing status
******************************************************************************************/
static ZCL_TKEStatus_t keConfirmKeyRespProcessing(ZCL_Addressing_t *addressing,
  ZCL_ConfirmKeyCommand_t *payload)
{
  (void)addressing;
  postponedProcessing = (ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_SENDING_STATE == keState);

  if (ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE == keState || postponedProcessing)
  {
#ifdef _ENDDEVICE_
     // restore indirect poll rate
     uint32_t oldIndirectPollRate;
     CS_ReadParameter(CS_INDIRECT_POLL_RATE_ID, &oldIndirectPollRate);
     CS_WriteParameter(CS_INDIRECT_POLL_RATE_ID, &keIndirectPollRate);

     keIndirectPollRate = oldIndirectPollRate;
#endif /* _ENDDEIVE_ */
    if (0 == memcmp(macvHash, payload->mac, AES_MMO_HASH_SIZE))
    {
      if (!postponedProcessing)
        keSwitchKey();
    }
    else
    {
      return ZCL_TKE_BAD_KEY_CONFIRM_STATUS;
    }
  }
  else
    return ZCL_TKE_BAD_MESSAGE_STATUS;

  return ZCL_TKE_NO_STATUS;
}

/*************************************************************************************//**
  \brief Sends KE Termionate command with specific status.

  \param shortAddr - ahort destanation address of the command
  \param endpoint - command destination endpoint
  \param status - statu of KE Terminate command
******************************************************************************************/
static void keSendTerminateKECommand(ShortAddr_t shortAddr, Endpoint_t endpoint, ZCL_TKEStatus_t status)
{
  ZCL_TerminateKeyEstablishmentCommand_t *buf = &keTerminateKEPayload;

  if (!keTerminateReqBusy)
  {
    buf->statusCode = status;
    buf->waitTime = TERMINATE_BASE_WAIT_TIME + (SYS_GetRandomNumber() & 0xf);
    buf->keyEstablishmentSuite = ZCL_KE_CBKE_ECMQV_KEY_ESTABLISHMENT_SUITE_ID;

    keTerminateZclReq.id = ZCL_KE_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID;
    keTerminateZclReq.endpointId = keLocalEndpoint;
    keTerminateZclReq.dstAddressing.profileId  = PROFILE_ID_SMART_ENERGY;
    keTerminateZclReq.dstAddressing.clusterId  = ZCL_KEY_ESTABLISHMENT_CLUSTER_ID;
    keTerminateZclReq.dstAddressing.endpointId = endpoint;
    keTerminateZclReq.dstAddressing.addrMode   = APS_SHORT_ADDRESS;
    keTerminateZclReq.dstAddressing.addr.shortAddress = shortAddr;
    if (!keSrvMode)
      keTerminateZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_SERVER;
    else
      keTerminateZclReq.dstAddressing.clusterSide = ZCL_CLUSTER_SIDE_CLIENT;
    keSeqNum = ZCL_GetNextSeqNumber();
    keTerminateZclReq.dstAddressing.sequenceNumber = keSeqNum;

    keTerminateZclReq.requestPayload = (uint8_t *) buf;
    keTerminateZclReq.requestLength  = sizeof(ZCL_TerminateKeyEstablishmentCommand_t);
    keTerminateZclReq.defaultResponse = 1;
    keTerminateZclReq.ZCL_Notify = keSendTerminateKECommandRespHandler;
    keTerminateReqBusy = true;

    if ((keRemoteShortAddr == shortAddr) &&
        (keRemoteEndpoint == endpoint))
    {
      keRemoteExtAddr = ZCL_KE_INVALID_EXT_ADDRESS;
      keRemoteShortAddr = ZCL_KE_INVALID_SHORT_ADDRESS;
      keRemoteEndpoint  = ZCL_KE_INVALID_ENDPOINT;
    }

    ZCL_CommandReq(&keTerminateZclReq);
  }
}

/*************************************************************************************//**
  \brief Response handler for KE Terminate Command.

  \param zclResp - pointer to response parameters structure. See ZCL_responce_t declaration.
******************************************************************************************/
static void keSendTerminateKECommandRespHandler(ZCL_Notify_t *zclResp)
{
  (void)zclResp;

  sysAssert(keTerminateReqBusy, KE_WRONG_STATE_5);
  if ((ZCL_KE_INVALID_ENDPOINT == keRemoteEndpoint) ||
      (ZCL_KE_INVALID_SHORT_ADDRESS == keRemoteShortAddr) ||
      (ZCL_KE_INVALID_EXT_ADDRESS == keRemoteExtAddr))
  {
    keStopKe(ZCL_SECURITY_STATUS_TERMINATED);
  }
  keTerminateReqBusy = false;
}

/*************************************************************************************//**
  \brief Indicates that KE Terminate Command is received.

  \param addressing - command's source address information
  \param payloadLength - command's payload length
  \param payload - command's payload
  \return command's processing status, in case if not SUCCESS - default response will be
    generated
******************************************************************************************/
static ZCL_Status_t terminateKECommandIndHandler(ZCL_Addressing_t *addressing,
  uint8_t payloadLength, ZCL_TerminateKeyEstablishmentCommand_t *payload)
{
  (void) payloadLength;

  if ((keRemoteShortAddr == addressing->addr.shortAddress) &&
     ((APS_EXT_ADDRESS != addressing->addrMode) || (keRemoteExtAddr == addressing->addr.extAddress)) &&
     (keRemoteEndpoint == addressing->endpointId))
  {
    postponedProcessing = false;
    if (keIsRequestInProgress())
      return ZCL_SUCCESS_STATUS;

    if (ZCL_CLUSTER_SIDE_CLIENT == addressing->clusterSide)
      //Server (Responder)
      keSrvMode = true;
    else
      //Client (Initiator)
      keSrvMode = false;

    switch (payload->statusCode)
    {
      case ZCL_TKE_NO_RESOURCES_STATUS:
        keState = ZCL_KE_CLUSTER_WAITING_STATE;
        keSetTimeoutInSec(payload->waitTime, restartKEFired);
        break;
      case ZCL_TKE_BAD_MESSAGE_STATUS:
      case ZCL_TKE_UNSUPPORTED_SUITE_STATUS:
         /* If the device receives a status of UNKNOWN_SUITE it should examine the
         KeyEstablishmentSuite field to determine if another suite can be used that is
         supported by the partner device. It may re-initiate key establishment using that
         one of the supported suites after waiting the amount of time specified in the Wait
         Time field. If the device does not support any of the types in the
         KeyEstablishmentSuite field, it should not attempt key establishment again with
         that device. */
      case ZCL_TKE_UNKNOWN_ISSUER_STATUS:
      case ZCL_TKE_BAD_KEY_CONFIRM_STATUS:
         /* Add ext addr to black list */
      default:
        keStopKe(ZCL_SECURITY_STATUS_TERMINATED);
        break;
    }
  }

  SYS_PostEvent(BC_EVENT_KE_CLUSTER_TERMINATE_CMD, (uintptr_t)&payload->statusCode);
  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************************//**
  \brief

  \param
  \return
******************************************************************************************/
static void keGenerateKey(void)
{
  uint8_t hash1[25];
  uint8_t hash2[25];
  uint8_t *keLocalPrivateKey = keCertificateDescriptor.privateKey;
  uint8_t *keLocalPublicKey = keCertificateDescriptor.publicKey;

  //Derive the shared secret using the ECMQV primitive
  //Z = ECC_GenerateSharedSecret
  ZSE_ECCKeyBitGenerate(keLocalPrivateKey , keLocalEphemeralPrivateKey, keLocalEphemeralPublicKey,
                          keRemoteCertificate, keRemoteEphemeralPublicKey, keLocalPublicKey /*CA Public Key*/,
                          keLocalKeyBits, SSP_BcbHash, yield, YIELD_LEVEL);
  //Derive the Keying data
  //Hash-1 = Z || 00 00 00 01 || SharedData
  //Hash-2 = Z || 00 00 00 02 || SharedData

  //Hash-1
  //Concatenation
  memcpy((uint8_t *) hash1, keLocalKeyBits, SECT163K1_SHARED_SECRET_SIZE /*21*/);
  hash1[21] = 0; hash1[22] = 0; hash1[23] = 0; hash1[24] = 1;

  //hash
  SSP_BcbHash(digest1, 25, hash1);


  //Hash-2
  //Concatenation
  memcpy((uint8_t *) hash2, keLocalKeyBits, SECT163K1_SHARED_SECRET_SIZE /*21*/);
  hash2[21] = 0; hash2[22] = 0; hash2[23] = 0; hash2[24] = 2;

  //hash
  SSP_BcbHash(digest2, 25, hash2);

  keCalculateMac1();
}

/*************************************************************************************//**
  \brief

  \param
  \return
******************************************************************************************/
static void keCalculateMac1(void)
{
  ZclCertificate_t *remoteCertificate = (ZclCertificate_t *) &keRemoteCertificate;
  CHECK_GUARDS(&macBuf, ZCL_MEMORY_CORRUPTION_1);
  uint8_t *dst = macBuf.macData;

  keState = ZCL_KE_CLUSTER_MAC1_CALCULATING_STATE;

  if (!keSrvMode)
  {
    *dst = ZCL_KE_INITATOR_ADDITIONAL_MESSAGE_COMPONENT;
  }
  else
  {
    *dst = ZCL_KE_RESPONDER_ADDITIONAL_MESSAGE_COMPONENT;
  }
  dst += ZCL_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE;

  memcpy(dst,
    GET_FIELD_PTR(&keCertificateDescriptor.certificate, ZclCertificate_t, subject),
    sizeof(ExtAddr_t));
  dst += sizeof(ExtAddr_t);

  memcpy(dst,
    GET_FIELD_PTR(remoteCertificate, ZclCertificate_t, subject),
    sizeof(ExtAddr_t));
  dst += sizeof(ExtAddr_t);

  memcpy(dst, keLocalEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  dst += SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE;

  memcpy(dst, keRemoteEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  sspKeyedHashReq.text = macBuf.macData;
  sspKeyedHashReq.key = digest1;
  sspKeyedHashReq.textSize = ZCL_KE_MAC_DATA_SIZE;
  if (!keSrvMode)
  {
    sspKeyedHashReq.hash_i = macuHash;
  }
  else
  {
    sspKeyedHashReq.hash_i = macvHash;
  }
  sspKeyedHashReq.SSP_KeyedHashMacConf = mac1Conf;
  SSP_KeyedHashMacReq(&sspKeyedHashReq);
}

/*************************************************************************************//**
  \brief

  \param
  \return
******************************************************************************************/
static void mac1Conf(void)
{
  if (ZCL_KE_CLUSTER_MAC1_CALCULATING_STATE == keState)
  {
    keCalculateMac2();
  }
}

/*************************************************************************************//**
  \brief

  \param
  \return
******************************************************************************************/
static void keCalculateMac2(void)
{
  ZclCertificate_t *remoteCertificate = (ZclCertificate_t *) &keRemoteCertificate;
  CHECK_GUARDS(&macBuf, ZCL_MEMORY_CORRUPTION_2);
  uint8_t *dst = macBuf.macData;

  keState = ZCL_KE_CLUSTER_MAC2_CALCULATING_STATE;

  if (!keSrvMode)
  {
    *dst = ZCL_KE_RESPONDER_ADDITIONAL_MESSAGE_COMPONENT;
  }
  else
  {
    *dst = ZCL_KE_INITATOR_ADDITIONAL_MESSAGE_COMPONENT;
  }
  dst += ZCL_KE_ADDITIONAL_MESSAGE_COMPONENT_SIZE;

  memcpy(dst,
    GET_FIELD_PTR(remoteCertificate, ZclCertificate_t, subject),
    sizeof(ExtAddr_t));
  dst += sizeof(ExtAddr_t);

  memcpy(dst,
    GET_FIELD_PTR(&keCertificateDescriptor.certificate, ZclCertificate_t, subject),
    sizeof(ExtAddr_t));
  dst += sizeof(ExtAddr_t);

  memcpy(dst, keRemoteEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);
  dst += SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE;

  memcpy(dst, keLocalEphemeralPublicKey, SECT163K1_COMPRESSED_PUBLIC_KEY_SIZE);

  sspKeyedHashReq.text = macBuf.macData;
  sspKeyedHashReq.key = digest1;
  sspKeyedHashReq.textSize = ZCL_KE_MAC_DATA_SIZE;
  if (!keSrvMode)
  {
    sspKeyedHashReq.hash_i = macvHash;
  }
  else
  {
    sspKeyedHashReq.hash_i = macuHash;
  }

  sspKeyedHashReq.SSP_KeyedHashMacConf = mac2Conf;
  SSP_KeyedHashMacReq(&sspKeyedHashReq);
}

/*************************************************************************************//**
  \brief

  \param
  \return
******************************************************************************************/
static void mac2Conf(void)
{
  CHECK_GUARDS(&macBuf, ZCL_MEMORY_CORRUPTION_3);
  if (ZCL_KE_CLUSTER_MAC2_CALCULATING_STATE == keState)
  {
    if (!keSrvMode)
    {
      if (!postponedProcessing)
      {
#ifdef _ZSE_CERTIFICATION_
        if (useDelay)
        {
          delayTimer.interval = (uint32_t)confirmKeyGenerateDelay * 1000;
          HAL_StartAppTimer(&delayTimer);
        }
        else
#endif // _ZSE_CERTIFICATION_
          keSendConfirmKeyDataCommand();
      }
    }
    else
    {
      if (postponedProcessing)
      {
        postponedProcessing = false;
        if (0 == memcmp(macuHash, receivedMacuHash, AES_MMO_HASH_SIZE))
          keSendConfirmKeyDataCommand();
        else
          keSendTerminateKECommand(keRemoteShortAddr, keRemoteEndpoint, ZCL_TKE_BAD_KEY_CONFIRM_STATUS);
      }
      else
        keState = ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE;
    }
  }
}

/*************************************************************************************//**
  \brief Sets new established key to APS and saves that KE procedure finished successfully.
******************************************************************************************/
static void keSwitchKey(void)
{
  if (APS_KEY_HANDLE_IS_VALID(APS_SetLinkKey(&keRemoteExtAddr, digest2)))
  {
    APS_SetAuthorizedStatus(&keRemoteExtAddr, true);

    SYS_PostEvent(BC_EVENT_UPDATE_LINK_KEY, (uintptr_t)&keRemoteExtAddr);

    if (!keSrvMode)
    {
      keStopKe(ZCL_SECURITY_STATUS_SUCCESS);
    }
    else
      keSrvMode = false;
  }
}

/*************************************************************************************//**
  \brief TimeOut Timer callback - restarts KE procedure after Terminate command with
    NO_RESOURCES status
******************************************************************************************/
static void restartKEFired(void)
{
  sysAssert(ZCL_KE_CLUSTER_WAITING_STATE == keState, KE_WRONG_STATE_6);
  keSendInitiateKECommand();
}

/*************************************************************************************//**
  \brief Timeout timer expired
******************************************************************************************/
static void keTimeoutHandler(void)
{
  sysAssert(!keIsRequestInProgress(), KE_WRONG_STATE_4);
  keStopKe(ZCL_SECURITY_STATUS_TIMEOUT);
}

/*************************************************************************************//**
  \brief Starts timeout timer
  \param[in] timeout - timeout in seconds
  \param[in] callback - timer callback function pointer
******************************************************************************************/
static void keSetTimeoutInSec(uint16_t timeout, void (*callback)(void))
{
  SYS_InitTimer(&keApsTimer, TIMER_ONE_SHOT_MODE, (uint32_t)timeout << 10ul, callback);
  SYS_StartTimer(&keApsTimer);
}

/*************************************************************************************//**
  \brief Stop Timeout timer
******************************************************************************************/
static void keCancelTimeout(void)
{
  SYS_StopTimer(&keApsTimer);
}

/*************************************************************************************//**
  \brief Stops KE

  \param status - status of KE execution - to be passed to ZCL Security Manager
******************************************************************************************/
static void keStopKe(ZCL_SecurityStatus_t status)
{
  if (ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_SENDING_STATE == keState)
  {
    keState = ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_TERMINATE_STATE;
    return; // wait for ending of the match descriptor discovery
  }

  keCancelTimeout();
  postponedProcessing = false;
  keState = ZCL_KE_CLUSTER_IDLE_STATE;
  keNotification(status);
}

/*************************************************************************************//**
  \brief Detects wether KE is in progress or not
  \return true if KE procedure is in progress, false othewise.
******************************************************************************************/
static bool keIsRequestInProgress(void)
{
  return keState == ZCL_KE_MATCH_DESCRIPTOR_DISCOVERY_SENDING_STATE
      || keState == ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_SENDING_STATE
      || keState == ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_SENDING_STATE
      || keState == ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_SENDING_STATE;
}

/*************************************************************************************//**
  \brief Performs copying with byte order change.

  \param dst - memory pointer to copy to
  \param src - memory pointer to copy from
  \param size - number of byte to copy
******************************************************************************************/
static void keCpyReverse(uint8_t *dst, uint8_t *src, uint8_t size)
{
  src += size - 1;
  while (size--)
  {
    *dst++ = *src--;
  }
}

/*************************************************************************************//**
  \brief Finds KE Certificate descriptor for the device with particular address

  \param subject - device's address which certificate is looking for
  \return pointer to KE Certificate descriptor if it was found, NULL otherwise
******************************************************************************************/
ZclCertificate_t *keGetCertificate(ExtAddr_t subject)
{
  uint8_t *result = NULL;

  SYS_Swap((uint8_t*)&subject, sizeof(ExtAddr_t));
  if (((ZclCertificate_t *) keRemoteCertificate)->subject == subject)
    result = keRemoteCertificate;
  else if (keCertificateDescriptor.certificate.subject == subject)
    result = keCertificateDescriptor.certificate.publicReconstrKey;
  return (ZclCertificate_t *)result;
}

/*************************************************************************************//**
  \brief Returns server or client cluster descriptor for external use.

  Use this function if you want register KE cluster on endpoint.

  \param[in] server - If true returns server descriptor, client descriptor otherwise.
  \return pointer to Cluster Descriptor.
******************************************************************************************/
ZCL_Cluster_t* keGetClusterDescriptor(uint8_t server)
{
  return (server) ? &keClusterServer : &keClusterClient;
}

#ifdef _ZSE_CERTIFICATION_
/*****************************************************************************
  \brief   Sets timeouts for KE cluster
  \param:  None
  \return: None
*****************************************************************************/
void setKETimeouts(uint8_t ephemeralTimeout, uint8_t confirmTimeout, uint8_t ephemeralDelay, uint8_t confirmDelay)
{
  ephemeralDataGenerateTime = ephemeralTimeout;
  confirmKeyGenerateTime = confirmTimeout;
  ephemeralDataGenerateDelay = ephemeralDelay;
  confirmKeyGenerateDelay = confirmDelay;
  useDelay = true;
}

void setBadCertificatesProcessing(void)
{
  passBadCertificates = true;
}

void setOutOfOrderResponse(void)
{
  outOfOrder = true;
}

/*****************************************************************************
  \brief   Delay timer has fired
  \param:  None
  \return: None
*****************************************************************************/
void delayTimerFired(void)
{
  if (ZCL_KE_CLUSTER_EPHEMERAL_DATA_COMMAND_WAITING_STATE == keState)
    keSendEphemeralDataCommand();
  if (ZCL_KE_CLUSTER_INITIATE_KE_COMMAND_WAITING_STATE == keState)
    keSendEphemeralDataCommand();
  if (ZCL_KE_CLUSTER_MAC2_CALCULATING_STATE == keState)
    keSendConfirmKeyDataCommand();
  if (ZCL_KE_CLUSTER_CONFIRM_KEY_COMMAND_WAITING_STATE == keState)
    keSendConfirmKeyDataCommand();
}

void ZCL_KeSendTooLongCertificate(int8_t addLen)
{
  sendTooLongCertificate = true;
  certAdditionLength = addLen;
}

#endif //_ZSE_CERTIFICATION_

#endif // CERTICOM_SUPPORT == 1
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
#endif // ZCL_SUPPORT == 1
// eof zclKeyEstablishmentCluster.c
