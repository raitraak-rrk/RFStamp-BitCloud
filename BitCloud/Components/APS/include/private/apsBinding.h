/**************************************************************************//**
  \file  apsBinding.h

  \brief Private interface of APS binding.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-05 Max Gekk - Created.
   Last change:
    $Id: apsBinding.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_BINDING_H
#define _APS_BINDING_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <aps.h>
#include <apsmeBind.h>
#include <apsMemoryManager.h>

#if defined _BINDING_
/******************************************************************************
                              Define(s) section
 ******************************************************************************/
#define ACTIVATE_BINDING_ENTRY(entry) \
  (entry)->confirm.status = ACTIVE_BINDING_ENTRY_STATUS

/******************************************************************************
                                Types section
 ******************************************************************************/
/** Size of the binding table. */
typedef uint8_t ApsBindingTableSize_t;
/** Type of a binding table index. */
typedef ApsBindingTableSize_t ApsBindIndex_t;

/** Type of the binding table. */
typedef struct _ApsBindingTable_t
{
  /* Total amount of binding entries. */
  ApsBindingTableSize_t  size;
  /* Array of binding entries. */
  ApsBindingEntry_t  *entry;
} ApsBindingTable_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Reset the binding table and initialize it.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsResetBindingTable(void);

/**************************************************************************//**
  \brief Allocate a place in the binding table for new entry.

  \param[in] req - pointer to request's parameters: source and destination
                   addresses, cluster identifier and etc. See APS_BindReq_t.
  \return Pointer to new binding entry or NULL.
 ******************************************************************************/
APS_PRIVATE ApsBindingEntry_t* apsAllocBindingEntry(APS_BindReq_t *const req);

/**************************************************************************//**
  \brief Get pointer to next binding entry.

  \code Example:
    ApsBindingEntry_t *entry = NULL;

    while (NULL != (entry = apsNextBind(entry, cluster, endpoint, addr)))
    {
      if (APS_GROUP_ADDRESS == entry->dstAddrMode)
        if (dstGroup == entry->dst.group)
          // send message to group
      ...
    }
  \endcode

  \param[in] entry - pointer to previous binding entry
                     or NULL if it's initial call.
  \param[in] clusterId - cluster identifier of next bind link.
  \param[in] endpoint - endpoint number of next binding entry.
  \param[in] extAddr - pointer to source IEEE address of next entry.

  \return if next binding entry is found then return valid entry pointer
          otherwise return NULL.
 ******************************************************************************/
APS_PRIVATE ApsBindingEntry_t* apsNextBind(ApsBindingEntry_t *entry,
  const ClusterId_t clusterId, const Endpoint_t endpoint,
  const ExtAddr_t *const extAddr);

/**************************************************************************//**
  \brief Find entry by bind request in the binding table.

    This function initializes the status field to APS_SUCCESS_STATUS
   if entry is found otherwise it writes APS_INVALID_PAREMETER_STATUS.

  \param[in] req - bind or unbind request's parameters.
  \parap[in] errorStatus - return this status if an entry is not found.

  \return Pointer to an entry in the binding table or NULL.
 ******************************************************************************/
APS_PRIVATE ApsBindingEntry_t* apsFindBindingEntry(APS_BindReq_t *const req,
  const APS_Status_t errorStatus);

/**************************************************************************//**
  \brief Is APS already bound or not?

    It always writes APS_SUCCESS_STATUS to the status field
   of APSME-BIND.confirm.

  \param[in] req - bind or unbind request's parameters.

  \return true is APS is not bound with given parameters otherwise false.
 ******************************************************************************/
APS_PRIVATE bool apsIsNotBound(APS_BindReq_t *const req);

/**************************************************************************//**
  \brief Copy binding information to the binding table.

  \param[in] entry - pointer to an entry in the binding table.
  \param[in] req - pointer to bind request's parameters.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsCopyBindingInfo(ApsBindingEntry_t *const entry,
  APS_BindReq_t *const req);

/**************************************************************************//**
  \brief Check binding to any group from given source endpoint.

  \param[in] clusterId - cluster identifier of group binding link
  \param[in] srcEndpoint - source endpoint of group binding link.

  \return true if group bind link is found otherwise return false.
 ******************************************************************************/
APS_PRIVATE bool apsIsBoundToAnyGroup(const ClusterId_t clusterId,
  const Endpoint_t srcEndpoint);

/**************************************************************************//**
  \brief Find bind link and try to send to bound devices.

  \param[in] apsDataReq - pointer to initial APSDE-DATA.request's parameters.
  \param[in] buffer - allocated buffer for internal transmission parameters.

  \return APS_SUCCESS_STATUS if transmission is successfully.
 ******************************************************************************/
APS_PRIVATE APS_Status_t apsProcessIndirectDataReq(APS_DataReq_t *const apsDataReq,
  ApsDataBuffer_t *const buffer);

/**************************************************************************//**
  \brief Find next bound device and try to send to it.

  \param[in] apsDataReq - pointer to initial APSDE-DATA.request's parameters.
  \param[in] buffer - allocated buffer for internal transmission parameters.

  \return APS_SUCCESS_STATUS if transmission is successfully.
 ******************************************************************************/
APS_PRIVATE APS_Status_t apsSendToNextBoundDevice(APS_DataReq_t *const apsDataReq,
  ApsDataBuffer_t *const buffer);

#endif /* _BINDING_ */
#endif /* _APS_BINDING_H */
/** eof apsBinding.h */

