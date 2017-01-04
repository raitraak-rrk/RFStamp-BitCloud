/**************************************************************************//**
  \file zdoBinding.h

  \brief Internal interface of ZDO binding functionality.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-17 Max Gekk - Created.
   Last change:
    $Id: zdoBinding.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _ZDO_BINDING_H
#define _ZDO_BINDING_H

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
#if defined _BINDING_
/**************************************************************************//**
  \brief Prepare APS_DataReq_t structure for Bind/Unbind ZDP request.

  \param[in] zdpReq - the pointer to ZDP request for Bind (Unbind) request.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoUnbindBindRequestCreate(ZDO_ZdpReq_t *const zdpReq);

/**************************************************************************//**
  \brief Process an incoming Bind or Unbind request and send response.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of 
                          the received Mgmt Bind request command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoUnbindBindInd(const APS_DataInd_t *const apsDataInd);

/**************************************************************************//**
  \brief Prepare APS_DataReq_t structure for ZDP Mgmt Bind request.
  \param[in] zdpReq - the pointer to ZDP request's structure
                      for the Mgmt Bind Request.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoMgmtBindRequestCreate(ZDO_ZdpReq_t *const zdpReq);

/**************************************************************************//**
  \brief Process an incoming Mgmt Bind request and send response.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of 
                          the received Mgmt Bind request command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoMgmtBindInd(const APS_DataInd_t *const apsDataInd);

#endif /* _BINDING_ */
#endif /* _ZDO_BINDING_H */
/** eof zdoBinding.h */
