/**************************************************************************//**
  \file apsmeBind.h

  \brief Interface of APS binding.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-07 Max Gekk - Created.
   Last change:
    $Id: apsmeBind.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
/**//**
 *
 *  The APS binding table allows ZigBee devices to establish designated
 * destination for frames from a given source endpoint and with a given
 * cluster ID. See ZigBee spec. r18, 2.2.8.2, page 65.
 *
 **/
#if !defined _APSME_BIND_H
#define _APSME_BIND_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <appFramework.h>
#include <apsCommon.h>

#if defined _BINDING_
/******************************************************************************
                              Define(s) section
 ******************************************************************************/
#define ACTIVE_BINDING_ENTRY_STATUS APS_INVALID_BINDING_STATUS
#define IS_ACTIVE_BINDING_ENTRY(entry) \
  (ACTIVE_BINDING_ENTRY_STATUS == (entry)->confirm.status)

/******************************************************************************
                                Types section
 ******************************************************************************/
/**//**
 * \union APS_DstBindAddr_t apsmeBind.h "aps.h"
 *
 * \brief Information about the destination address of a binding entry.
 *
 * \note A value of APS_DstBindAddr_t type should be in little-endian format.
 **/
typedef union
{
  GroupAddr_t  group;
  struct
  {
    ExtAddr_t  extAddr;
    Endpoint_t  endpoint;
  } unicast;
} APS_DstBindAddr_t;

/**//**
 * \struct APS_BindConf_t apsmeBind.h "aps.h"
 *
 * \brief The structure is used to report the result of a binding request. Conforms to APSME-BIND.confirm primitive.
 **/
typedef struct
{
  /** The status of the binding request. See \ref BindReqStatuses "details".
   **/
  APS_Status_t  status;
} APS_BindConf_t;

/**//**
 * \struct APS_BindReq_t apsmeBind.h "aps.h"
 *
 * \brief The structure used to hold parameters for APS_BindReq()
 *
 * A pointer to an instance of this type should be passed to APS_BindReq() function. The structure conforms to the APSME-BIND.request
 * primitive defined in ZigBee specification.
 **/
typedef struct
{
  /** \ref Endian "[LE]" The source IEEE address for the binding entry. */
  ExtAddr_t  srcAddr;
  /** The source endpoint for the binding entry. Valid range: 0x01-0xFE. */
  Endpoint_t  srcEndpoint;
  /** \ref Endian "[LE]" The identifier of the cluster on the source device
   * that is to be bound to the destination device. */
  ClusterId_t  clusterId;
  /** The addressing mode for the destination address. This parameter can take
   * a value from the following list:
   * \li APS_GROUP_ADDRESS - 16-bit dst.group is used to identify the destination group
   * \li APS_EXT_ADDRESS - the destination node is identified with extended address;
   *                        dst.unicast.extAddr holds the extended address, dst.unicast.endpoint holds the endpoint
   **/
  APS_AddrMode_t  dstAddrMode;
  /** \ref Endian "[LE]" The destination address for the binding entry. */
  APS_DstBindAddr_t  dst;
  /** The field to keep the result of the request*/
  APS_BindConf_t  confirm;
} APS_BindReq_t;

/**//**
 * \struct APS_UnbindReq_t apsmeBind.h "aps.h"
 *
 * \brief A type to hold parameters for APS_UnbindReq(), a typedef of the APS_BindReq_t type.
 **/
typedef APS_BindReq_t APS_UnbindReq_t;

/**//**
 * \struct ApsBindingEntry_t apsmeBind.h "aps.h"
 *
 * \brief A type for a binding table entry
 **/
typedef APS_BindReq_t ApsBindingEntry_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Bind to a remote device or a group

  \ingroup aps_data

   This function establishes binding of two devices, or binding of a device to a group,
   by creating an entry in the lcoal binding table, if supported. The function follows the primitive defined in
   ZigBee specification r18, 2.2.4.3.1, page 34.

  \param[in] req - pointer to request's parameters: source and destination
                  addresses, cluster identifier and etc. \sa APS_DataReq_t.
  \return None.

\anchor BindReqStatuses  
\par Response statuses:
  Accessed via the APS_BindConf_t::status field of the APS_BindReq_t::confirm argument's field \n\n
  ::APS_SUCCESS_STATUS (0x00) - the binding link has been successfully created \n
  ::APS_TABLE_FULL_STATUS (0xAE) - the binding table capacity had been exceeded \n
  ::APS_NOT_SUPPORTED_STATUS (0xAA) - the device does not support the binding table \n
  ::APS_ILLEGAL_REQUEST_STATUS (0xA3) - the device is not joined to
   a network, or one of the parameters has a value that is out of range
 ******************************************************************************/
void APS_BindReq(APS_BindReq_t *const req);

/**************************************************************************//**
\brief Unbind device from a group or from other device.

\ingroup aps_data

This function performs unbinding of two devices, or unbinding of a device from a group, by removing an entry in
the local binding table, if supported. The function follows the primitive defined in ZigBee specification r18, 2.2.4.3.3, page 38.

\param[in] req - pointer to request's parameters.

\return None.

\par Response statuses:
  Accessed via the APS_UnbindConf_t::status field of the APS_UnbindReq_t::confirm 
  argument's field \n\n
  ::APS_SUCCESS_STATUS (0x00) - the binding link has been successfully removed \n
  ::APS_INVALID_BINDING_STATUS (0xA4) - the binding entry has not been found
 ******************************************************************************/
void APS_UnbindReq(APS_UnbindReq_t *const req);

/**************************************************************************//**
  \brief Deactivates all records in  the binding table that contain dstAddr as
         the destination address in a binding entry

  \ingroup aps_data

  \param[in] dstAddr - pointer to the target extended address

  \return None
 ******************************************************************************/
void APS_DeactivateBindRecords(const ExtAddr_t *const dstAddr);

/**************************************************************************//**
  \brief Activates all records in the binding table that contain dstAddr as
         the destination address in a binding entry

  \ingroup aps_data

  \param[in] dstAddr - pointer to the target extended address

  \return None
 ******************************************************************************/
void APS_ActivateBindRecords(const ExtAddr_t *const dstAddr);

/**************************************************************************//**
  \brief Get a next binding entry.

  \ingroup aps_data

  \code Example:
    ApsBindingEntry_t *bindingEntry = NULL;

    while (NULL != (bindingEntry = APS_NextBindingEntry(bindingEntry)))
    {
      if (0x1234 == bindingEntry->clusterId)
        ...
      ...
    }
  \endcode

  \param[in] entry - pointer to binding entry or NULL if it's initial call.

  \return if next binding entry is found then return valid pointer
          otherwise return NULL.
 ******************************************************************************/
ApsBindingEntry_t* APS_NextBindingEntry(ApsBindingEntry_t *entry);

/**************************************************************************//**
  \brief Free allocated binding entry.

  \param[in] entry - pointer to valid entry in the binding table.

  \return None.
 ******************************************************************************/
void APS_FreeBindingEntry(ApsBindingEntry_t *const entry);

#endif /* _BINDING_ */
#endif /* _APSME_BIND_H */
/** eof apsmeBind.h */

