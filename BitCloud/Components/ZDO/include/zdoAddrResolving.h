/**************************************************************************//**
  \file zdoAddrResolving.h

  \brief Interface for ZDO Address Resolving request routines

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    15.06.12 D. Kolmakov - Created
  Last change:
    $Id: zdoAddrResolving.h 28144 2015-07-09 08:58:32Z viswanadham.kotla $
******************************************************************************/
#ifndef _ZDOADDRRESOLVING_H
#define _ZDOADDRRESOLVING_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <apsCommon.h>
#include <zdoCommon.h>

/******************************************************************************
                        Types section
******************************************************************************/
typedef enum _ZdoAddrResolvingReqState_t
{
  ZDO_ADDRESS_RESOLVING_IDLE_STATE = 0x01,
  ZDO_ADDRESS_RESOLVING_IN_PROGRESS_STATE,
  ZDO_ADDRESS_RESOLVING_CONFIRM_STATE
} ZdoAddrResolvingReqState_t;

/**************************************************************************//**
\brief Describes the parameters of the ZDO_ResolveAddrConf() function
******************************************************************************/
typedef struct _ZDO_ResolveAddrConf_t
{
  ZDO_Status_t status; //!< The status of the address resolving request
} ZDO_ResolveAddrConf_t;

/**************************************************************************//**
\brief Describes the parameters of the ZDO_ResolveAddrReq() function
******************************************************************************/
typedef struct _ZDO_ResolveAddrReq_t
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    QueueElement_t next;
  } service; //!< Service field for internal use only, should not be touched by user
  /** \endcond **/
  uint8_t reqSeqNo;
  ZdoAddrResolvingReqState_t  resolveAddrState;
  APS_AddrMode_t dstAddrMode; //!< The addressing mode for identifying the destination
  APS_Address_t dstAddress; //!< Address of device for which the resolving will be executed
  ZDO_ResolveAddrConf_t confirm; //!< Parameters of the confirm being returned in ZDO_ResolveAddrConf()
  void (*ZDO_ResolveAddrConf)(ZDO_ResolveAddrConf_t *conf); //!< The function called to confirm the request
} ZDO_ResolveAddrReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Determines the missing short address corresponding to the given extended
address or the missing extended address corresponding to the given short address

\ingroup zdo

The function sends a ZDP request: either an IEEE address ZDP request if the extended
address should be determined, or a short address ZDP request if the short address
should be determined. The callback function reports the result of ZDP request
execution. If the request has completed successfully, the requested addressing
info is provided in the argument.

The function is called by the stack in several cases when one element in a pair of
short and extended addresses of the same device is unknown. These cases include
sending a data request when soome information about destination is lacking:
::APS_EXT_ADDRESS or ::APS_NO_ADDRESS (send to bound devices) addressing modes are used, 
but the short address is not known; short address addressing 
is used, and link key security is used, but the extended address to find out the
link key is not known. The stack also checks if the extended address is known
when it receives an encrypted frame.

\par Response statuses:
  Accessed via the ZDO_ResolveAddrConf_t::status field of the
  ZDO_ResolveAddrReq_t::ZDO_ResolveAddrConf callback's argument \n\n
  ::ZDO_SUCCESS_STATUS (0x00) - the requested short or extended address has been 
  successfully determined \n
  ::ZDO_INVALID_REQUEST_STATUS (0x80) - the address resolving module is disabled \n
  Any of error codes reported by the ZDO_ZdpReq() function
******************************************************************************/
void ZDO_ResolveAddrReq(ZDO_ResolveAddrReq_t *req);

/*************************************************************************//**
  \brief Determines NWK address by Extended 64-bit address.
  \param[in] extAddr - pointer to extended 64-bit IEEE address
  \return pointer to a valid NWK short address if it has been found,
          NULL otherwise.
*****************************************************************************/
const ShortAddr_t* ZDO_FindShortByExt(const ExtAddr_t *const extAddr);

/*************************************************************************//**
  \brief Determines Extended 64-bit address by NWK address.
  \param[in] shortAddr - pointer to NWK address.
  \return pointer to a valid IEEE address if it has been found,
          NULL otherwise.
*****************************************************************************/
const ExtAddr_t* ZDO_FindExtByShort(const ShortAddr_t shortAddr);

#endif // _ZDOADDRRESOLVING_H
