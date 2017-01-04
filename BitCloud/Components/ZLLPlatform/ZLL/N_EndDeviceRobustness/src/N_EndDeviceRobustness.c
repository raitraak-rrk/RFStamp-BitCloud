/**************************************************************************//**
  \file N_EndDeviceRobustness.c

  \brief Implementation of EndDevice robustness component.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    25.01.13 D. Kolmakov - Created.
******************************************************************************/

#if defined(ZIGBEE_END_DEVICE)
/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysUtils.h>
#include <sysQueue.h>
#include <aps.h>
#include <zdo.h>

#include <N_EndDeviceRobustness_Bindings.h>
#include <N_EndDeviceRobustness.h>
#include <N_Zcl_Framework.h>
#include <N_Zdp.h>
#include <N_Connection.h>
#include <N_Connection_Private.h>
#include <N_Connection_Internal.h>
#include <N_DeviceInfo.h>
#include <N_ErrH.h>

/******************************************************************************
                   Defines section
******************************************************************************/
#define COMPID "N_EndDeviceRobustness"

#ifndef N_END_DEVICE_ROBUSTNESS_MAX_POLL_FAILURES
  #define N_END_DEVICE_ROBUSTNESS_MAX_POLL_FAILURES 2u
#endif /* N_END_DEVICE_ROBUSTNESS_MAX_POLL_FAILURES */

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum _RequestType_t
{
  N_END_DEVICE_ROBUSTNESS_ZDP_REQUEST,
  N_END_DEVICE_ROBUSTNESS_APS_DATA_REQUEST
} RequestType_t;

typedef struct _N_EndDeviceRobustness_ZdpReqInfo_t
{
  void (*ZDO_ZdpResp)(ZDO_ZdpResp_t *zdpResp);
  ZDO_ZdpReq_t *zdpReq;
} N_EndDeviceRobustness_ZdpReqInfo_t;

typedef struct _N_EndDeviceRobustness_ApsDataReqInfo_t
{
  void (*APS_DataConf)(APS_DataConf_t *conf);
  APS_DataReq_t *apsDataReq;
} N_EndDeviceRobustness_ApsDataReqInfo_t;

typedef struct _N_EndDeviceRobustness_RequestInfo_t
{
  /** Service field, used for queue support */
  QueueElement_t next;

  RequestType_t type;
  union {
    N_EndDeviceRobustness_ApsDataReqInfo_t apsDataReqInfo;
    N_EndDeviceRobustness_ZdpReqInfo_t zdpReqInfo;
  };
} N_EndDeviceRobustness_RequestInfo_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static void parentLinkFailureHandler(SYS_EventId_t eventId, SYS_EventData_t data);
static void zdpRespHandler(ZDO_ZdpResp_t* zdpResp);
static void apsDataConfHandler(APS_DataConf_t *conf);
static void waitForReconnectionTimerFired(void);
static N_EndDeviceRobustness_RequestInfo_t *getFreeReqInfoCell(void);
static N_EndDeviceRobustness_RequestInfo_t *findApsDataReqInfo(APS_DataConf_t *conf);
static N_EndDeviceRobustness_RequestInfo_t *findZdpReqInfo(ZDO_ZdpResp_t* zdpResp);
static void connectedHandler(void);
static void disconnectedHandler(void);
static void performRequest(N_EndDeviceRobustness_RequestInfo_t *reqInfo);
static void callbackHandler(N_EndDeviceRobustness_RequestInfo_t *reqInfo);
static void dropRequests(void);
static void triggerConfCallback(N_EndDeviceRobustness_RequestInfo_t *reqInfo);

/******************************************************************************
                    Static section
******************************************************************************/
static SYS_EventReceiver_t nEndDeviceRobustnessEventReceiver = {
  .func = parentLinkFailureHandler
};

static HAL_AppTimer_t waitForReconnectionTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = 2000,
  .callback = waitForReconnectionTimerFired
};

static N_EndDeviceRobustness_RequestInfo_t reqInfoBuf[N_ZDP_REQUESTS_POOL_SIZE + N_ZCL_APS_BUFFERS_AMOUNT];

/** Queue of busy request information cells. */
static DECLARE_QUEUE(busyReqInfoQueue);

/** Queue of free request information cells. */
static DECLARE_QUEUE(freeReqInfoQueue);

EndDeviceRobustnessState_t sState = N_END_DEVICE_ROBUSTNESS_IDLE;

static const N_Connection_Callback_t s_N_ConnectionCallback =
{
    connectedHandler,
    disconnectedHandler,
    NULL,
    NULL,
    NULL,
    -1
};
/* To keep track of Linkfailures and link with corresponding Confirms */
static uint8_t linkFailureEventCount = 0;
/* To check whether any request is raised after disconnection */
static bool reqRxdAfterDisconnect = false;
/* To keep track of last req */
static N_EndDeviceRobustness_RequestInfo_t *lastReqInfo = NULL;

/***********************************************************************************
                    Implementation section
***********************************************************************************/
/**************************************************************************//**
  \brief Initialization routine.
******************************************************************************/
void N_EndDeviceRobustness_Init(void)
{
  resetQueue(&busyReqInfoQueue);
  resetQueue(&freeReqInfoQueue);

  for (uint8_t i = 0; i < ARRAY_SIZE(reqInfoBuf); i++)
  {
    putQueueElem(&freeReqInfoQueue, &reqInfoBuf[i].next);
  }

  N_Connection_Subscribe(&s_N_ConnectionCallback);

  if (!N_DeviceInfo_IsFactoryNew())
  {
    SYS_SubscribeToEvent(BC_EVENT_PARENT_LINK_FAILURE, &nEndDeviceRobustnessEventReceiver);
    SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &nEndDeviceRobustnessEventReceiver);
  }
}

/**************************************************************************//**
  \brief Returns pointer to the free request information structure.

  \return pointer to the free request information structure.
******************************************************************************/
static N_EndDeviceRobustness_RequestInfo_t *getFreeReqInfoCell(void)
{
  QueueElement_t *queueElement = deleteHeadQueueElem(&freeReqInfoQueue);

  N_ERRH_ASSERT_FATAL(queueElement);
  putQueueElem(&busyReqInfoQueue, queueElement);

  return GET_STRUCT_BY_FIELD_POINTER(N_EndDeviceRobustness_RequestInfo_t,
                                     next,
                                     queueElement);
}

/**************************************************************************//**
  \brief Access iterator for busy request information structures.

  \param[in] prev - pointer to the current position.

  \return pointer to the next request information structure if exist,
          NULL otherwise.
******************************************************************************/
static N_EndDeviceRobustness_RequestInfo_t *getNextReqInfo(N_EndDeviceRobustness_RequestInfo_t *prev)
{
  QueueElement_t *queueElement;
  N_EndDeviceRobustness_RequestInfo_t *nextReqInfo;

  if (prev)
  {
    queueElement = &prev->next;
    queueElement = getNextQueueElem(queueElement);
  }
  else
    queueElement = getQueueElem(&busyReqInfoQueue);

  nextReqInfo = GET_STRUCT_BY_FIELD_POINTER(N_EndDeviceRobustness_RequestInfo_t,
                                            next,
                                            queueElement);

  return (queueElement ? nextReqInfo : NULL);
}

/**************************************************************************//**
  \brief Finds APS data request information within the busy records.

  \param[in] conf - pointer to the received confirm.

  \return pointer to the request information structure if exist,
          NULL otherwise.
******************************************************************************/
static N_EndDeviceRobustness_RequestInfo_t *findApsDataReqInfo(APS_DataConf_t *conf)
{
  APS_DataReq_t *apsDataReq = GET_STRUCT_BY_FIELD_POINTER(APS_DataReq_t,
                                                          confirm,
                                                          conf);
  N_EndDeviceRobustness_RequestInfo_t *reqInfo = getNextReqInfo(NULL);

  while (reqInfo)
  {
    if (apsDataReq == reqInfo->apsDataReqInfo.apsDataReq)
      return reqInfo;

    reqInfo = getNextReqInfo(reqInfo);
  }

  return NULL;
}

/**************************************************************************//**
  \brief Finds ZDP request information within the busy records.

  \param[in] zdpResp - pointer to the received response.

  \return pointer to the request information structure if exist,
          NULL otherwise.
******************************************************************************/
static N_EndDeviceRobustness_RequestInfo_t *findZdpReqInfo(ZDO_ZdpResp_t* zdpResp)
{
  ZDO_ZdpReq_t *zdpReq = GET_STRUCT_BY_FIELD_POINTER(ZDO_ZdpReq_t,
                                                     resp,
                                                     zdpResp);
  N_EndDeviceRobustness_RequestInfo_t *reqInfo = getNextReqInfo(NULL);

  while (reqInfo)
  {
    if (zdpReq == reqInfo->zdpReqInfo.zdpReq)
      return reqInfo;

    reqInfo = getNextReqInfo(reqInfo);
  }

  return NULL;
}

/**************************************************************************//**
  \brief Checks if the destination address is the device's own address

  \param[in] dstAddrMode - Destination address mode.
  \param[in] dstAddress -  Destination address.
******************************************************************************/
static bool checkIfOwnAddr(APS_AddrMode_t dstAddrMode, APS_Address_t* dstAddress)
{
  switch (dstAddrMode)
  {
    case APS_EXT_ADDRESS:
        return IS_EQ_EXT_ADDR(dstAddress->extAddress, *MAC_GetExtAddr());
        break;
    case APS_SHORT_ADDRESS:
        if (dstAddress->shortAddress == NWK_GetShortAddr())
          return true;
        else 
          return false;
        break;
    default:
        return false;
        break;
  }
}

/**************************************************************************//**
  \brief Performs APS data request.

  \param[in] apsDataReq - pointer to the request.
******************************************************************************/
void N_EndDeviceRobustness_DataRequest_Impl(APS_DataReq_t *apsDataReq)
{
  N_EndDeviceRobustness_RequestInfo_t *reqInfo;
  if ((N_END_DEVICE_ROBUSTNESS_CONNECTED != sState) && (N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM != sState) && \
       !checkIfOwnAddr(apsDataReq->dstAddrMode,&apsDataReq->dstAddress))
  {
    if (reqRxdAfterDisconnect)
    {
      triggerConfCallback(lastReqInfo);
      deleteQueueElem(&busyReqInfoQueue, &lastReqInfo->next);
      putQueueElem(&freeReqInfoQueue, &lastReqInfo->next);
    }
	reqRxdAfterDisconnect = true;
  }

  reqInfo = getFreeReqInfoCell();
  reqInfo->apsDataReqInfo.APS_DataConf = apsDataReq->APS_DataConf;
  reqInfo->apsDataReqInfo.apsDataReq = apsDataReq;
  reqInfo->type = N_END_DEVICE_ROBUSTNESS_APS_DATA_REQUEST;

  if (N_END_DEVICE_ROBUSTNESS_CONNECTED == sState || checkIfOwnAddr(apsDataReq->dstAddrMode,&apsDataReq->dstAddress))
    performRequest(reqInfo);
  else
  {
    lastReqInfo = reqInfo;
    N_Connection_Reconnect();
    HAL_StopAppTimer(&waitForReconnectionTimer);
    HAL_StartAppTimer(&waitForReconnectionTimer);
  }
}

/**************************************************************************//**
  \brief Performs ZDP request.

  \param[in] apsDataReq - pointer to the request.
******************************************************************************/
void N_EndDeviceRobustness_ZdpRequest_Impl(ZDO_ZdpReq_t *zdpReq)
{
  N_EndDeviceRobustness_RequestInfo_t *reqInfo;
  if ((N_END_DEVICE_ROBUSTNESS_CONNECTED != sState) && (N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM != sState) && \
        !checkIfOwnAddr(zdpReq->dstAddrMode,&zdpReq->dstAddress))
  {
    if (reqRxdAfterDisconnect)
    {
      triggerConfCallback(lastReqInfo);
      deleteQueueElem(&busyReqInfoQueue, &lastReqInfo->next);
      putQueueElem(&freeReqInfoQueue, &lastReqInfo->next);
    }
    reqRxdAfterDisconnect = true;
  }

  reqInfo = getFreeReqInfoCell();
  reqInfo->zdpReqInfo.ZDO_ZdpResp = zdpReq->ZDO_ZdpResp;
  reqInfo->zdpReqInfo.zdpReq = zdpReq;
  reqInfo->type = N_END_DEVICE_ROBUSTNESS_ZDP_REQUEST;

  if (N_END_DEVICE_ROBUSTNESS_CONNECTED == sState || checkIfOwnAddr(zdpReq->dstAddrMode,&zdpReq->dstAddress))
    performRequest(reqInfo);
  else
  {
    lastReqInfo = reqInfo;
    N_Connection_Reconnect();
    HAL_StopAppTimer(&waitForReconnectionTimer);
    HAL_StartAppTimer(&waitForReconnectionTimer);
  }
}

/**************************************************************************//**
  \brief Performs request with accordance to the specified information.
******************************************************************************/
static void performRequest(N_EndDeviceRobustness_RequestInfo_t *reqInfo)
{
  if (N_END_DEVICE_ROBUSTNESS_APS_DATA_REQUEST == reqInfo->type)
  {
    reqInfo->apsDataReqInfo.apsDataReq->APS_DataConf = apsDataConfHandler;
    reqInfo->apsDataReqInfo.apsDataReq->txOptions.indicateBroadcasts = 1;
    APS_DataReq(reqInfo->apsDataReqInfo.apsDataReq);
  }
  else if (N_END_DEVICE_ROBUSTNESS_ZDP_REQUEST == reqInfo->type)
  {
    reqInfo->zdpReqInfo.zdpReq->ZDO_ZdpResp = zdpRespHandler;
    ZDO_ZdpReq(reqInfo->zdpReqInfo.zdpReq);
  }
  else
    N_ERRH_FATAL();
}

/**************************************************************************//**
  \brief Callback for ZDP request.

  \param[in] zdpResp - ZDP response.
******************************************************************************/
static void zdpRespHandler(ZDO_ZdpResp_t* zdpResp)
{
  /* If we are not in N_END_DEVICE_ROBUSTNESS_CONNECTED state and confirmation status
     is success it means that we received confirmed for previously sent frame.
     Confirmation about frame that caused a failure will be received later. */
  if ((N_END_DEVICE_ROBUSTNESS_CONNECTED == sState) || (APS_SUCCESS_STATUS == zdpResp->respPayload.status))
  {
    N_EndDeviceRobustness_RequestInfo_t *reqInfo = findZdpReqInfo(zdpResp);
    if(N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM == sState)
    {
      linkFailureEventCount--;

      if(!linkFailureEventCount)
        sState = N_END_DEVICE_ROBUSTNESS_CONNECTED;
    }
    N_ERRH_ASSERT_FATAL(reqInfo);
    callbackHandler(reqInfo);
  }
  else if (N_END_DEVICE_ROBUSTNESS_TX_FAILURE != sState)
  {
    if(N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM == sState)
      linkFailureEventCount = 0;

    sState = N_END_DEVICE_ROBUSTNESS_TX_FAILURE;
    HAL_StopAppTimer(&waitForReconnectionTimer);
    HAL_StartAppTimer(&waitForReconnectionTimer);

    N_Connection_Disconnected();
  }
}

/**************************************************************************//**
  \brief Callback for APS data request.

  \param[in] conf - APS data confirm.
******************************************************************************/
static void apsDataConfHandler(APS_DataConf_t *conf)
{
  /* If we are not in N_END_DEVICE_ROBUSTNESS_CONNECTED state and confirmation status
     is success it means that we received confirmed for previously sent frame.
     Confirmation about frame that caused a failure will be received later. */
  if ((N_END_DEVICE_ROBUSTNESS_CONNECTED == sState) || (APS_SUCCESS_STATUS == conf->status))
  {
    N_EndDeviceRobustness_RequestInfo_t *reqInfo = findApsDataReqInfo(conf);

    if(N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM == sState)
    {
      linkFailureEventCount--;

      if(!linkFailureEventCount)
        sState = N_END_DEVICE_ROBUSTNESS_CONNECTED;
    }
    N_ERRH_ASSERT_FATAL(reqInfo);
    callbackHandler(reqInfo);
  }
  else if (N_END_DEVICE_ROBUSTNESS_TX_FAILURE != sState)
  {
    if(N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM == sState)
      linkFailureEventCount = 0;

    sState = N_END_DEVICE_ROBUSTNESS_TX_FAILURE;
    HAL_StopAppTimer(&waitForReconnectionTimer);
    HAL_StartAppTimer(&waitForReconnectionTimer);

    N_Connection_Disconnected();
  }
}

/**************************************************************************//**
  \brief Calls callback to the upper layer with accordance to the
         specified information.
******************************************************************************/
static void callbackHandler(N_EndDeviceRobustness_RequestInfo_t *reqInfo)
{
  if (N_END_DEVICE_ROBUSTNESS_APS_DATA_REQUEST == reqInfo->type)
  {
    reqInfo->apsDataReqInfo.APS_DataConf(&reqInfo->apsDataReqInfo.apsDataReq->confirm);
    reqInfo->apsDataReqInfo.APS_DataConf = NULL;

    deleteQueueElem(&busyReqInfoQueue, &reqInfo->next);
    putQueueElem(&freeReqInfoQueue, &reqInfo->next);
  }
  else if (N_END_DEVICE_ROBUSTNESS_ZDP_REQUEST == reqInfo->type)
  {
    reqInfo->zdpReqInfo.ZDO_ZdpResp(&reqInfo->zdpReqInfo.zdpReq->resp);

    if( (MATCH_DESCRIPTOR_CLID != reqInfo->zdpReqInfo.zdpReq->reqCluster ) ||
      ( (MATCH_DESCRIPTOR_CLID == reqInfo->zdpReqInfo.zdpReq->reqCluster ) &&
        (ZDO_CMD_COMPLETED_STATUS == reqInfo->zdpReqInfo.zdpReq->resp.respPayload.status)) )
    {
      reqInfo->zdpReqInfo.ZDO_ZdpResp = NULL;
      deleteQueueElem(&busyReqInfoQueue, &reqInfo->next);
      putQueueElem(&freeReqInfoQueue, &reqInfo->next);
    }
  }
  else
    N_ERRH_FATAL();
}

/**************************************************************************//**
  \brief EndDevice Robustness event receiver.

  \param[in] eventId - event identifier.
  \param[in] data - event data.
******************************************************************************/
static void parentLinkFailureHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  if (BC_EVENT_ACCESS_REQUEST == eventId)
  {
    BcAccessReq_t *const accessReq = (BcAccessReq_t*)data;

    if (BC_POLL_FAILED_ACTION == accessReq->action)
    {
      const uint8_t* const syncFailCounter = (uint8_t*)accessReq->context;

      if (N_END_DEVICE_ROBUSTNESS_MAX_POLL_FAILURES < *syncFailCounter)
      {
        accessReq->denied = true;
        sState = N_END_DEVICE_ROBUSTNESS_POLL_FAILURE;
        N_Connection_Disconnected();
      }
    }
  }
  else if (BC_EVENT_PARENT_LINK_FAILURE == eventId)
  {
    if ((N_END_DEVICE_ROBUSTNESS_CONNECTED == sState ||
         N_END_DEVICE_ROBUSTNESS_POLL_FAILURE == sState) &&
        busyReqInfoQueue.head)
      {
         sState = N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM;
         linkFailureEventCount++;
       }
  }
}

/**************************************************************************//**
  \brief EndDevice Robustness timer callback.
******************************************************************************/
static void waitForReconnectionTimerFired(void)
{
  if((N_END_DEVICE_ROBUSTNESS_CONNECTED != sState) && (N_END_DEVICE_ROBUSTNESS_WAIT_FOR_CONFIRM != sState))
  {
    reqRxdAfterDisconnect = false;
    dropRequests();
  }
}

/**************************************************************************//**
  \brief drop the requests in the queue
******************************************************************************/
static void dropRequests(void)
{
  N_EndDeviceRobustness_RequestInfo_t *reqInfo;
  while (NULL != (reqInfo = getNextReqInfo(NULL)))
  {
    triggerConfCallback(reqInfo);
    deleteQueueElem(&busyReqInfoQueue, &reqInfo->next);
    putQueueElem(&freeReqInfoQueue, &reqInfo->next);
  }
}

/**************************************************************************//**
  \brief trigger aps/zdp confirmation callback to upper layer
******************************************************************************/
static void triggerConfCallback(N_EndDeviceRobustness_RequestInfo_t *reqInfo)
{
  if (N_END_DEVICE_ROBUSTNESS_APS_DATA_REQUEST == reqInfo->type)
  {
    reqInfo->apsDataReqInfo.apsDataReq->confirm.status = ZDO_FAIL_STATUS;
    reqInfo->apsDataReqInfo.APS_DataConf(&reqInfo->apsDataReqInfo.apsDataReq->confirm);
    reqInfo->apsDataReqInfo.APS_DataConf = NULL;
    APS_ApsDataReqDeleteQueueElem(reqInfo->apsDataReqInfo.apsDataReq);
  }
  else if (N_END_DEVICE_ROBUSTNESS_ZDP_REQUEST == reqInfo->type)
  {
    reqInfo->zdpReqInfo.zdpReq->resp.respPayload.status = ZDO_FAIL_STATUS;
    reqInfo->zdpReqInfo.ZDO_ZdpResp(&reqInfo->zdpReqInfo.zdpReq->resp);
    reqInfo->zdpReqInfo.ZDO_ZdpResp = NULL;
    ZDO_ZdpReqDeleteQueueElem(reqInfo->zdpReqInfo.zdpReq);
  }
  else
    N_ERRH_FATAL();
}

/**************************************************************************//**
  \brief EndDevice Robustness on parent found callback.
******************************************************************************/
static void connectedHandler(void)
{
  reqRxdAfterDisconnect = false;

  if (N_END_DEVICE_ROBUSTNESS_CONNECTED != sState)
  {
    sState = N_END_DEVICE_ROBUSTNESS_CONNECTED;
    HAL_StopAppTimer(&waitForReconnectionTimer);

    N_EndDeviceRobustness_RequestInfo_t *reqInfo = getNextReqInfo(NULL);

    while (NULL != reqInfo)
    {
      performRequest(reqInfo);
      reqInfo = getNextReqInfo(reqInfo);
    }
  }

  SYS_SubscribeToEvent(BC_EVENT_PARENT_LINK_FAILURE, &nEndDeviceRobustnessEventReceiver);
  SYS_SubscribeToEvent(BC_EVENT_ACCESS_REQUEST, &nEndDeviceRobustnessEventReceiver);
}

/**************************************************************************//**
  \brief EndDevice Robustness on parent lost callback.
******************************************************************************/
static void disconnectedHandler(void)
{
  sState = N_END_DEVICE_ROBUSTNESS_DISCONNECTED;
  SYS_UnsubscribeFromEvent(BC_EVENT_PARENT_LINK_FAILURE, &nEndDeviceRobustnessEventReceiver);
  SYS_UnsubscribeFromEvent(BC_EVENT_ACCESS_REQUEST, &nEndDeviceRobustnessEventReceiver);
}

EndDeviceRobustnessState_t N_EndDeviceRobustness_GetState(void)
{
  return sState;
}

#endif /* ZIGBEE_END_DEVICE */
// eof N_EndDeviceRobustness.c
