/**************************************************************************//**
  \file  sysUtils.h
  \brief Header file describes stack utilities functions.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
     History:
      07/10/08 - Created.
******************************************************************************/
#ifndef _SYSUTILS_H
#define _SYSUTILS_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <string.h>
#include <stdlib.h>
#include <sysIncrementMacro.h>
#include <sysRepeatMacro.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
//\cond internal
#if defined(AT91SAM7X256) || defined(AT91SAM3S4C) || defined(AT91SAM4S16C) || defined(ATSAMD20J18) \
  || defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
  #define SYS_BYTE_MEMCPY SYS_ByteMemcpy
#elif defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) \
   || defined(ATXMEGA128A1) || defined(ATXMEGA256A3) || defined(ATXMEGA128B1) || defined(ATXMEGA256D3) \
   || defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
  #define SYS_BYTE_MEMCPY memcpy
#else
  #define SYS_BYTE_MEMCPY memcpy
#endif

#if defined(ATMEGA1281) || defined(ATMEGA2561) || defined(ATMEGA1284) || defined(AT90USB1287) || \
   defined(ATMEGA128RFA1) || defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
/* Macroses to accept memory I/O registers for AVR Mega family */
#define MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
#define MMIO_WORD(mem_addr) (*(volatile uint16_t *)(mem_addr))
#endif /* AVR Mega family */

#define  GET_FIELD_PTR(structPtr, typeName, fieldName) \
  ((uint8_t *)(structPtr) + offsetof(typeName, fieldName))

#define GET_PARENT_BY_FIELD(TYPE, FIELD, FIELD_POINTER)  \
  ((TYPE *)(((uint8_t *)FIELD_POINTER) - offsetof(TYPE, FIELD)))
#define GET_CONST_PARENT_BY_FIELD(TYPE, FIELD, FIELD_POINTER)  \
  ((const TYPE *)(((const uint8_t *)FIELD_POINTER) - offsetof(TYPE, FIELD)))

#define GET_STRUCT_BY_FIELD_POINTER(struct_type, field_name, field_pointer)\
  ((struct_type *) (((uint8_t *) field_pointer) - FIELD_OFFSET(struct_type, field_name)))
#define GET_INDEX_FROM_OFFSET(PTR1, PTR2)  (PTR1 - PTR2)

// Size of slice between firstField end lastField of struct (including lastField)
#define SLICE_SIZE(type, firstField, lastField)\
  (offsetof(type, lastField) - offsetof(type, firstField) + sizeof(((type *)0)->lastField))

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define ADD_COMMA(value, n) value,
/* Macro for array initialization */
#define INIT_ARRAY(initValue, size) {REPEAT_MACRO(ADD_COMMA, initValue, size)}

#undef FIELD_OFFSET
#define FIELD_OFFSET(struct_type, field_name)\
  (((uint8_t*) &((struct_type *)(NULL))->field_name) - (uint8_t*)NULL)

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)<(y)?(y):(x))

#define FIRST_BIT_SET(bitmask)     ((bitmask) & (~(bitmask) + 1))
#define FIRST_BIT_CLEARED(bitmask) (~(bitmask) & ((bitmask) + 1))

#define COMPARE_WITH_THRESHOLD(a, b, threshold) \
  (abs((a) - (b)) < (threshold) ? ((a) > (b) ? 1 : 0) : ((a) > (b) ? 0 : 1))

#define CEIL(a, b) (((a) - 1U)/(b) + 1U)
/******************************************************************************
                        Inline functions' section.
******************************************************************************/
/**************************************************************************//**
  \brief Performs bytes memory copying operation.

  \param dst  - points destination memory start address.
         src  - points source memory start address.
         size - number of bytes to copy.
  \return pointer to dst.
******************************************************************************/
INLINE void *SYS_ByteMemcpy(void *dst, const void *src, uint16_t size)
{
  uint8_t *dst_ = (uint8_t *) dst;
  const uint8_t *src_ = (const uint8_t *) src;

  while(size--)
    *(dst_++) = *(src_++);
  return dst;
}

/**************************************************************************//**
  \brief Performs swap bytes in array of length

  \param array  - pointer to array.
         length  - array length
  \return no return.
******************************************************************************/
void SYS_Swap(uint8_t *array, uint8_t length);

/**************************************************************************//**
\brief  Starts timer to update rand
******************************************************************************/
void sysStartUpdatingRandSeed(void);

#if defined(_SLEEP_WHEN_IDLE_)
/**************************************************************************//**
  \brief Stops priodic updating of the random seed.
******************************************************************************/
void sysStopUpdatingRandSeed(void);
#endif /* _SLEEP_WHEN_IDLE_ */
//\endcond

/**************************************************************************//**
\brief Generates a random two-bytes number

\ingroup sys

Generation of a random number is based on the seed value created in ZDO
with the use of the radio (if the radio supports such mechanism). The stack
creates the seed value on startup and updates it periodically later on.

\returns A random number in the range 0x000 - 0xFFFF
******************************************************************************/
static inline uint16_t SYS_GetRandomNumber(void)
{
  uint16_t result = rand();
  return result;
}

/**************************************************************************//**
\brief Generates a random two-bytes number normalized by given upper limit.

\ingroup sys

Generation of a random number is based on the seed value created in ZDO
with the use of the radio (if the radio supports such mechanism). The stack
creates the seed value on startup and updates it periodically later on.

\param [in] upperLimit - The upper limit of random number

\returns A normalized random number in the range 0x000 - upper limit
******************************************************************************/
static inline uint16_t SYS_GetNormalizedRandomNumber(uint16_t upperLimit)
{
  uint32_t result = (uint32_t)rand();

  result = (result * upperLimit) / 0xFFFFU;

  return result;
}

/**************************************************************************//**
\brief Generates a sequence of random numbers and copies them into the buffer

\ingroup sys

The function uses the same mechanism as the SYS_GetRandomNumber() function,
but creates the specified number of random bytes.

\param [out] buffer - The buffer to hold generated random data
\param [in]  size   - The number of bytes of random data to compute and store
in the buffer

\return  0 - indicates successful completion
******************************************************************************/
int SYS_GetRandomSequence(uint8_t *buffer, unsigned long size);

/**************************************************************************//**
\brief Calculates CRC using CRC-16-CCITT algorithm

\ingroup sys

\param [in] initValue - CRC initial value
\param [in] byte - next byte to calculate CRC

\return  calculated CRC value
******************************************************************************/
INLINE uint16_t SYS_Crc16Ccitt(uint16_t initValue, uint8_t byte)
{
  byte ^= initValue & 0xffU;
  byte ^= byte << 4U;

  return ((((uint16_t)byte << 8) | ((initValue & 0xff00U) >> 8))
          ^ (uint8_t)(byte >> 4) ^ ((uint16_t)byte << 3));
}

#ifndef _MAC2_
/**************************************************************************//**
\brief This function reads version number in CS and returns as string

\param[out] strVersion - string version. intVersion - integer version

\return:  bool. true indicates success. false indicates failure,
            which means version is unavailable
******************************************************************************/
bool SYS_GetBitCloudRevision(uint8_t *strVersion, uint32_t *intVersion);
#endif // _MAC2_

#endif // _SYSUTILS_H
// eof sysUtils.h
