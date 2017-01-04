/**************************************************************************//**
  \file zdoConfig.h

  \brief ZDO Constant definition header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2013-02-19 Max Gekk - Created.
   Last change:
    $Id: zdoConfig.h 24552 2013-02-19 11:52:53Z mgekk $
******************************************************************************/
#if !defined _ZDO_CONFIG_H
#define _ZDO_CONFIG_H

/******************************************************************************
                             Definitions section
 ******************************************************************************/
#if defined _CERTIFICATION_
/* It should be 1U on certification. See log from Golden Units on 2.4GHz. */
#define ZDP_BIND_APS_ACK 1U
#else
/** The flag requires APS acks for ZDP Bind request and response.*/
#define ZDP_BIND_APS_ACK 0U
#endif

#endif /* _ZDO_CONFIG_H */
/** eof zdoConfig.h */
