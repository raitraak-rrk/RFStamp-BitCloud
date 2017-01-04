/**************************************************************************//**
  \file zdoEndDeviceBinding.h

  \brief Internal interface of ZDO End device Binding module.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-11 Max Gekk - Created.
   Last change:
    $Id: zdoEndDeviceBinding.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _ZDO_END_DEVICE_BINDING_H
#define _ZDO_END_DEVICE_BINDING_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <appTimer.h>
#include <zdo.h>

/******************************************************************************
                               Definition section
 ******************************************************************************/
/** Pre-configured timeout duration awaiting a second End_Device_Bind_req.
 * See ZigBee Smart Energy Profile Specification r15, Table 5.9 and  ZigBee
 * HA Profile Specification r26, 5.3.8, page 15. */
#define ZDO_END_DEVICE_BIND_TIMEOUT 60000UL /* milliseconds */

/** Minimum size of ZDP End Device Binding command:
 * - 2 bytes - binding target,
 * - 8 bytes - IEEE address of the device generating request,
 * - 1 byte - source endpoint,
 * - 2 bytes - profileId,
 * - 2 bytes - numInClusters and numOutClusters.
 */
#define ZDO_MIN_END_DEVICE_BINDING_COMMAND_SIZE 15U

/** Maximum number of stored input and output cluster ids. */
#define ZDO_TOTAL_BIND_CLUSTERS_AMOUNT (2 * MAX_REQUESTED_CLUSTER_NUMBER)

#if defined _END_DEVICE_BINDING_
/******************************************************************************
                                 Types section
 ******************************************************************************/
/* States of End Device Binding module. */
typedef enum _ZdoEndDeviceBindingState_t
{
  ZDO_END_DEVICE_BINDING_NO_INIT_STATE = 0x00,
  ZDO_WAIT_FIRST_END_DEVICE_BINDING_REQ_STATE = 0x45,
  ZDO_WAIT_SECOND_END_DEVICE_BINDING_REQ_STATE = 0x46,
  ZDO_SEND_FIRST_UNBIND_REQ_STATE = 0x47,
  ZDO_SEND_END_DEVICE_BINDING_NO_MATCH_STATE = 0x48,
  ZDO_SEND_END_DEVICE_BINDING_TIMEOUT_STATE = 0x49,
  ZDO_SENDING_FIRST_UNBIND_REQ_STATE = 0x4A,
  ZDO_SENDING_UNBIND_BIND_REQ_STATE = 0x4B,
  ZDO_SENDING_FIRST_END_DEVICE_BINDING_RESP_STATE = 0x4C,
  ZDO_SENDING_SECOND_END_DEVICE_BINDING_RESP_STATE = 0x4D,
  ZDO_SENDING_END_DEVICE_BINDING_TIMEOUT_STATE = 0x4E
} ZdoEndDeviceBindingState_t;

/* Information about cluster that is received in ZDO End Device Binding request. */
typedef struct _ZdoSavedClusterId_t
{
  ClusterId_t id; /* Identifier of stored cluster. */
  bool busy  : 1; /* Current entry is used (true) or not (false). */
  /* If this field is 'true' then clusterId was received in the Input Cluster List
   * of ZDP End Device Binding command frame. */
  bool isInputCluster : 1;
  /* if it's output cluster then this flag indicates bind state. */
  bool bound : 1;
} ZdoSavedClusterId_t;

/* Information from received End Device Binding request. */
typedef struct _ZdoBindDevice_t
{
  ExtAddr_t extAddress;
  ShortAddr_t shortAddress;
  ShortAddr_t bindingTarget;
  Endpoint_t endpoint;
  uint8_t seqNum; /*!< Sequence number of ZDP request. */
} ZdoBindDevice_t;

/* Internal state and variables of the End Device Binding module. */
typedef struct _ZdoEndDeviceBinding_t
{
  ZdoEndDeviceBindingState_t state;
  bool zdpReqBusy; /*!< Flag indicates using of zdpReq. */
  ZDO_ZdpReq_t zdpReq;
  HAL_AppTimer_t timer;
  /* List of clusterIds (input and output) from first ZDP request. */
  ZdoSavedClusterId_t cluster[ZDO_TOTAL_BIND_CLUSTERS_AMOUNT];
  ProfileId_t profileId;
  ClusterId_t bindClusterId;
  ZDO_Status_t responseStatus; /*!< Status used in finish stage. */
  ZdoBindDevice_t first;  /*!< Information about a first binding device. */
  ZdoBindDevice_t second; /*!< Information about a second binding device. */
} ZdoEndDeviceBinding_t;

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Prepare APS_DataReq_t structure for end device binding request.

  \param[in] zdpReq - pointer to ZDP request's structure.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoEndDeviceBindingRequestCreate(ZDO_ZdpReq_t *const zdpReq);

#if defined _COORDINATOR_
/**************************************************************************//**
  \brief Process an incoming ZDP End Device Binding command.
  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of 
                          End Device Binding request command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoEndDeviceBindingInd(const APS_DataInd_t *const apsDataInd);

/**************************************************************************//**
  \brief Prepare APS_DataReq_t structure for end device binding response.

  \param[in] zdpReq - the pointer to ZDP request's structure for the End 
                      Device Binding response.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoEndDeviceBindingResponseCreate(ZDO_ZdpReq_t *const zdpReq);

/**************************************************************************//**
  \brief Reset internal state and variables of ZDO End Device Binding modile.
 ******************************************************************************/
ZDO_PRIVATE void zdoResetEndDeviceBinding(void);

#endif /* _COORDINATOR_ */
#endif /* _END_DEVICE_BINDING_ */
#endif /* _ZDO_END_DEVICE_BINDING_H */
/** eof zdoEndDeviceBinding.h */

