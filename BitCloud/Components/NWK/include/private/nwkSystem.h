/**************************************************************************//**
  \file nwkSystem.h

  \brief Network system header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2007-06-20 V. Panov - Created.
    2009-08-16 M. Gekk  - Refactoring.
   Last change:
    $Id: nwkSystem.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _NWKSYSTEM_H
#define _NWKSYSTEM_H

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <sysTypes.h>
#include <appFramework.h>
#include <sysUtils.h>
#include <sysAssert.h>

 /******************************************************************************
                   Inline static functions prototypes section
 ******************************************************************************/
INLINE bool isCorrectExtPanId(const uint64_t panId)
{
  const uint8_t *p = (const uint8_t *)&panId;
  uint8_t panIdLength = sizeof(panId);
  uint16_t sum = 0U;
  
  while(panIdLength > 0)
  {
    sum += *(p++);
    panIdLength--;
  }
  return (0U != sum) && ((8U * 0xFFU) != sum);
}

/******************************************************************************
                               Types section
 ******************************************************************************/
typedef  ShortAddr_t  NwkAddrRange_t;

/******************************************************************************
                            Definitions section
 ******************************************************************************/
/** Converting of time value from milliseconds to seconds. */
#define NWK_MSEC_TO_SEC(time) ((time) >> 10)
/** Converting of time value from seconds to milliseconds. */
#define NWK_SEC_TO_MSEC(time) ((time) << 10)

#define IS_CORRECT_BROADCAST_ADDR(A) \
  ((0xFFFFU == (A))||((0xFFFBU <= LE16_TO_CPU(A))&&(LE16_TO_CPU(A) <= 0xFFFDU)))

#define NWK_INVALID_PANID  0xFFFFU

#define CHECK_MEM_OVERLAP(dst, src, len, assert_num)         \
  SYS_E_ASSERT_FATAL(!(((src) < (dst))                       \
    && ((uint8_t *)(dst) < (const uint8_t *)(src) + (len))), \
    assert_num )

#if defined UINT64_MEMCMP
  #define IS_EQ_EXT_PANID(A, B) IS_EQ_64BIT_VALUES(A, B)
  #define IS_CORRECT_EXT_PANID(P) isCorrectExtPanId(P)
  #define COPY_TO_RAW_EXT_ADDR(DST, SRC) \
    memmove((DST).raw, &(SRC), sizeof(uint64_t))
  #define COPY_FROM_RAW_EXT_ADDR(DST, SRC) \
    memmove(&(DST), (SRC).raw, sizeof(uint64_t))
  #define COPY_EXT_PANID(DST, SRC) memmove(&(DST), &(SRC), sizeof(uint64_t))
#else
  #define IS_EQ_EXT_PANID(A, B) IS_EQ_64BIT_VALUES(A, B)
  #define IS_CORRECT_EXT_PANID(P) \
    ((0ULL < (P)) && ((P) < 0xFFFFFFFFFFFFFFFFULL))
  #define COPY_TO_RAW_EXT_ADDR(DST, SRC) (DST).value = (SRC)
  #define COPY_FROM_RAW_EXT_ADDR(DST, SRC) (DST) = (SRC).value
  #define COPY_EXT_PANID(DST, SRC) COPY_64BIT_VALUE(DST, SRC)
#endif

/* Get address of any NWK table. */
#define NWK_ADDR_OF_TABLE(table) ((void **)&(table))

/* Macros are used for queue operations. */
#define NWK_DEL_QELEM(queue, type) \
  GET_PARENT_BY_FIELD(type, service.qelem, deleteHeadQueueElem(queue))

#define NWK_GET_QELEM(queue, type) \
  GET_PARENT_BY_FIELD(type, service.qelem, getQueueElem(queue))

#define NWK_NEXT_QELEM(parent, type) \
  GET_PARENT_BY_FIELD(type, service.qelem, getNextQueueElem(&(parent)->service.qelem))

#endif /* _NWKSYSTEM_H */
/** eof nwkSystem.h */

