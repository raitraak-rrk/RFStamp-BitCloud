/**************************************************************************//**
  \file zdoServiceDiscovery.h

  \brief The header file describes the constants and types of ZDO discovery manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-12 Max Gekk - Refactoring. Support parallel processing of ZDP requests.
   Last change:
    $Id: zdoServiceDiscovery.h 23835 2012-12-12 15:52:36Z mgekk $
 ******************************************************************************/
#if !defined _ZDO_SERVICE_DISCOVERY_H
#define _ZDO_SERVICE_DISCOVERY_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <zdo.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Prepare ZDP request to be sent.

  \param[in] zdpReq - the pointer to the ZDP request's structure for
                      ZDP Discovery Requests.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpDiscoveryRequestCreate(ZDO_ZdpReq_t *zdpReq);

/******************************************************************************
  \brief Process ZDP Discovery request.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of
                          the received ZDP command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpDiscoveryMessageInd(const APS_DataInd_t *const apsDataInd);

/******************************************************************************
  \brief Process ZDP Device Annce command.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of
                          the received Device Annce command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoDeviceAnnceInd(const APS_DataInd_t *const apsDataInd);

#endif /* _ZDO_SERVICE_DISCOVERY_H */
/** eof zdoServiceDiscovery.h */
