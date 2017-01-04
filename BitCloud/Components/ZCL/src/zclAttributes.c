/************************************************************************//**
  \file zclAttributes.c

  \brief
    Implementation of state-independent attributes logic for ZCL.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    16.03.2012 - D.Loskutnikov - extracted from zcl.c
******************************************************************************/
#if ZCL_SUPPORT == 1
/******************************************************************************
                   Includes section
******************************************************************************/
#include <zcl.h>
#include <zclInt.h>
#include <sysUtils.h>
#include <zclParser.h>
#include <zclAttributes.h>
#include <zclDbg.h>
#include <sysAssert.h>

/******************************************************************************
                   Static function prototypes section
******************************************************************************/
static ZCL_Status_t zclWriteAttribute(Endpoint_t endpointId, ClusterId_t clusterId, uint8_t clusterSide, ZCL_AttributeId_t attrId,
                                      uint8_t attrType, uint8_t *attrValue, bool isRemoteWrite, bool onlyCheck);
static ZclStructureElement_t *zclObtainStructureElement(const ZclStructure_t *st, uint16_t idx);
#if APP_CLUSTERS_IN_FLASH == 1
static ZCL_Cluster_t * zclUpdateClusterImage(ZCL_ClusterPartFlash_t *flashPart, ZCL_ClusterPartRam_t *ramPart);
#endif // APP_CLUSTERS_IN_FLASH == 1

static uint64_t readUnsignedLE(const uint8_t *value, uint8_t length);
static int64_t readSignedLE(const uint8_t *value, uint8_t length);
static bool zclIsAttributeTypeUnsigned(uint8_t typeId);

ZclCommand_t * zclGetNextCommand(ZclCommand_t *command);
static bool isOnChangeReportingNeeded(const ZclAttribute_t *pAttr);
static void zclReportOnChangeIfNeeded(ZclAttribute_t *attr, ZCL_DataTypeDescriptor_t *desc);

/******************************************************************************
                   Implementation section
******************************************************************************/
#if APP_CLUSTERS_IN_FLASH == 1
static zclClusterImage_t clusterImage;
#endif // APP_CLUSTERS_IN_FLASH == 1
/*************************************************************************//**
  \brief  ZCL Data Type Unsigness get by Type Id function.
  \param  Id - ZCL Data Type Id (unsigned 8-bit integer)
  \return true if ZCL Data Type is unsigned, otherwise - false.
*****************************************************************************/
static bool zclIsAttributeTypeUnsigned(uint8_t typeId)
{
  switch (typeId)
  {
    //General data
    case ZCL_S8BIT_DATA_TYPE_ID:
    case ZCL_S16BIT_DATA_TYPE_ID:
    case ZCL_S24BIT_DATA_TYPE_ID:
    case ZCL_S32BIT_DATA_TYPE_ID:
    case ZCL_S40BIT_DATA_TYPE_ID:
    case ZCL_S48BIT_DATA_TYPE_ID:
    case ZCL_S56BIT_DATA_TYPE_ID:
    case ZCL_S64BIT_DATA_TYPE_ID:
      return false;
    default:
      return true;
  }
}

/*************************************************************************//**
  \brief Schedules attribute reporting on change if it is needed.

  \param[in] attr - pointer to attribute;
  \param[in] desc - pointer to attribute data descriptor
*****************************************************************************/
static void zclReportOnChangeIfNeeded(ZclAttribute_t *attr, ZCL_DataTypeDescriptor_t *desc)
{
  (void)desc;
  if ((attr->properties & ZCL_REPORTABLE_ATTRIBUTE) && isReportingPermitted(attr) && isOnChangeReportingNeeded(attr))
  {
    if (attr->properties & ZCL_ON_CHANGE_REPORT)
      return;

    attr->properties |= ZCL_ON_CHANGE_REPORT;
    zclStartReportTimer();
  }
}

/*************************************************************************//**
  \brief Finds attribute in ZCL and rewrites its value by the new one.
  Triggers attribute reporting on change.

  \param[in] endpointId - endpoint on which cluster is registered.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - determs the cluster's side (client or server).
  \param[in] attrId - attribute unique identifier.
  \param[in] attrType - type of the attribute to be set.
  \param[in] attrValue - pointer to attribute value to be set.
  \param[in] isRemoteWrite - flag to ignore attempt of remote write
                             to read-only attribute
  \param[in] onlyCheck - not to write, but only check attribute existance.

  \return status of operation.
*****************************************************************************/
static ZCL_Status_t zclWriteAttribute(Endpoint_t endpointId,
                                      ClusterId_t clusterId,
                                      uint8_t clusterSide,
                                      ZCL_AttributeId_t attrId,
                                      uint8_t attrType,
                                      uint8_t *attrValue,
                                      bool isRemoteWrite,
                                      bool onlyCheck)
{
  ZclAttribute_t *zclAttr;
  ZCLActionWriteAttrReq_t  zclWriteAttrReqInfo = 
  {
    .clusterId = clusterId,
    .clusterSide = clusterSide,
    .attrId = attrId,
    .attrType = attrType,
    .attrValue = attrValue
  };
  BcZCLActionReq_t zclActionReq =
  {
    .action = ZCL_ACTION_WRITE_ATTR_REQUEST,
    .context = &zclWriteAttrReqInfo,
    .denied = 1U
  };

  if (!attrValue)
    return ZCL_INVALID_PARAMETER_STATUS;

  zclAttr = zclGetAttribute(endpointId, clusterId, clusterSide, attrId);

  if (!zclAttr)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  if (zclAttr->type != attrType)
    return ZCL_INVALID_DATA_TYPE_STATUS;

  if (isRemoteWrite)
  {
    if (zclAttr->properties & ZCL_READONLY_ATTRIBUTE)
      return ZCL_READ_ONLY_STATUS;
  }

  if (!onlyCheck)
  {
    ZCL_DataTypeDescriptor_t desc;
    ZCL_GetDataTypeDescriptor(zclAttr->type, zclAttr->value, &desc);

    if (ZCL_DATA_TYPE_STRUCTURED_KIND == desc.kind)
    {
      // Structured data. A few more checks
      const ZclArraySetBag_t *src = (const ZclArraySetBag_t*)attrValue;
      ZclArraySetBag_t *dst = (ZclArraySetBag_t*)zclAttr->value;

      // TODO: Support structure write. Structures are read-only until then.
      if (ZCL_STRUCTURE_DATA_TYPE_ID == zclAttr->type)
        return ZCL_READ_ONLY_STATUS;

      // Collection type may not be changed
      if (src->type != dst->type)
        return ZCL_READ_ONLY_STATUS;

      // We disallow collection resize
      if (src->cnt != dst->cnt)
        return ZCL_READ_ONLY_STATUS;
    }
    if (zclAttr->properties & ZCL_BOUNDARY_CHECK)
    {
      uint8_t *minValRef = NULL;
      uint8_t *maxValRef = NULL;
      // finding out min & max value position
      uint8_t *minValPos = ((uint8_t *)zclAttr) + SLICE_SIZE(ZclAttribute_t, id, properties) + desc.length;
      if (zclAttr->properties & ZCL_REPORTABLE_ATTRIBUTE)
        // adding report parameters
        minValPos = minValPos + sizeof(ZclReportableAttributeTail_t) + 2 * desc.length + sizeof(ZCL_ReportTime_t);

      if (zclAttr->properties & ZCL_CHECK_OTHER_ATTR)
      {
        // Read the attribute referred for boundry check
        ZclAttribute_t *zclMinValAttr;
        ZclAttribute_t *zclMaxValAttr;
        ZCL_AttributeId_t minMaxAttrID;
        minMaxAttrID = LE16_TO_CPU((ZCL_AttributeId_t)minValPos[0] | (minValPos[1] << 0x08));
        zclMinValAttr = zclGetAttribute(endpointId, clusterId, clusterSide, minMaxAttrID);
        minValPos += sizeof(ZCL_AttributeId_t);
        minMaxAttrID = LE16_TO_CPU((ZCL_AttributeId_t)minValPos[0] | (minValPos[1] << 0x08));
        zclMaxValAttr = zclGetAttribute(endpointId, clusterId, clusterSide, minMaxAttrID);
        if (zclMinValAttr && zclMaxValAttr)
        {
          minValRef = (uint8_t *)zclMinValAttr->value;
          maxValRef = (uint8_t *)zclMaxValAttr->value;
        }
      }
      else
      {
        minValRef = minValPos;
        maxValRef = minValPos + desc.length;
      }
      if (minValRef && maxValRef)
      {
        if (zclIsAttributeTypeUnsigned(zclAttr->type))
        {
          uint64_t minVal = readUnsignedLE(minValRef, desc.length);
          uint64_t maxVal = readUnsignedLE(maxValRef, desc.length);
          uint64_t attrVal = readUnsignedLE(attrValue, desc.length);

          if (!((attrVal >= minVal) && (attrVal <= maxVal)))
          {
            return ZCL_INVALID_VALUE_STATUS;
          }
        }
        else
        {
          int64_t minVal = readSignedLE(minValRef, desc.length);
          int64_t maxVal = readSignedLE(maxValRef, desc.length);
          int64_t attrVal = readSignedLE(attrValue, desc.length);

          if (!((attrVal >= minVal) && (attrVal <= maxVal)))
          {
            return ZCL_INVALID_VALUE_STATUS;
          }
        }
      }
    }
    zclActionReq.denied = 0;
    SYS_PostEvent(BC_ZCL_EVENT_ACTION_REQUEST, (SYS_EventData_t)&zclActionReq);
    if(!zclActionReq.denied)
    {
      memcpy(zclAttr->value, attrValue, desc.length);
      zclReportOnChangeIfNeeded(zclAttr, &desc);
    }
    else
    {
      return ZCL_INVALID_VALUE_STATUS;
    }
  }
  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Jumps to the next attribute

\param[in] attr - current attribute
\return next attribute
******************************************************************************/
ZclAttribute_t *jumpToNextAttribute(ZclAttribute_t *attr)
{
  uint8_t *p = (uint8_t*) attr;
  // Get length of current attribute
  uint8_t attributeLength = ZCL_GetAttributeLength(attr->type, attr->value);

  // Account for additional fields if current attribute is reportable
  if (attr->properties & ZCL_REPORTABLE_ATTRIBUTE)
  {
    p +=  sizeof(ZCL_ReportTime_t)  /* report counter */
        + sizeof(ZCL_ReportTime_t)  /* min report interval */
        + sizeof(ZCL_ReportTime_t)  /* max report interval */
        + attributeLength +         /* reportable change */
        + sizeof(ZCL_ReportTime_t)  /* timeout period */
        + attributeLength;          /* last reported value */
  }
  if (attr->properties & ZCL_BOUNDARY_CHECK)
  {
    if (attr->properties & ZCL_CHECK_OTHER_ATTR)
	  p += (sizeof(ZCL_AttributeId_t) * 2);
	else
	  p += (attributeLength * 2);
  }

  // Actually jump to next attribute
  p += SLICE_SIZE(ZclAttribute_t, id, properties) + attributeLength;

  return (ZclAttribute_t *)p;
}

/*************************************************************************//**
  \brief Reads unsigned LE value submitted as array to uint64_t.

  \param[in] value - pointer to array that contains value.
  \param[in] length - array length (max == 6).

  \return value of uint64_t type.
*****************************************************************************/
static uint64_t readUnsignedLE(const uint8_t *value, uint8_t length)
{
  union {
    uint64_t value;
    uint8_t bytes[sizeof(uint64_t)];
  } arrToVal = { .value = 0};

  if (sizeof(uint64_t) < length) // Invalid length
    return 0;

  memcpy(arrToVal.bytes, value, length);

  return LE64_TO_CPU(arrToVal.value);
}

/*************************************************************************//**
  \brief Reads signed LE value submitted as array to int64_t.

  \param[in] value - pointer to array that contains value.
  \param[in] length - array length (max == 6).

  \return value of int64_t type.
*****************************************************************************/
static int64_t readSignedLE(const uint8_t *value, uint8_t length)
{
  union {
    int64_t value;
    uint8_t bytes[sizeof(uint64_t)];
  } arrToVal = { .value = 0};

  if (sizeof(uint64_t) < length) // Invalid length
    return 0;

  memcpy(arrToVal.bytes, value, length);

  // Detect sign
  if (((int8_t)*(arrToVal.bytes + length - 1)) < 0)
  {
    memset(arrToVal.bytes + length, 0xFF, sizeof(uint64_t)-length);
  }

  return LE64_TO_CPU(arrToVal.value);
}

/*************************************************************************//**
  \brief Finds attribute in ZCL and returns attribute value.

  \param[in] endpointId - endpoint on which cluster is registered.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - determs the cluster's side (client or server).
  \param[in] attrId - attribute unique identifier.
  \param[out] attrType - pointer to attribute type determined and returned.
  \param[out] attrValue - read out attribute value pointer.

  Caller is responsible for providing big enough attrValue buffer.

  \return status of operation.
*****************************************************************************/
ZCL_Status_t ZCL_ReadAttributeValue(Endpoint_t endpointId,
                                    ClusterId_t clusterId,
                                    uint8_t clusterSide,
                                    ZCL_AttributeId_t attrId,
                                    uint8_t *attrType,
                                    uint8_t *attrValue)
{
  const ZclAttribute_t *zclAttr = zclGetAttribute(endpointId, clusterId, clusterSide, attrId);
  // if attribute is found it should be copied to user memory using macros
  if (zclAttr && attrType && attrValue)
  {
    *attrType = zclAttr->type;
    SYS_BYTE_MEMCPY(attrValue, zclAttr->value, ZCL_GetAttributeLength(zclAttr->type, zclAttr->value));
    return ZCL_SUCCESS_STATUS;
  }
  return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;
}

/**************************************************************************//**
\brief Safe version of ZCL_ReadAttributeValue without caller buffer overflow

\param[in] endpointId - endpoint of attribute to be read
\param[in] clusterId - cluster of attribute to be read
\param[in] clusterSide - client or server side
\param[in] attrId - attribute identifier
\param[out] attrType - attribute type
\param[out] attrValue - buffer to read attribute value
\param[in] maxSize - maximum size of buffer
\return status of operation
******************************************************************************/
ZCL_Status_t zclReadAttributeValueSafe(Endpoint_t endpointId,
                                       ClusterId_t clusterId,
                                       uint8_t clusterSide,
                                       ZCL_AttributeId_t attrId,
                                       uint8_t *attrType,
                                       uint8_t *attrValue,
                                       uint16_t maxSize)
{
  const ZclAttribute_t *zclAttr;
  uint16_t len;

  if (!attrType || !attrValue)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  zclAttr = zclGetAttribute(endpointId, clusterId, clusterSide, attrId);

  if (!zclAttr)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  len = ZCL_GetAttributeLength(zclAttr->type, zclAttr->value);

  if (len > maxSize)
    return ZCL_INSUFFICIENT_SPACE_STATUS;

  *attrType = zclAttr->type;
  memcpy(attrValue, zclAttr->value, len);
  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************//**
  \brief Finds attribute in ZCL and rewrites its value by the new one
  (to be used with local write request).

  \param[in] endpointId - endpoint on which cluster is registered.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - determs the cluster's side (client or server).
  \param[in] attrId - attribute unique identifier.
  \param[in] attrType - type of the attribute to be set.
  \param[in] attrValue - pointer to attribute value to be set.

  \return status of operation.
*****************************************************************************/
ZCL_Status_t ZCL_WriteAttributeValue(Endpoint_t endpointId,
                                     ClusterId_t clusterId,
                                     uint8_t clusterSide,
                                     ZCL_AttributeId_t attrId,
                                     uint8_t attrType,
                                     uint8_t *attrValue)
{
  return zclWriteAttribute(endpointId, clusterId, clusterSide, attrId, attrType, attrValue, false, false);
}


/*************************************************************************//**
 \brief Reports attribute if report on change is enabled and all
   conditions to report are met.

 \ingroup zcl_common

 \param[in] attr - pointer to attribute

 \return The status of the operation: \n
    ::ZCL_SUCCESS_STATUS (0x00) - new value has been written successfully \n
    ::ZCL_UNSUPPORTED_ATTRIBUTE_STATUS (0x86) - the specified attribute is not supported
*****************************************************************************/
ZCL_Status_t ZCL_ReportOnChangeIfNeeded(void *attr)
{
  ZCL_DataTypeDescriptor_t desc;

  if (!attr)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  ZCL_GetDataTypeDescriptor(((ZclAttribute_t *)attr)->type, ((ZclAttribute_t *)attr)->value, &desc);
  zclReportOnChangeIfNeeded(attr, &desc);

  return ZCL_SUCCESS_STATUS;
}


/*************************************************************************//**
  \brief Finds attribute in ZCL and rewrites its value by the new one
  (to be used with remote write request).

  \param[in] endpointId - endpoint on which cluster is registered.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - determs the cluster's side (client or server).
  \param[in] attrId - attribute unique identifier.
  \param[in] attrType - type of the attribute to be set.
  \param[in] attrValue - pointer to attribute value to be set.
  \param[in] onlyCheck - not to write, but only check attribute existance.

  \return status of operation.
*****************************************************************************/
ZCL_Status_t zclRemoteWriteAttributeValue(Endpoint_t endpointId,
                                          ClusterId_t clusterId,
                                          uint8_t clusterSide,
                                          ZCL_AttributeId_t attrId,
                                          uint8_t attrType,
                                          uint8_t *attrValue,
                                          bool onlyCheck)
{
  return zclWriteAttribute(endpointId, clusterId, clusterSide, attrId, attrType, attrValue, true, onlyCheck);
}


/*************************************************************************//**
  \brief Finds attribute in ZCL and rewrites its reportable change value by the new one.

  \param[in] endpointId - endpoint on which cluster is registered.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - determs the cluster's side (client or server).
  \param[in] attrId - attribute unique identifier.
  \param[in] attrType - type of the attribute to be set.
  \param[in] repChange - pointer to reportable change value to be set.

  \return status of operation.
*****************************************************************************/
ZCL_Status_t ZCL_SetReportableChange(Endpoint_t endpointId,
                                     ClusterId_t clusterId,
                                     uint8_t clusterSide,
                                     ZCL_AttributeId_t attrId,
                                     uint8_t attrType,
                                     uint8_t *repChange)
{
  ZclAttribute_t *zclAttr;

  if (!repChange)
    return ZCL_INVALID_PARAMETER_STATUS;

  zclAttr = zclGetAttribute(endpointId, clusterId, clusterSide, attrId);

  if (!zclAttr)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  if (zclAttr->type != attrType)
    return ZCL_INVALID_DATA_TYPE_STATUS;

  if (!(zclAttr->properties & ZCL_REPORTABLE_ATTRIBUTE))
    return ZCL_INVALID_PARAMETER_STATUS;

  uint8_t attrLength = ZCL_GetAttributeLength(zclAttr->type, (uint8_t *)zclAttr->value);
  uint8_t *reportableChange = ((uint8_t *)zclAttr) +
                      /* attribute id */ATTRIBUTE_ID_SIZE +
                      /* attribute type */ATTRIBUTE_TYPE_SIZE +
                      /* attribute properties bitmask */sizeof(uint8_t) +
                      /* attribute value */attrLength +
                      /* report counter */ sizeof(ZCL_ReportTime_t) +
                      /* min report interval */ sizeof(ZCL_ReportTime_t) +
                      /* max report interval */ sizeof(ZCL_ReportTime_t);

  memcpy(reportableChange, repChange, attrLength);

  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
  \brief Get next registered ZCL endpoint descriptor by previous.

  \param[in] prev - previous ZCL endpoint descriptor pointer.It is must be non NULL
                    sequential access required if. And it is must be NULL the
                    first descriptor as registered endpoints queue head element
                    access required if.

  \return Valid ZCL endpoint descriptor pointer descriptor found if,
           NULL - other case.
 ******************************************************************************/
ZCL_DeviceEndpoint_t* zclNextEndpoint(ZCL_DeviceEndpoint_t *prev)
{
  APS_RegisterEndpointReq_t *iter = prev? &prev->service.apsEndpoint: NULL;

  while (NULL != (iter = APS_NextEndpoint(iter)))
  {
    if (iter->APS_DataInd == zclDataInd)
      return GET_PARENT_BY_FIELD(ZCL_DeviceEndpoint_t, service.apsEndpoint, iter);
  }

  return NULL;
}

/*************************************************************************//**
  \brief Find endpoint descriptor by endpoint id.

  \param[in] endpointId - endpoint unique identifier.
  \return endpoint descriptor if found, NULL otherwise..
*****************************************************************************/
ZCL_DeviceEndpoint_t* zclGetEndpoint(Endpoint_t endpointId)
{
  ZCL_DeviceEndpoint_t *iter = NULL;

  while (NULL != (iter = zclNextEndpoint(iter)))
  {
    if (iter->service.apsEndpoint.simpleDescriptor->endpoint == endpointId)
      return iter;
  }

  return NULL;
}

/*************************************************************************//**
  \brief Find cluster descriptor by endpoint id, cluster id.

  \param[in] endpointId - endpoint unique identifier.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - cluster side (client or server).
  \return ZCL cluster descriptor if found, NULL otherwise..
*****************************************************************************/
ZCL_Cluster_t *ZCL_GetCluster(Endpoint_t endpointId, ClusterId_t clusterId, uint8_t clusterSide)
{
  ZCL_DeviceEndpoint_t *endpoint = NULL;
  ZCL_Cluster_t *cluster = NULL;
  uint8_t clusterCounter = 0;

  endpoint = zclGetEndpoint(endpointId);
  if (!endpoint)
    return NULL;

  if (ZCL_CLUSTER_SIDE_CLIENT == clusterSide)
    clusterCounter = endpoint->simpleDescriptor.AppOutClustersCount;
  else if (ZCL_CLUSTER_SIDE_SERVER == clusterSide)
    clusterCounter = endpoint->simpleDescriptor.AppInClustersCount;
  else
  {
    SYS_E_ASSERT_WARN(false,  ZCLGETCLUSTER_0);
    return NULL;
  }
  if (clusterCounter)
    cluster = ZCL_GetHeadCluster(endpoint, clusterSide);
  while (clusterCounter--)
  {
    if (cluster->id == clusterId)
      return cluster;
    if (clusterCounter)
      cluster = ZCL_GetNextCluster(cluster);
  }
  return NULL;
}

/*************************************************************************//**
  \brief get the first ZCL cluster descriptor by endpoint and cluster side.

  \param[in] endpoint - pointer to endpoint.
  \param[in] clusterSide - cluster side (client or server).
  \return pointer to the first ZCL cluster descriptor.
*****************************************************************************/
ZCL_Cluster_t * ZCL_GetHeadCluster(ZCL_DeviceEndpoint_t *endpoint, uint8_t clusterSide)
{
  ZCL_Cluster_t *cluster = NULL;
#if APP_CLUSTERS_IN_FLASH == 1
  ZCL_ClusterPartFlash_t *partFlash = NULL;
  ZCL_ClusterPartRam_t *partRam = NULL;
#endif // APP_CLUSTERS_IN_FLASH == 1

  if (ZCL_CLUSTER_SIDE_CLIENT == clusterSide)
  {
    cluster = endpoint->clientCluster;
#if APP_CLUSTERS_IN_FLASH == 1
    partFlash = endpoint->clientClusterPartFlash;
    partRam = endpoint->clientClusterPartRam;
#endif // APP_CLUSTERS_IN_FLASH == 1
  }
  else if (ZCL_CLUSTER_SIDE_SERVER == clusterSide)
  {
    cluster = endpoint->serverCluster;
#if APP_CLUSTERS_IN_FLASH == 1
    partFlash = endpoint->serverClusterPartFlash;
    partRam = endpoint->serverClusterPartRam;
#endif // APP_CLUSTERS_IN_FLASH == 1
  }
#if APP_CLUSTERS_IN_FLASH == 1
  if (!cluster)
    cluster = zclUpdateClusterImage(partFlash, partRam);
#endif // APP_CLUSTERS_IN_FLASH == 1
  return cluster;
}
/*************************************************************************//**
  \brief Finds next cluster descriptor.

  \param[in] cluster - pointer to current cluster descriptor.

  \return poiter to next cluster descriptor.
*****************************************************************************/
ZCL_Cluster_t * ZCL_GetNextCluster(ZCL_Cluster_t *cluster)
{
  //  TODO: solve situation: if castumer difened two list of cluster, 3 clusters in ram memory and 2 clusters in separate memory , then on 4 iteration we go to trash.
#if APP_CLUSTERS_IN_FLASH == 1
  if((ZCL_Cluster_t *)&clusterImage != cluster)
#endif // APP_CLUSTERS_IN_FLASH == 1
    cluster++;
#if APP_CLUSTERS_IN_FLASH == 1
  else
  {
    ZCL_ClusterPartFlash_t *flashPart = clusterImage.partFlashAddr + 1;
    ZCL_ClusterPartRam_t *ramPart = clusterImage.partRamAddr + 1;
    cluster = zclUpdateClusterImage(flashPart, ramPart);
  }
#endif // APP_CLUSTERS_IN_FLASH == 1
  return cluster;
}
#if APP_CLUSTERS_IN_FLASH == 1
/*************************************************************************//**
  \brief Update cluster descriptor image.

  \param[in] flashPart - pointer to immutable part of separate cluster.
  \param[in] ramPart - pointer to mutable part of separete cluster.
  \return pointer to recovered cluster descriptor.
*****************************************************************************/
static ZCL_Cluster_t * zclUpdateClusterImage(ZCL_ClusterPartFlash_t *flashPart, ZCL_ClusterPartRam_t *ramPart)
{
  //Synchronizes the image of a cluster with a mutable part
  if (clusterImage.needSynchronize)
  {
    memcpy((void *)clusterImage.partRamAddr, &clusterImage.partRam, sizeof(ZCL_ClusterPartRam_t));
    clusterImage.needSynchronize = NULL;
  }

  // update immutable part image
  clusterImage.partFlashAddr = flashPart;
  if (clusterImage.partFlashAddr)
  {
    // copy all immutable fields without commands, because *uint8_t and __farflash *uint8_t can have different size.
    memcpy_P((void *)&clusterImage.partFlash, clusterImage.partFlashAddr, sizeof(ZCL_ClusterPartFlash_t) - sizeof(FLASH_PTR ZclCommand_t*));
    // copy commands address
    memcpy_P((void *)&clusterImage.commandsAddr, &(clusterImage.partFlashAddr->commands), sizeof(FLASH_PTR ZclCommand_t*));
  }
  else
    clusterImage.commandsAddr = NULL;

  // update commands image
  if (clusterImage.commandsAddr)
  {
    memcpy_P((void *)&clusterImage.commandImage, clusterImage.commandsAddr, sizeof(ZclCommand_t));
    clusterImage.partFlash.commands = (uint8_t *)&clusterImage.commandImage;
  }
  else
    clusterImage.partFlash.commands = NULL;

  // update mutable part image
  clusterImage.partRamAddr = ramPart;
  if (clusterImage.partRamAddr)
  {
    memcpy((void *)&clusterImage.partRam, clusterImage.partRamAddr, sizeof(ZCL_ClusterPartRam_t));
    clusterImage.needSynchronize = 1;
  }

  return (ZCL_Cluster_t *)&clusterImage;
}
#endif // APP_CLUSTERS_IN_FLASH == 1
/*************************************************************************//**
  \brief Finds attribute descriptor by endpoint id, cluster id , cluster side
         and attribute id.

  \param[in] endpointId - endpoint unique identifier.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - cluster side (client or server).
  \param[in] attributeId - attribute unique identifier.
  \return attribute descriptor if found, NULL otherwise.
*****************************************************************************/
ZclAttribute_t * zclGetAttribute(Endpoint_t endpointId, ClusterId_t clusterId, uint8_t clusterSide, ZCL_AttributeId_t attributeId)
{
  ZCL_Cluster_t *cluster;
  ZclAttribute_t *attr;

  cluster = ZCL_GetCluster(endpointId, clusterId, clusterSide);
  if (!cluster)
    return NULL;

  attr = (ZclAttribute_t *)cluster->attributes;
  if (!attr)
    return NULL;

  // find requested attribute
  for (uint8_t i = cluster->attributesAmount; i; i--)
  {
    if (attr->id == attributeId)
      return attr;

    attr = jumpToNextAttribute(attr);
  }
  return NULL;
}

/*************************************************************************//**
  \brief Finds next attribute descriptor.

  \param[in] endpoint - endpoint number.
  \param[in] clusterId - cluster unique identifier.
  \param[in] clusterSide - cluster side (client or server).
  \param[in] currentAttribute - current attribute descriptor.
  \return next attribute descriptor if found, NULL otherwise.
*****************************************************************************/
ZclAttribute_t *zclGetNextAttribute(Endpoint_t endpoint,
                                    ClusterId_t clusterId,
                                    uint8_t clusterSide,
                                    ZclAttribute_t *currentAttribute)
{
  static uint8_t attributeNumber = 0;
  static uint8_t attributesAmount = 0;
  static Endpoint_t endpointSaved = 0;
  static ClusterId_t clusterIdSaved = 0;
  static ZclAttribute_t *result = NULL;

  ZCL_Cluster_t *cluster;

  if (NULL == currentAttribute)
  {
    // Start of interation
    attributeNumber = 0;
    attributesAmount = 0;
    endpointSaved = endpoint;
    clusterIdSaved = clusterId;
    result = NULL;
    cluster = ZCL_GetCluster(endpoint, clusterId, clusterSide);
    if (NULL != cluster)
    {
      attributesAmount = cluster->attributesAmount;
      if (0 != attributesAmount)
      {
        result = (ZclAttribute_t *) cluster->attributes;
        attributeNumber = 0;
      }
    }
  }
  else
  {
    if ((endpointSaved == endpoint) &&
        (clusterIdSaved == clusterId) &&
        (result == currentAttribute) &&
        ((attributeNumber + 1) < attributesAmount))
    {
      attributeNumber++;
      result = jumpToNextAttribute(currentAttribute);
    }
    else
    {
      result = NULL;
    }
  }
  return result;
}

/**************************************************************************//**
\brief Read indexed (structured) attribute from array

\param[in,out] selector - pointer to pointer to index selector
\param[in] endpointId - endpoint of attribute to be read
\param[in] clusterId - cluster of attribute to be read
\param[in] clusterSide - client or server side
\param[in] attrId - attribute identifier
\param[out] attrType - attribute type
\param[out] attrValue - buffer to read attribute value
\param[in] maxSize - maximum size of buffer
\return status of operation
******************************************************************************/
ZCL_Status_t zclReadIndexedAttribute(const ZCL_Selector_t *selector,
                                     Endpoint_t endpointId,
                                     ClusterId_t clusterId,
                                     uint8_t clusterSide,
                                     ZCL_AttributeId_t attrId,
                                     uint8_t *attrType,
                                     uint8_t *attrValue,
                                     uint16_t maxSize)
{
  ZclAttribute_t *attr;
  uint8_t type;
  const uint8_t *val;
  uint16_t len;

  // 2.4.15.1.3
  if (selector->indicator > 15)
    return ZCL_INVALID_SELECTOR_STATUS;

  // Obtain mother attribute (root)
  attr = zclGetAttribute(endpointId, clusterId, clusterSide, attrId);
  if (!attr)
    return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS;

  type = attr->type;
  val = attr->value;
  // Dive deeper and deeper
  for (uint8_t depth = 0; depth < selector->indicator; depth++)
  {
    uint16_t idx = selector->index[depth];
    // Only ordered sequences (array/structure) may be index-readed
    if (type != ZCL_ARRAY_DATA_TYPE_ID && type != ZCL_STRUCTURE_DATA_TYPE_ID)
        return ZCL_INVALID_SELECTOR_STATUS; // should we return ZCL_UNSUPPORTED_ATTRIBUTE_STATUS instead ?

    if (0 == idx)                             // Read of array/structure size
    {
      if (depth != selector->indicator - 1)   // Zero elementh may be only at the end of traverse
        return ZCL_INVALID_SELECTOR_STATUS;
      // Counts are placed at different offsets for array and structure
      val += (ZCL_ARRAY_DATA_TYPE_ID == type) ? offsetof(ZclArraySetBag_t, cnt) : offsetof(ZclStructure_t, cnt);
      type = ZCL_U16BIT_DATA_TYPE_ID;
      break;
    }

    idx--;                                    // Structures/arrays are indexed from 1 in ZCL

    if (ZCL_ARRAY_DATA_TYPE_ID == type)
    {
      const ZclArraySetBag_t *p = (const ZclArraySetBag_t*)val;
      if (idx >= p->cnt)
        return ZCL_INVALID_SELECTOR_STATUS; // Out of range
      type = p->type;
      val = p->elements + idx * ZCL_GetAttributeLength(p->type, p->elements);
    }
    else // ZCL_STRUCTURE_DATA_TYPE_ID == type
    {
      const ZclStructure_t *st = (const ZclStructure_t*)val;
      if (idx >= st->cnt)
        return ZCL_INVALID_SELECTOR_STATUS; // Out of range
      ZclStructureElement_t *e = zclObtainStructureElement(st, idx);
      if (!e)
        return ZCL_INVALID_SELECTOR_STATUS;     // Out of range
      type = e->type;
      val = e->value;
    }
  }

  len = ZCL_GetAttributeLength(type, val);  // Finally descended to required attribute

  if (len > maxSize)
    return ZCL_INSUFFICIENT_SPACE_STATUS;

  *attrType = type;
  memcpy(attrValue, val, len);
  return ZCL_SUCCESS_STATUS;
}

/*************************************************************************//**
  \brief Finds command descriptor by endpoint id, cluster id and command id.

  \param[in] endpointId - endpoint number.
  \param[in] clusterId - cluster unique identifier.
  \param[in] direction - command direction (client or server).
  \param[in] commandId - command unique identifier.
  \return command descriptor if found, NULL otherwise.
*****************************************************************************/
ZclCommand_t * zclGetCommand(Endpoint_t endpointId, ClusterId_t clusterId, uint8_t direction, ZCL_CommandId_t commandId)
{
  ZCL_Cluster_t *cluster = NULL;

  cluster = ZCL_GetCluster(endpointId,
                          clusterId,
                          getOwnClusterSideByIncomingCommandDirection(direction));

  return zclGetCommandByCluster(cluster, direction, commandId);
}

/*************************************************************************//**
  \brief Compares current attribute value with last reported value and if
         difference exceeds reportableChange value returns true.

  \param[in] pAttr - pointer to attribute.

  \return true if reporting is needed, false otherwise.
*****************************************************************************/
static bool isOnChangeReportingNeeded(const ZclAttribute_t *pAttr)
{
  ZCL_DataTypeDescriptor_t attrDesc;
  ZCL_GetDataTypeDescriptor(pAttr->type, (uint8_t *)&pAttr->value, &attrDesc);

  uint8_t *repAttrTail = ((uint8_t *)pAttr) + SLICE_SIZE(ZclAttribute_t, id, properties) + attrDesc.length;
  uint8_t *pLastRepValue = repAttrTail + sizeof(ZclReportableAttributeTail_t) + attrDesc.length + sizeof(ZCL_ReportTime_t);
  uint8_t *pReportableChange = repAttrTail + sizeof(ZclReportableAttributeTail_t);

  if (memcmp(pAttr->value, pLastRepValue, attrDesc.length))
  {
    if (ZCL_DATA_TYPE_STRUCTURED_KIND == attrDesc.kind) // collection data types are not reportable
      return false;

    if (attrDesc.kind == ZCL_DATA_TYPE_DISCRETE_KIND) // discrete types should be always reported on change (?)
      return true;

    if (zclIsAttributeTypeUnsigned(pAttr->type))
    {
      uint64_t currentVal = readUnsignedLE(pAttr->value, attrDesc.length);
      uint64_t lastVal = readUnsignedLE(pLastRepValue, attrDesc.length);
      uint64_t repChange = readUnsignedLE(pReportableChange, attrDesc.length);

      if ( repChange &&
          (lastVal + repChange <= currentVal ||
           lastVal >= currentVal  + repChange))
      {
        return true;
      }
    }
    else
    {
      int64_t currentVal = readSignedLE(pAttr->value, attrDesc.length);
      int64_t lastVal = readSignedLE(pLastRepValue, attrDesc.length);
      int64_t repChange = readSignedLE(pReportableChange, attrDesc.length);

      // Ignore the sign of reportable change value
      repChange = (repChange < 0) ? -repChange : repChange;

      if (repChange &&
          (lastVal + repChange <= currentVal ||
          lastVal >= currentVal  + repChange))
      {
        return true;
      }
    }
  }

  return false;
}

/*************************************************************************//**
  \brief Checks whether attribute is configured for reporting.

  \param[in] pAttr - pointer to attribute.

  \return true if reporting is permitted, false otherwise.
*****************************************************************************/
bool isReportingPermitted(const ZclAttribute_t *pAttr)
{
  uint8_t attrLength = ZCL_GetAttributeLength(pAttr->type, pAttr->value);
  uint8_t *maxReportTimePtr = (uint8_t *)pAttr + ATTRIBUTE_ID_SIZE + ATTRIBUTE_TYPE_SIZE +
    + sizeof(uint8_t) + attrLength + sizeof(ZCL_ReportTime_t) + sizeof(ZCL_ReportTime_t); // properties + value + reportCounter + minReportInterval
  ZCL_ReportTime_t maxReportTime = (ZCL_ReportTime_t)*maxReportTimePtr | (*(maxReportTimePtr + 1) << 8);

  return (0xFFFF == maxReportTime ? false : (pAttr->properties & ZCL_REPORTING_CONFIGURED));
}

/*************************************************************************//**
  \brief Finds command descriptor by cluster, direction and command id.

  \param[in] cluster   - pointer to cluster.
  \param[in] direction - command direction (client or server).
  \param[in] commandId - command unique identifier.
  \return command descriptor if found, NULL otherwise.
*****************************************************************************/
ZclCommand_t * zclGetCommandByCluster(ZCL_Cluster_t *cluster, uint8_t direction, ZCL_CommandId_t commandId)
{
  ZclCommand_t  *command;

  // Assert here ?
  if (!cluster)
    return NULL;

  command = (ZclCommand_t  *) cluster->commands;
  if (!command)
    return NULL;

  for (uint8_t i = cluster->commandsAmount; i; i--)
  {
    if (command->id == commandId && command->options.direction == direction)
      return command;
    command = zclGetNextCommand(command);
  }
  return NULL;
}
/*************************************************************************//**
  \brief Give next command descriptor.

  \param[in] command - pointer to command.
  \return none
*****************************************************************************/
ZclCommand_t * zclGetNextCommand(ZclCommand_t *command)
{
#if APP_CLUSTERS_IN_FLASH == 1
  if ((ZclCommand_t *)&clusterImage.commandImage != command)
#endif // APP_CLUSTERS_IN_FLASH == 1
    command++;
#if APP_CLUSTERS_IN_FLASH == 1
  else
    memcpy_P((void *)&clusterImage.commandImage, ++clusterImage.commandsAddr, sizeof(ZclCommand_t));
#endif // APP_CLUSTERS_IN_FLASH == 1
  return command;
}

/*************************************************************************//**
  \brief     Security Type get by Cluster Id function and by endpointId, on which
             was registered cluster.
             Returns Security Type should be used with ClusterId specified
  \param[in] clusterId - cluster identifier
  \param[in] endpointId - end point identifier
  \return    ZCL_NETWORK_KEY_CLUSTER_SECURITY - NWK Key Security should be used
             ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY - APS Link Key Security
             shoud be used.
             If cluster is unknown, security key will be determined by security mode,
             i.e. ZCL_NETWORK_KEY_CLUSTER_SECURITY for Standard Security,
                  ZCL_APPLICATION_LINK_KEY_CLUSTER_SECURITY for High Security.
*****************************************************************************/
uint8_t zclGetSecurityTypeByClusterId(ClusterId_t clusterId, Endpoint_t endpointId)
{
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  ZCL_Cluster_t *cluster = NULL;

  cluster = ZCL_GetCluster(endpointId, clusterId, ZCL_CLUSTER_SIDE_CLIENT);
  if (!cluster)
    cluster = ZCL_GetCluster(endpointId, clusterId, ZCL_CLUSTER_SIDE_SERVER);

  if (cluster)
    return cluster->options.security;

#else
  (void)clusterId;
  (void)endpointId;
#endif /* (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_) */
  return ZCL_DEFAULT_CLUSTER_SECURITY;
}


/**************************************************************************//**
\brief Obtain pointer to given element in ZCL structure

\param[in] st - structure
\param[in] idx - index of element
\return pointer to element
******************************************************************************/
static ZclStructureElement_t *zclObtainStructureElement(const ZclStructure_t *st, uint16_t idx)
{
  const uint8_t *p = (const uint8_t *)st->elements;

  // Note: index to the last element + 1 is valid and handy for calculating
  // structure size. Greater indexes are invalid.
  if (idx > st->cnt)
  {
    SYS_E_ASSERT_ERROR(false, ZCL_UNBOUNDED_READ);
    return NULL;
  }

  while (idx--)
  {
    const ZclStructureElement_t *e = (const ZclStructureElement_t *)p;
    const uint16_t len = sizeof(e->type) + ZCL_GetAttributeLength(e->type, e->value);
    p += len;
  }
  return (ZclStructureElement_t *)p;
}

/*************************************************************************//**
  \brief  ZCL Data Type Descriptor get by Type Id function.
          Fills the ZCL Data Type Descriptor dased on ZCL Data Type Id
  \param  Id - ZCL Data Type Id (unsigned 8-bit integer)
  \param  value - pointer to variable of typeId, NULL means maximum possible
          size.
  \param  descriptor - ZCL Data Type Descriptor being filled.
  \return None.
  \sa     ZCL_DataTypeDescriptor_t
  \sa     ZCL_GetAttributeLength()
*****************************************************************************/
void ZCL_GetDataTypeDescriptor(uint8_t typeId, const uint8_t *value, ZCL_DataTypeDescriptor_t *descriptor)
{
  uint8_t kind = ZCL_DATA_TYPE_DISCRETE_KIND;
  uint16_t length = 0;

  if (!descriptor)
    return;

  switch (typeId)
  {
    case ZCL_NO_DATA_TYPE_ID:
      break;

    //General data
    case ZCL_S8BIT_DATA_TYPE_ID:
    case ZCL_U8BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_8BIT_DATA_TYPE_ID:
    case ZCL_BOOLEAN_DATA_TYPE_ID:
    case ZCL_8BIT_BITMAP_DATA_TYPE_ID:
    case ZCL_8BIT_ENUM_DATA_TYPE_ID:
      length = 1;
      break;

    case ZCL_U16BIT_DATA_TYPE_ID:
    case ZCL_S16BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_16BIT_DATA_TYPE_ID:
    case ZCL_16BIT_BITMAP_DATA_TYPE_ID:
    case ZCL_16BIT_ENUM_DATA_TYPE_ID:
    case ZCL_CLUSTER_ID_DATA_TYPE_ID:
    case ZCL_ATTRIBUTE_ID_DATA_TYPE_ID:
      length = 2;
      break;

    case ZCL_U24BIT_DATA_TYPE_ID:
    case ZCL_S24BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_24BIT_DATA_TYPE_ID:
    case ZCL_24BIT_BITMAP_DATA_TYPE_ID:
      length = 3;
      break;

    case ZCL_U32BIT_DATA_TYPE_ID:
    case ZCL_S32BIT_DATA_TYPE_ID:
    case ZCL_UTC_TIME_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_32BIT_DATA_TYPE_ID:
    case ZCL_32BIT_BITMAP_DATA_TYPE_ID:
      length = 4;
      break;

    case ZCL_U40BIT_DATA_TYPE_ID:
    case ZCL_S40BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_40BIT_DATA_TYPE_ID:
    case ZCL_40BIT_BITMAP_DATA_TYPE_ID:
      length = 5;
      break;

    case ZCL_U48BIT_DATA_TYPE_ID:
    case ZCL_S48BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_48BIT_DATA_TYPE_ID:
    case ZCL_48BIT_BITMAP_DATA_TYPE_ID:
      length = 6;
      break;

    case ZCL_U56BIT_DATA_TYPE_ID:
    case ZCL_S56BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_56BIT_DATA_TYPE_ID:
    case ZCL_56BIT_BITMAP_DATA_TYPE_ID:
      length = 7;
      break;

    case ZCL_U64BIT_DATA_TYPE_ID:
    case ZCL_S64BIT_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_ANALOG_KIND;
    case ZCL_64BIT_DATA_TYPE_ID:
    case ZCL_64BIT_BITMAP_DATA_TYPE_ID:
    case ZCL_IEEE_ADDRESS_DATA_TYPE_ID:
      length = 8;
      break;

    case ZCL_128BIT_SECURITY_KEY_DATA_TYPE_ID:
      length = 16;
      break;
    case ZCL_OCTET_STRING_DATA_TYPE_ID:
    case ZCL_CHARACTER_STRING_DATA_TYPE_ID:
      if (NULL == value)
        length = OCTET_STRING_MAX_SIZE;
      else
        length = *value + sizeof(uint8_t);
      break;

    case ZCL_ARRAY_DATA_TYPE_ID:
    case ZCL_SET_DATA_TYPE_ID:
    case ZCL_BAG_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_STRUCTURED_KIND;
      if (value)
      {
        const ZclArraySetBag_t *p = (const ZclArraySetBag_t *)value;
        length =  SLICE_SIZE(ZclArraySetBag_t, type, cnt)
                            + p->cnt * ZCL_GetAttributeLength(p->type, p->elements);
      }
      break;

    case ZCL_STRUCTURE_DATA_TYPE_ID:
      kind = ZCL_DATA_TYPE_STRUCTURED_KIND;
      if (value)
      {
        // Calculate length via obtaining pointer to the last+1 element (structures are indexed from 1 in ZCL)
        const ZclStructure_t *st = (const ZclStructure_t *)value;
        ZclStructureElement_t *elem = zclObtainStructureElement(st, st->cnt);

        if(!elem)
          length = 0;
        else
          length = (const uint8_t *)elem - value;
      }
      break;

    default:
      SYS_E_ASSERT_WARN(false, UNKNOWN_DAT_TYPE_DESCR);
      break;
  }

  if ((0 != length) || (ZCL_NO_DATA_TYPE_ID == typeId))
  {
    descriptor->length = length;
    descriptor->kind = kind;
  }
}

/*************************************************************************//**
  \brief  ZCL Attribute Data Type length get function.
          Returns the ZCL Attribute Data Type length in bytes according to ZCL Data Type Id
  \param  Id - ZCL Attribute Data Type Id (unsigned 8-bit integer)
  \param  Value - Pointer to the value, NULL - means that the length will be
          the maximum size.
  \return ZCL Attribute Data Type length in bytes.
  \sa     ZCL_DataTypeDescriptor_t
  \sa     ZCL_GetAttributeLength()
*****************************************************************************/
uint16_t ZCL_GetAttributeLength(uint8_t typeId, const uint8_t *value)
{
  ZCL_DataTypeDescriptor_t descriptor;

  ZCL_GetDataTypeDescriptor(typeId, value, &descriptor);

  return descriptor.length;
}
#endif // ZCL_SUPPORT == 1
//eof zclAttributes.c
