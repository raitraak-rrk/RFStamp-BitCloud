/**************************************************************************//**
  \file apsGroup.h

  \brief Interface of APS Multicast Transmission.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-12 Max Gekk - Created.
   Last change:
    $Id: apsGroup.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_GROUP_H
#define _APS_GROUP_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <aps.h>
#include <apsMemoryManager.h>

#if defined _APS_MULTICAST_
/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/*****************************************************************************
  \brief Group indication (APSDE-DATA.indication) to all group members.

  \param[in] apsDataInd - pointer to APSDE-DATA.indication's parameters.
  \return None.
 *****************************************************************************/
APS_PRIVATE void apsGroupIndication(APS_DataInd_t *const apsDataInd);

/*****************************************************************************
  \brief Process multicast packets from NWK layer.

  \param[in] nwkDataInd - pointer to NLDE-DATA.indication parameters.
  \param[in] securityStatus - kind of used security.

  \return true  - response for NWK_DataInd requered,
          false - response for NWK_DataInd not required
                  and will be made in the future.
 *****************************************************************************/
APS_PRIVATE bool apsGroupPacketHandler(const NWK_DataInd_t *const nwkDataInd,
  const APS_Status_t securityStatus);

/*****************************************************************************
  \brief Process APSDE-DATA.request with Group Addressing Mode

  \param[in] apsDataReq - pointer to APSDE-DATA.request's parameters
  \param[in] apsBuffer - pointer to APS buffer with NLDE-DATA.request.

  \return APS_SUCCESS_STATUS
 *****************************************************************************/
APS_PRIVATE APS_Status_t apsProcessGroupAddrModeDataReq(APS_DataReq_t *apsDataReq,
  ApsDataBuffer_t *apsBuffer);

#endif /* _APS_MULTICAST_ */
#endif /* _APS_GROUP_H */
/** eof apsGroup.h */
