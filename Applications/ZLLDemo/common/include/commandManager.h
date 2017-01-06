/**************************************************************************//**
  \file commandManager.h

  \brief
    Interface to provide ZCL commands management for ZLLDemo

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    04.07.11 A. Taradov - Created.
    21.03.12 A. Razinkov - Refactored, renamed from commonClusters.h
******************************************************************************/
#ifndef _COMMANDMANAGER_H
#define _COMMANDMANAGER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl.h>
#include <zdo.h>
#include <zclZllBasicCluster.h>
#include <zclZllColorControlCluster.h>
#include <zclZllCommissioningCluster.h>
#include <zclZllGroupsCluster.h>
#include <zclZllIdentifyCluster.h>
#include <zclZllLevelControlCluster.h>
#include <zclZllOnOffCluster.h>
#include <zclZllScenesCluster.h>
#include <zclLinkInfoCluster.h>

/******************************************************************************
                    Types
******************************************************************************/
/* ZCL command descriptor */
typedef struct _CommandDescriptor_t
{
  /* Service field for descriptor queing */
  struct
  {
    QueueElement_t next;
  } service;
  /* Cluster identifier of specific command */
  uint16_t clusterId;
  /* Specific command identifier */
  uint8_t  commandId;
  /* Source endpoint */
  uint8_t srcEndpoint;
  /* Destination addressing info */
  ZCL_Addressing_t dstAddressing;
  /* Flag to disable default response for specific command */
  bool disableDefaultResp;
  /* Flag forcing ZCL to use sequence number from destination addressing
     info structure. */
  bool seqNumberSpecified;
  /* Flag for general commands generation - acting on attributes */
  bool isAttributeOperation;
  /* Notification to be called after command sending */
  void (*ZCL_Notify)(ZCL_Notify_t *ntfy);
  /* Command payload size */
  uint8_t size;
  /* Command payload */
  union
  {
    // On/Off cluster
    ZCL_OffWithEffect_t   offWithEffect;
    ZCL_OnWithTimedOff_t  onWithTimedOff;

    // Level Control cluster
    ZCL_MoveToLevel_t     moveToLevel;
    ZCL_Move_t            moveLevel;
    ZCL_Step_t            stepLevel;

    // Color Control cluster
    ZCL_ZllMoveToHueCommand_t         moveToHue;
    ZCL_ZllMoveHueCommand_t           moveHue;
    ZCL_ZllStepHueCommand_t           stepHue;
    ZCL_ZllMoveToSaturationCommand_t  moveToSaturation;
    ZCL_ZllMoveSaturationCommand_t    moveSaturation;
    ZCL_ZllStepSaturationCommand_t    stepSaturation;
    ZCL_ZllMoveToHueAndSaturationCommand_t moveToHueAndSaturation;
    ZCL_ZllMoveToColorCommand_t       moveToColor;
    ZCL_ZllMoveColorCommand_t         moveColor;
    ZCL_ZllStepColorCommand_t         stepColor;
    ZCL_ZllEnhancedMoveToHueCommand_t enhancedMoveToHue;
    ZCL_ZllEnhancedMoveHueCommand_t   enhancedMoveHue;
    ZCL_ZllEnhancedStepHueCommand_t   enhancedStepHue;
    ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t enhancedMoveToHueAndSaturation;
    ZCL_ZllColorLoopSetCommand_t      colorLoopSet;
    ZCL_ZllMoveToColorTemperatureCommand_t moveToColorTemperature;
    ZCL_ZllMoveColorTemperatureCommand_t moveColorTemperature;
    ZCL_ZllStepColorTemperatureCommand_t stepColorTemperature;

    // Groups cluster
    ZCL_AddGroup_t                   addGroup;
    ZCL_ViewGroup_t                  viewGroup;
    ZCL_GetGroupMembership_t         getGroupMembership;
    ZCL_RemoveGroup_t                removeGroup;
    ZCL_AddGroupIfIdentifying_t      addGroupIfIdentifying;
    ZCL_AddGroupResponse_t           addGroupResp;
    ZCL_ViewGroupResponse_t          viewGroupResp;
    ZCL_GetGroupMembershipResponse_t getGroupMembershipResp;
    ZCL_RemoveGroupResponse_t        removeGroupResp;

    // Scenes cluster
    ZCL_AddScene_t                   addScene;
    ZCL_ViewScene_t                  viewScene;
    ZCL_RemoveScene_t                removeScene;
    ZCL_RemoveAllScenes_t            removeAllScenes;
    ZCL_StoreScene_t                 storeScene;
    ZCL_RecallScene_t                recallScene;
    ZCL_GetSceneMembership_t         getSceneMembership;
    ZCL_EnhancedAddScene_t           enhancedAddScene;
    ZCL_EnhancedViewScene_t          enhancedViewScene;
    ZCL_CopyScene_t                  copyScene;
    ZCL_AddSceneResponse_t           addSceneResp;
    ZCL_EnhancedViewSceneResponse_t  enhancedViewSceneResp;
    ZCL_RemoveSceneResponse_t        removeSceneResp;
    ZCL_RemoveAllScenesResponse_t    removeAllScenesResp;
    ZCL_StoreSceneResponse_t         storeSceneResp;
    ZCL_GetSceneMembershipResponse_t getSceneMembershipResp;
    ZCL_CopySceneResponse_t          copySceneResp;

    // Identify cluster
    ZCL_Identify_t              identify;
    ZCL_TriggerEffect_t         triggerEffect;
    ZCL_IdentifyQueryResponse_t identifyQueryResp;

    // Commissioning cluster
    ZCL_GetGroupIdentifiers_t         getGroupIdentifiers;
    ZCL_GetEndpointList_t             getEndpointList;
    ZCL_EndpointInformation_t         endpointInformation;
    ZCL_GetGroupIdentifiersResponse_t getGroupIdentifiersResp;
    ZCL_GetEndpointListResponse_t     getEndpointListResp;

    // Link Info Cluster
    ZCL_ReadyToTransmit_t readyToTransmit;

    // Read attribute request
    ZCL_ReadAttributeReq_t    readAttribute;
    ZCL_WriteAttributeReq_t   writeAttribute;
    uint8_t writeAttributesUndivided[sizeof(ZCL_WriteAttributeReq_t) * 2 - 2 + sizeof(uint32_t) * 2];
  } payload;
} CommandDescriptor_t;

typedef struct
{
  APS_AddrMode_t mode;
  uint64_t addr;
  Endpoint_t ep;
  uint16_t manufacturerSpecCode;
  bool defaultResponse;
} DstAddressing_t;

/******************************************************************************
                    Prototypes
******************************************************************************/

/**************************************************************************//**
\brief Initialize common cluster functionality.
******************************************************************************/
void clustersInit(void);

/**************************************************************************//**
\brief Set addressing parameters for all outgoing clisters requests
\param[in] dstAddressing - pointer to addressing information structure
******************************************************************************/
void clustersSetDstAddressing(DstAddressing_t *dstAddressing);

/**************************************************************************//**
\brief Get addressing parameters for all outgoing clisters requests
\returns pointer to addressing information structure
******************************************************************************/
DstAddressing_t *clustersGetDstAddressing(void);

/**************************************************************************//**
\brief Get free ZCL command buffer.
\return Command buffer pointer or NULL if no free buffers left
******************************************************************************/
CommandDescriptor_t *clustersAllocCommand(void);

/**************************************************************************//**
\brief Get free ZCL command buffer.
\param[in] req - command to free
******************************************************************************/
void clustersFreeCommand(CommandDescriptor_t *req);

/**************************************************************************//**
\brief Send command using default addressing.
\param[in] req - request parameters
******************************************************************************/
void clustersSendCommand(CommandDescriptor_t *req);

/**************************************************************************//**
\brief Send command using provided addressing.
\param[in] req - request parameters
******************************************************************************/
void clustersSendCommandWithAddressing(CommandDescriptor_t *req, ZCL_Addressing_t *addressing);

/**************************************************************************//**
\brief Checks if there are any transmissions inprogress.

\return True, if any transmissions are inprogress; false - otherwise.
******************************************************************************/
bool transmissionIsInProgress(void);

/***************************************************************************//**
\brief Default response handler.

\param[in] resp - pointer to response parametres.
******************************************************************************/
void commandManagerDefaultResponseHandler(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
#endif // _COMMANDMANAGER_H

// eof commandManager.h
