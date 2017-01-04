/************************************************************************//**
  \file zcl.h

  \brief
    The header file describes internal ZCL interface

    The header file describes internal ZCL interface and types

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    09.12.08 I. Fedina - Created.
******************************************************************************/

#ifndef _ZCLINT_H
#define _ZCLINT_H

#include <zcl.h>
#include <clusters.h>

BEGIN_PACK
typedef struct PACK
{
  uint8_t           status;
  ZCL_AttributeId_t id;
  uint8_t           type;
  uint8_t           value[1];
} ZclReadAttributeResp_t;

typedef struct PACK
{
  uint8_t           status;
  ZCL_AttributeId_t id;
} ZclWriteAttributeResp_t;
END_PACK

ZclAttribute_t *zclGetNextAttribute(Endpoint_t endpoint, ClusterId_t clusterId, uint8_t direction, ZclAttribute_t *currentAttribute);

#endif // _ZCL_INT_H
