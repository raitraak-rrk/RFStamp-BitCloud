/***************************************************************************//**
  \file N_Binding.c

  \brief Implementation of API for the binding table.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-01-21 Max Gekk - Created.
   Last change:
    $Id: N_Binding.c 24241 2013-01-23 12:52:47Z mgekk $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysUtils.h>
#include <aps.h>

#include <N_ErrH.h>

#include <N_Binding.h>
#include <N_Binding_Bindings.h>

/******************************************************************************
                              Definitions section
 ******************************************************************************/
#define COMPID "N_Binding"

/******************************************************************************
                           Global variables section
 ******************************************************************************/
N_Address_t g_N_Binding_Address = {{0U}, N_Address_Mode_Binding, 0U, 0U};

/******************************************************************************
                      Local functions prototypes section
 ******************************************************************************/
static void initBindAddresses(APS_BindReq_t *const apsBindReq, uint8_t sourceEndpoint,
  const N_Address_t* pDestinationAddress);

/******************************************************************************
                            Implementations section
 ******************************************************************************/
/** Add an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param numClusterIds The number of ClusterIds
    \param pClusterIds Array of clusterIds
    \returns TRUE if operation succeeded, FALSE otherwise
*/
bool N_Binding_AddBindingExt_Impl(uint8_t sourceEndpoint,
  const N_Address_t* pDestinationAddress, uint8_t numClusterIds,
  uint16_t *pClusterIds)
{
  APS_BindReq_t apsBindReq;
  uint8_t i;

  initBindAddresses(&apsBindReq, sourceEndpoint, pDestinationAddress);
  N_ERRH_ASSERT_FATAL( NULL != pClusterIds );
  for (i = 0; i < numClusterIds; ++i)
  {
    apsBindReq.clusterId = pClusterIds[i];
    APS_BindReq(&apsBindReq);
    if (APS_SUCCESS_STATUS != apsBindReq.confirm.status)
      return false;
  }

  return true;
}

/** Add an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param numClusterIds The number of ClusterIds
    \param pClusterIds Array of clusterIds
*/
void N_Binding_AddBinding_Impl(uint8_t sourceEndpoint,
  const N_Address_t *pDestinationAddress, uint8_t numClusterIds,
  uint16_t *pClusterIds)
{
  N_Binding_AddBindingExt(sourceEndpoint, pDestinationAddress, numClusterIds, pClusterIds);
}

/** Remove a binding from the binding table
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
    \param clusterId the clusterId
    \returns TRUE if operation succeeded, FALSE otherwise
*/
bool N_Binding_RemoveBinding_Impl(uint8_t sourceEndpoint,
  const N_Address_t* pDestinationAddress, uint16_t clusterId)
{
  APS_UnbindReq_t apsUnbindReq;

  initBindAddresses(&apsUnbindReq, sourceEndpoint, pDestinationAddress);
  apsUnbindReq.clusterId = clusterId;

  APS_UnbindReq(&apsUnbindReq);

  return APS_SUCCESS_STATUS == apsUnbindReq.confirm.status;
}

/** Remove all bindings for an endpoint
    \param endpoint Endpoint to remove, 0xFF is all endpoints
*/
void N_Binding_RemoveBindings_Impl(uint8_t endpoint)
{
  ApsBindingEntry_t *bindingEntry = NULL;

  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
  {
    if (endpoint == bindingEntry->srcEndpoint || APS_BROADCAST_ENDPOINT == endpoint)
      APS_FreeBindingEntry(bindingEntry);
  }
}

/** find an entry to the binding table
    \param sourceEndpoint The source endpoint
    \param clusterId The concerning clusterId
    \param skip The number of bindings to skip. '0' returns the index to the first binding record
                with sourceEndpoint and clusterId, '1' the second, etc.
    \returns the index of the binding record, returns INVALID_BINDING_RECORD_INDEX if no such
             binding exist
*/
uint8_t N_Binding_FindBinding_Impl(uint8_t sourceEndpoint, uint16_t clusterId, uint8_t skip)
{
  ApsBindingEntry_t *bindingEntry = NULL;
  uint8_t index = 0U;

  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
  {
    ++index;
    if (skip)
    {
      --skip;
      continue;
    }
    if (bindingEntry->srcEndpoint != sourceEndpoint)
      continue;
    if (bindingEntry->clusterId != clusterId)
      continue;
    
    return index - 1U;
  }

  return INVALID_BINDING_RECORD_INDEX;
}

/** Get the number of binding table records
    \returns the number of binding table records
*/
uint8_t N_Binding_GetNumberOfBindingRecords_Impl(void)
{
  ApsBindingEntry_t *bindingEntry = NULL;
  uint8_t number = 0U;

  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
    ++number;

  return number;
}

/** Get the binding table records specified by the index
    \param[in] index Index of the binding record to retrieve
    \param[out] pRecord The binding table record
*/
void N_Binding_GetBindingRecord_Impl(uint8_t index, N_Binding_Record_t *pRecord)
{
  ApsBindingEntry_t *bindingEntry = NULL;

  N_ERRH_ASSERT_FATAL( NULL != pRecord );
  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
    if (!index--)
    {
      pRecord->sourceEndpoint = bindingEntry->srcEndpoint;
      pRecord->clusterId = bindingEntry->clusterId;
      pRecord->destinationAddress.panId = 0x0000U;
      if (APS_EXT_ADDRESS == bindingEntry->dstAddrMode)
      {
        pRecord->destinationAddress.addrMode = N_Address_Mode_Extended;
        memcpy(pRecord->destinationAddress.address.extendedAddress,
               &bindingEntry->dst.unicast.extAddr,
               sizeof(ExtAddr_t));
        pRecord->destinationAddress.endPoint = bindingEntry->dst.unicast.endpoint;
      }
      else
      {
        N_ERRH_ASSERT_FATAL( APS_GROUP_ADDRESS == bindingEntry->dstAddrMode );
        pRecord->destinationAddress.addrMode = N_Address_Mode_Group;
        pRecord->destinationAddress.address.shortAddress = bindingEntry->dst.group;
      }
      break;
    }
}

/** Remove a binding record
    \param index Index of the binding to remove
*/
void N_Binding_RemoveBindingRecord_Impl(uint8_t index)
{
  ApsBindingEntry_t *bindingEntry = NULL;

  while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
    if (!index--)
    {
      APS_FreeBindingEntry(bindingEntry);
      break;
    }
}

/** Fill in source and destination addresses of the Bind Request.
    \param apsBindReq - pointer to the APS-BIND.request which should be filled in.
    \param sourceEndpoint The source endpoint
    \param pDestinationAddress The destination address
*/
static void initBindAddresses(APS_BindReq_t *const apsBindReq, uint8_t sourceEndpoint,
  const N_Address_t* pDestinationAddress)
{
  N_ERRH_ASSERT_FATAL( NULL != apsBindReq );
  N_ERRH_ASSERT_FATAL( NULL != pDestinationAddress );

  COPY_EXT_ADDR(apsBindReq->srcAddr, *MAC_GetExtAddr());
  apsBindReq->srcEndpoint = sourceEndpoint;
  if (N_Address_Mode_Extended == pDestinationAddress->addrMode)
  {
    apsBindReq->dstAddrMode = APS_EXT_ADDRESS;
    memcpy(&apsBindReq->dst.unicast.extAddr,
            pDestinationAddress->address.extendedAddress,
            sizeof(apsBindReq->dst.unicast.extAddr));            
    apsBindReq->dst.unicast.endpoint = pDestinationAddress->endPoint;
  }
  else
  {
    N_ERRH_ASSERT_FATAL( N_Address_Mode_Group == pDestinationAddress->addrMode );
    apsBindReq->dstAddrMode = APS_GROUP_ADDRESS;
    apsBindReq->dst.group = pDestinationAddress->address.shortAddress;            
  }
}

/** Initializes the component.*/
void N_Binding_Init(void) {}

/** eof N_Binding.c */
