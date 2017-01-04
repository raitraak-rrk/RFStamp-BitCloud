/**************************************************************************//**
  \file tcDeviceTableCtrl.h

  \brief Security Trust Centre devices' control header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      2008.02 - ALuzhetsky created.
******************************************************************************/
#ifndef _TCDEVICETABLECTRLH
#define _TCDEVICETABLECTRLH

/******************************************************************************
                             Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <mac.h>
#include <aps.h>
#include <sspCommon.h>

#ifdef _PERMISSIONS_
/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief Adds new device to Trust Centre device permission table.

  \param extAddr - new device's address.
  \return true in case if operation is successful - false if there is no free
  space in Trust Centre device permission table.
******************************************************************************/
bool TC_AddDeviceToPermissionTable(ExtAddr_t *extAddr);


/**************************************************************************//**
  \brief Removes device from Trust Centre device permission table.

  \param extAddr - address of the device to remove.
  \return true in case if operation is successful - false if there is no such
  device in Trust Centre device permission table.
******************************************************************************/
bool TC_RemoveDeviceFromPermissionTable(ExtAddr_t *extAddr);
#endif // _PERMISSIONS_

/******************************************************************************
                             Types section.
******************************************************************************/
/**************************************************************************//**
  \brief Possible key types which could be stored for each device.

   Used in TC_SetDeviceKey primitive - to fill device table with particular
   values.
******************************************************************************/
typedef enum
{
  //! Master key type.
  KEY_TYPE_MASTER,
  //! Link key type.
  KEY_TYPE_LINK
} TC_KeyType_t;

/**************************************************************************//**
  \brief Trust Centre set device key primitive paramemtrs.

   Used in TC_SetDeviceKey primitive - to fill device table with particular
   values.
******************************************************************************/
typedef struct
{
  //! Device extended address.
  ExtAddr_t    *deviceAddr;
  //! Key value - 16 bytes array.
  uint8_t      (*key)[SECURITY_KEY_SIZE];
  //! Key type.
  TC_KeyType_t keyType;
} TC_SetDeviceKey_t;

/******************************************************************************
                             Functions prototypes section.
******************************************************************************/
/**************************************************************************//**
  \brief Sets device parameters in device table - extended address associated
  with particular key.

  \param params -  request parameters' structure pointer.
  \return true in case if operation is successful - false otherwise.
******************************************************************************/
bool TC_SetDeviceKey(TC_SetDeviceKey_t *param);

/**************************************************************************//**
  \brief Deletes whole information about device with extended address equeal to
         deviceAddr from APS_KeyPairSet_t table.
  \param deviceAddr - device address.

  \return true - if device with extended address deviceAddr was found and its
          entry was deleted, false - otherwise.
******************************************************************************/
bool TC_DeleteDeviceEntry(ExtAddr_t *deviceAddr);

#endif // _TCDEVICETABLECTRLH

// eof tcDeviceTableCtrl.h
