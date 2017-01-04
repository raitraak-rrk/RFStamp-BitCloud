/**************************************************************************//**
  \file rfRegAccess.h

  \brief Radio registers access interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-03-01 Dmitry Kolmakov - Created.
   Last change:
    $Id: rfCalibration.h 18970 2011-10-21 12:48:41Z mgekk $
 ******************************************************************************/
#if !defined _RFREGACCESS_H
#define _RFREGACCESS_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <macCommon.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/

/**//**
 * \brief The type declaration of the RF register value.
 */
typedef uint8_t RF_RegValue_t;

/**//**
 * \brief The type declaration of the RF register address.
 */
typedef uint16_t RF_RegAddress_t;

/**
 * \brief RF register access status type.
 *
 * - RF_REGISTER_SUCCESS_STATUS means that the value of the requested rf
 *   register has been read successfully.
 * - RF_REGISTER_FAIL_STATUS - MAC hardware depended subsystem cannot perform
 *   reading of the register at this time.
 **/
typedef enum _RF_RegAccessStatus_t
{
  RF_REGISTER_SUCCESS_STATUS,
  RF_REGISTER_FAIL_STATUS
} RF_RegAccessStatus_t;

/**//**
 * \brief The structure declaration of RF register access confirmation parameters.
 */
typedef struct _RF_RegAccessConf_t
{
  RF_RegAccessStatus_t status;
  RF_RegValue_t        value;
} RF_RegAccessConf_t;

/**//**
 * \brief Parameters of RF register access request.
 */
typedef struct _RF_RegAccessReq_t
{
  /*! Service field - for internal needs. */
  MAC_Service_t   service;

  /** Value to write to the register. */
  RF_RegAddress_t addr;

  /** Value to write to the register. */
  RF_RegValue_t   value;

  /** RF register access confirm callback function's pointer. 
      Must not be set to NULL. */
  void (*RF_RegAccessConf)(RF_RegAccessConf_t *conf);

  /** Confirmation parameters of processing this request. */
  RF_RegAccessConf_t confirm;
} RF_RegAccessReq_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Initiates a register writing procedure.

  \param[in] req - pointer to register write parameters -
                   callback and conf status.

  \return None.
 ******************************************************************************/
void RF_RegWriteReq(RF_RegAccessReq_t *const req);

/**************************************************************************//**
  \brief Initiates a register reading procedure.

  \param[in] req - pointer to register read parameters -
                   callback and conf status.

  \return None.
 ******************************************************************************/
void RF_RegReadReq(RF_RegAccessReq_t *const req);

#endif /* _RFREGACCESS_H */
/** eof rfRegAccess.h */
