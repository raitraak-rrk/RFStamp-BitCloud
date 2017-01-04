/************************************************************************//**
  \file zclMemoryManager.c

  \brief
    The ZCL Memory Manager implementation

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    03.12.08 A. Potashov - Created.
******************************************************************************/
#if ZCL_SUPPORT == 1
/******************************************************************************
                   Includes section
******************************************************************************/
#include <sysUtils.h>
#include <zclMemoryManager.h>
#include <configServer.h>
#include <zclDbg.h>
#include <sysAssert.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
typedef uint8_t ZclQuota_t;

/******************************************************************************
                               Definitions section
 ******************************************************************************/
#define ZCL_MAX_BUF_WITH_QUOTAS  5U
#define ZCL_READ_BUF_QUOTA(ptr, offset, pktType) \
  memcpy_P(ptr, &quotas[offset][pktType-1U], sizeof(ZclQuota_t))

/******************************************************************************
                               Constants section
 ******************************************************************************/
static PROGMEM_DECLARE (ZclQuota_t
quotas[ZCL_MAX_BUF_WITH_QUOTAS + 1U][ZCL_BUFFER_TYPE_LAST - 1U]) =
{
/* OUT_DATA IN_DATA RESP_DATA REP_DATA*/
  {  2U,      3U,      2U,       2U    }, /* (quota * totalAmount)/4 */
  {  1U,      1U,      1U,       1U    }, /* totalAmount == 1 */
  {  1U,      2U,      2U,       1U    }, /* totalAmount == 2 */
  {  2U,      2U,      2U,       1U    }, /* totalAmount == 3 */
  {  2U,      3U,      2U,       1U    }, /* totalAmount == 4 */
  {  2U,      3U,      2U,       2U    }, /* totalAmount == 5 */
};

/******************************************************************************
                   Implementation section
******************************************************************************/
static bool checkBufferQuotas(ZclBufferType_t type, uint8_t typeAmount, uint8_t totalAmount);

/******************************************************************************
                   Implementation section
******************************************************************************/
/*************************************************************************//**
\brief Looks for and returns free zcl memory buffer

\param[in] type - the type of a buffer

\returns pointer to memory buffer or NULL if there is no free buffer
*****************************************************************************/
ZclMmBuffer_t *zclMmGetMem(ZclBufferType_t type)
{
  uint8_t bufferAmount;
  uint8_t bufferSize;
  ZclMmBufferDescriptor_t *descriptor, *freeBuffer = NULL;
  uint8_t *tmp = NULL;
  uint8_t typeAmount = 0U;

  CS_ReadParameter(CS_ZCL_MEMORY_BUFFERS_AMOUNT_ID, (void *)&bufferAmount);
  CS_ReadParameter(CS_ZCL_BUFFER_SIZE_ID, (void *)&bufferSize);
  CS_GetMemory(CS_ZCL_BUFFER_DESCRIPTORS_ID, (void *)&descriptor);
  CS_GetMemory(CS_ZCL_BUFFERS_ID, (void *)&tmp);
  if (NULL == descriptor)
  {
    SYS_E_ASSERT_ERROR(false, ZCL_THERE_ARE_NO_BUFFERS);
    return NULL;
  }
  /* if descriptor is NULL, control breaks here */

  // Look for free buffer
  for (uint8_t i = 0U; i < bufferAmount; i++)
  {
    if (descriptor->type == type)
      typeAmount++;
    if (ZCL_UNKNOWN_BUFFER == descriptor->type)
    {
      freeBuffer = descriptor;
      freeBuffer->buf.frame = tmp;
    }
    tmp += bufferSize + APS_AFFIX_LENGTH + 2U;
    descriptor++;
  }

  if (freeBuffer && checkBufferQuotas(type, typeAmount, bufferAmount))
  {
    freeBuffer->type = type;
    memset((void *)&(freeBuffer->buf.primitive), 0U, sizeof(freeBuffer->buf.primitive));
    memset(freeBuffer->buf.frame, 0U, bufferSize + APS_AFFIX_LENGTH + 2U);
    freeBuffer->buf.frame[0U] = TOP_GUARD_VALUE;
    freeBuffer->buf.frame[bufferSize + APS_AFFIX_LENGTH + 1U] = BOTTOM_GUARD_VALUE;
    return &freeBuffer->buf;
  }

  return NULL;
}

/*************************************************************************//**
\brief Frees zcl buffer

\param[in] mem - pointer to used zcl buffer
*****************************************************************************/
void zclMmFreeMem(ZclMmBuffer_t *mem)
{
  ZclMmBufferDescriptor_t *descriptor = GET_STRUCT_BY_FIELD_POINTER(ZclMmBufferDescriptor_t, buf, mem);
  uint8_t bufferSize;

  CS_ReadParameter(CS_ZCL_BUFFER_SIZE_ID, (void *)&bufferSize);
  SYS_E_ASSERT_FATAL(((TOP_GUARD_VALUE == descriptor->buf.frame[0U]) &&
    (BOTTOM_GUARD_VALUE == descriptor->buf.frame[bufferSize + APS_AFFIX_LENGTH + 1U])),
    ZCL_MEMORY_CORRUPTION_0);
  descriptor->link = NULL;
  descriptor->type = ZCL_UNKNOWN_BUFFER;
  descriptor->timeout = 0;
}

/*************************************************************************//**
\brief Gets next busy zcl memory buffer descriptor or
  the first one if descr is NULL

\param[in] descr - pointer to current zcl buffer descriptor

\returns pointer to the next busy zcl memory buffer descriptor
  or NULL if no next busy zcl memory buffer descriptor is available
*****************************************************************************/
ZclMmBufferDescriptor_t *zclMmGetNextOutputMemDescriptor(ZclMmBufferDescriptor_t *descr)
{
  uint8_t bufferAmount;
  bool bool_exp;
  ZclMmBufferDescriptor_t *descriptor, *iter;
 
  CS_ReadParameter(CS_ZCL_MEMORY_BUFFERS_AMOUNT_ID, (void *)&bufferAmount);
  CS_GetMemory(CS_ZCL_BUFFER_DESCRIPTORS_ID, (void *)&descriptor);
  bool_exp = (((descr >= descriptor) && (descr < descriptor + bufferAmount)) || (NULL == descr));
  if (false == bool_exp)
  {
    SYS_E_ASSERT_ERROR(false,ZCLMEMORYMANAGER_ZCLMMGETNEXTBUSYDESCRIPTOR_0);
    return NULL;
  }
  if ((descr < descriptor) && (descr >= descriptor + bufferAmount))
    return NULL;

  if (descr)
    iter = descr + 1;
  else
    iter = descriptor;

  while (iter < (descriptor + bufferAmount))
  {
    if (ZCL_OUTPUT_DATA_BUFFER == iter->type)
      return iter;
    iter++;
  }

  return NULL;
}

/**************************************************************************//**
\brief Checks quotas for buffer with given type

\param[in] type - type of input packet;
\param[in] typeAmount - current number of packet with given type;
\param[in] totalAmount - maximum packet buffers

\returns 'true' if quotas are not exceeded otherwise 'false'
 ******************************************************************************/
static bool checkBufferQuotas(ZclBufferType_t type, uint8_t typeAmount, uint8_t totalAmount)
{
  ZclQuota_t quota;

  if (totalAmount <= ZCL_MAX_BUF_WITH_QUOTAS)
  {
    ZCL_READ_BUF_QUOTA(&quota, totalAmount, type);
    return typeAmount < quota;
  }
  else
  {
    ZCL_READ_BUF_QUOTA(&quota, 0U, type);
    return typeAmount < ((quota * totalAmount) >> 2U);
  }
}

#endif // ZCL_SUPPORT == 1
//eof zclMemoryManager.c
