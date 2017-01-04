/**************************************************************************//**
  \file aps.h

  \brief Interface of APS layer.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-20 Max Gekk - Created.
   Last change:
    $Id: aps.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_H
#define _APS_H
/**//**
 * \page Endian
 * \note \b |LE| - parameter in little endian format.
 *
 *   Use macroes CPU_TO_LE* and CCPU_TO_LE* from sysEndian.h to convert from
 * CPU-endian to little-endian format.
 **/

/******************************************************************************
                                 Includes section
 ******************************************************************************/
#include <apsCommon.h>
#include <apsdeEndpoint.h>
#include <apsdeData.h>
#include <apsCommandReq.h>
#include <apsmeGroup.h>
#include <apsmeBind.h>
#include <apsAIB.h>
#include <apsCryptoKeys.h>
#include <apsmeAuthenticate.h>
#include <apsmeEstablishKey.h>
#include <apsmeRemoveDevice.h>
#include <apsmeRequestKey.h>
#include <apsmeSwitchKey.h>
#include <apsmeTransportKey.h>
#include <apsmeUpdateDevice.h>
#include <intrpData.h>

/******************************************************************************
                                Types section
 ******************************************************************************/
/** External events identifiers. */
typedef enum
{
  APS_EVENT_RESET_ID = 0x19,
  APS_EVENT_START_ID = 0x8a,
  APS_EVENT_STOP_ID  = 0xc5
} APS_EventId_t;

/** Type of external APS event. */
typedef struct
{
  void *next;       /*!< Used for queue support. */
  APS_EventId_t id; /*!< Identifier of APS event. */
} APS_Event_t;

/** Confirmation parameters of APS Reset. */
typedef struct
{
  APS_Status_t status;
} APS_ResetConf_t;

/** Type of APS Reset parameters structure. */
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    APS_Event_t event;
    /** NLME-RESET request primitive's parameters structure. */
    NWK_ResetReq_t nwkResetReq;
  } service;
  /** \endcond **/

  /** APS reset confirm callback function's pointer. Must not be set to NULL.*/
  void (*APS_ResetConf)(APS_ResetConf_t *conf);
  APS_ResetConf_t confirm; /*!< Parameters are passed to callback function. */
} APS_ResetReq_t;

/** Type of startup confirmation parameters. */
typedef struct
{
  APS_Status_t status; /*!< Status code of APS startup. */
} APS_StartConf_t;

/** Parameters of APS start procedure. */
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    APS_Event_t event;
  } service;
  /** \endcond **/

  /** APS startup callback function. Must not be set to NULL. */
  void (*APS_StartConf)(APS_StartConf_t *conf);
  APS_StartConf_t confirm; /*!< APS startup confirmation parameters. */
} APS_StartReq_t;

/** Type of APS stoping confirmation parameters. */
typedef struct
{
  APS_Status_t status; /*!< Status code of APS stoping. */
} APS_StopConf_t;

/** Parameters of APS stoping primitive. */
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    APS_Event_t event;
    uint16_t waitMask; /*! Bit map of stopping components. */
  } service;
  /** \endcond **/

  /** Callback and APS stoping confirmation function. Must not be set to NULL. */
  void (*APS_StopConf)(APS_StopConf_t *conf);
  APS_StopConf_t confirm;
} APS_StopReq_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Initialization internal state and main variables of APS-layer.

     The function can be called only by ZDO.
     It must be called before any operation with APS.

  \return None.
 ******************************************************************************/
void APS_Init(void);

/**************************************************************************//**
  \brief Resets APS to default state.

     The function can be called only by ZDO and only after APS_Init().

  \param[in] req - pointer to reset parameters.
  \return None.
 *****************************************************************************/
void APS_ResetReq(APS_ResetReq_t *const req);

/**************************************************************************//**
  \brief Makes APS allow processing of incoming packets.

   After this function is called, APS is ready to process incoming packets.

  \param[in] req - pointer to startup parameters.
  \return None.
 ******************************************************************************/
void APS_StartReq(APS_StartReq_t *const req);

/**************************************************************************//**
  \brief Makes APS disallow processing of incoming packets.

   After this function is called, APS does not process incoming packets.

  \param[in] req - pointer to stopping parameters.
  \return None.
 ******************************************************************************/
void APS_StopReq(APS_StopReq_t *const req);

/**************************************************************************//**
  \brief Check if APS is started.

  \return 'true' if APS is started otherwise return 'false'
 ******************************************************************************/
bool APS_IsStarted(void);

/**************************************************************************//**
  \brief Check if there are any active requests in progress.

  \return 'true' if there are any active requests in progress otherwise - 'false'
 ******************************************************************************/
bool APS_IsActiveTransaction(void);

/**************************************************************************//**
  \brief Resetting choosed tables.
  
  \param[in] ctrl - set of flags to checking.
  \return None
 ******************************************************************************/
void APS_HandleLeave(NWK_LeaveControl_t *const ctrl);

/**************************************************************************//**
  \brief Delete the specified element in APS Data request queue.

  \param[in] apsDataReq - element to be deleted
 ******************************************************************************/
void APS_ApsDataReqDeleteQueueElem(APS_DataReq_t *apsDataReq);
#endif  /* _APS_H */
/** eof aps.h */

