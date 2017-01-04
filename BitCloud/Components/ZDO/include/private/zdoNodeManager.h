/**************************************************************************//**
  \file zdoNodeManager.h

  \brief The header file describes the constants, types and internal interface
         of ZDO node manager

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2012-12-14 Max Gekk - Refactoring.
   Last change:
    $Id: zdoNodeManager.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _ZDO_NODE_MANAGER_H
#define _ZDO_NODE_MANAGER_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <aps.h>
#include <zdo.h>

/******************************************************************************
                               Prototypes section
 ******************************************************************************/
/******************************************************************************
  \brief Prepare ZDP request to be sent.
  \param[in] zdpReq - the pointer to the ZDP request's structure for
                      the Node ZDP request.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpNodeRequestCreate(ZDO_ZdpReq_t *zdpReq);

/******************************************************************************
  \brief Process ZDP Node Manager request.

  \param[in] apsDataInd - pointer to APSDE-DATA indication structure of
                          the received ZDP command.
  \return None.
 ******************************************************************************/
ZDO_PRIVATE void zdoZdpNodeMessageInd(const APS_DataInd_t *const apsDataInd);

#endif /* _ZDO_NODE_MANAGER_H */
/** eof zdoNodeManager.h */
