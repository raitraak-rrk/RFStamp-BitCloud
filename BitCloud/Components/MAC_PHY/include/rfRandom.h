/**************************************************************************//**
  \file rfRandom.h

  \brief Prototype of random generation function.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      08/04/08 A. Mandychev - Created.
******************************************************************************/

#ifndef _RFRANDOM_H
#define _RFRANDOM_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macCommon.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
//! Random confirm structure.
typedef struct
{
  uint8_t *buff;
  uint16_t value;
} RF_RandomConf_t;

//! Random request structure.
typedef struct
{
  /** \cond SERVICE_FIELDS **/
  struct
  {
    void *next; /*!< Used for queue support */
#ifdef _ZAPPSI_
    /* to be compatible with MAC service. */
    uint8_t requestId;
    /* Sequence number to identify request-response pair. */
    uint8_t sequenceNumber;
    union
    {
      /* Callback wrapper routine pointer */
      void (*callback)(void*);
      /* Processing routine pointer to call after required memory will become free */
      void (*process)(void*);
    } zsi;
#else /*  _ZAPPSI_ */
    uint8_t requestId;
#endif /* _ZAPPSI_ */
  } service;
  /** \endcond **/
  //! The flag indicates that MAC laer has to enable or disable frame receiving
  // during processing of the Random request.
  bool disableRx;

  //! Number of bytes to be generated
  uint8_t numberOfBytes;
  //! Confirm structure on RF_RandomReq.
  RF_RandomConf_t confirm;
  //! Callback on RF_RandomReq. Must not be set to NULL. 
  void (*RF_RandomConf)(RF_RandomConf_t *conf);
} RF_RandomReq_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
  \brief Requests random value from RF chip. Valid for RF231 and RF212.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void RF_RandomReq(RF_RandomReq_t *reqParams);

/**************************************************************************//**
  \brief Requests random value from RF chip. Valid for RF231 and RF212.
  \param reqParams - request parameters structure pointer.
  \return none.
******************************************************************************/
void RF_RandomSeqReq(RF_RandomReq_t *reqParams);

#endif /*_RFRANDOM_H*/

// eof rfRandom.h
