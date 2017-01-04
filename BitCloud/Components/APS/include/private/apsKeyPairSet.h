/**************************************************************************//**
  \file  apsKeyPairSet.h

  \brief Private interface of APS Key-Pair Set.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-11-01 Max Gekk - Created.
   Last change:
    $Id: apsKeyPairSet.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_KEY_PAIR_SET_H
#define _APS_KEY_PAIR_SET_H

#if defined _LINK_SECURITY_
/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <apsCryptoKeys.h>
#include <sspCommon.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
/* Size of APS security counter in bytes */
#define APS_SECURITY_COUNTER_SIZE 4U

/******************************************************************************
                                Types section
 ******************************************************************************/
/* Type of outgoing frame counter higher bytes. */
typedef uint16_t ApsOutFrameCounterTop_t;
/* Type of outgoing frame counter lower bytes. */
typedef uint16_t ApsOutFrameCounterLow_t;

/* Unsigned type of Key-Pair Index. */
typedef APS_KeyPairAmount_t ApsKeyPairUIndex_t;

/** Outgoing security frame counter handle type. */
BEGIN_PACK
typedef union PACK
{
  ApsOutFrameCounter_t value;
  struct
  {
    LITTLE_ENDIAN_OCTET(2,
      (ApsOutFrameCounterLow_t low,
       ApsOutFrameCounterTop_t top))
  } part;
} ApsOutFrameCounterHandle_t;
END_PACK

/* Type of key-pair descriptor. */
typedef struct _ApsKeyPairDescriptor_t
{
  /* Identifies the address of the entity with which this key-pair is shared. */
  ExtAddr_t deviceAddress;
  /* Outgoing frame counter higher bytes. */
  ApsOutFrameCounterTop_t outCounterTop;
  /* Bitmask of flags indicating entry status/type */
  uint8_t flags;
  /* The actual value of the initial key:
     master key for High Security and preinstalled link key for SE Security. */
  uint8_t initialKey[SECURITY_KEY_SIZE];
  /* The actual value of the link key. */
  uint8_t linkKey[SECURITY_KEY_SIZE];
} ApsKeyPairDescriptor_t;

/* Type of incoming and outgoing counters. */
typedef struct _ApsKeyCounters_t
{
  /* Outgoing frame counter lower bytes. */
  ApsOutFrameCounterLow_t outCounterLow;
  /* Incoming frame counter value corresponding to DeviceAddress. */
  ApsInFrameCounter_t in;
} ApsKeyCounters_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Get current value of outgoing frame counter and increment it.

  \param[in] keyHandle - valid key handle.

  \return Pointer to current value of outgoing frame counter.
 ******************************************************************************/
APS_PRIVATE
ApsOutFrameCounter_t apsGetUpdatedOutFrameCounter(const APS_KeyHandle_t keyHandle);

/**************************************************************************//**
  \brief Get current value of incoming frame counter.

  \param[in] handle - valid key handle.

  \return Pointer to current value of incoming frame counter.
 ******************************************************************************/
APS_PRIVATE
ApsInFrameCounter_t apsGetInFrameCounter(const APS_KeyHandle_t handle);

/**************************************************************************//**
  \brief Is the handle assocciated with a global or unique link key?

  \param[in] handle - valid key handle.

  \return true if handle is assocciated with a global link key otherwise false.
 ******************************************************************************/
APS_PRIVATE bool apsIsGlobalLinkKey(const APS_KeyHandle_t handle);

#endif /* _LINK_SECURITY_ */

#endif /* _APS_KEY_PAIR_SET_H */
/** eof apsKeyPairSet.h */

