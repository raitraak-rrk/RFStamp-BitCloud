/**************************************************************************//**
  \file macSetGet.h

  \brief Types', constants' and functions' declarations for IEEE 802.15.4-2006
     set and get primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      18/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef  _MACSETGET_H
#define  _MACSETGET_H

/******************************************************************************
                        Includes section
******************************************************************************/
#include <macphyPib.h>

/******************************************************************************
                        Defines section
******************************************************************************/

/******************************************************************************
                        Types section
******************************************************************************/
/**
 * \brief MLME-SET confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.13.2 MLME-SET.confirm.
*/
typedef struct
{
  //! The result of the request to write the PIB attribute.
  MAC_Status_t   status;
  //! The identifier of the PIB attribute that was written.
  MACPHY_PibId_t attrId;
}  MAC_SetConf_t;

/**
 * \brief MLME-SET request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.13.1 MLME-SET.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! The identifier of the PIB attribute to write.
  MACPHY_PibId_t   attrId;
  //! The value to write to the indicated PIB attribute.
  MACPHY_PibAttr_t attrValue;
  //! MLME-SET confirm callback function's pointer.
  void (*MAC_SetConf)(MAC_SetConf_t *conf);
  //! MLME-SET confirm parameters' structure.
  MAC_SetConf_t confirm;
}  MAC_SetReq_t;

/**
 * \brief MLME-GET confirm primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.6.2 MLME-GET.confirm.
 */
typedef struct
{
  //! The result of the request for PIB attribute information.
  MAC_Status_t status;
  //! The identifier of the PIB attribute that was read.
  MACPHY_PibId_t   attrId;
  //! The value of the indicated PIB attribute that was read.
  MACPHY_PibAttr_t attrValue;
}  MAC_GetConf_t;

/**
 * \brief MLME-GET request primitive's parameters structure declaration.
 * IEEE 802.15.4-2006 7.1.6.1 MLME-GET.request.
 */
typedef struct
{
  //! Service field - for internal needs.
  MAC_Service_t service;
  //! The identifier of the PIB attribute to read.
  MACPHY_PibId_t   attrId;
  //! MLME-GET confirm callback function's pointer.
  void (*MAC_GetConf)(MAC_GetConf_t *conf);
  //! MLME-GET confirm parameters' structure.
  MAC_GetConf_t confirm;
}  MAC_GetReq_t;

/******************************************************************************
                        Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief MLME-SET request primitive's prototype.
  \param reqParams - MLME-SET request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_SetReq(MAC_SetReq_t *reqParams);

/**************************************************************************//**
  \brief MLME-GET request primitive's prototype.
  \param reqParams - MLME-GET request parameters' structure pointer.
  \return none.
******************************************************************************/
void MAC_GetReq(MAC_GetReq_t *reqParams);

#endif /* _MACSETGET_H */

// eof macSetGet.h
