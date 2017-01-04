/***************************************************************************//**
  \file zsiSerializer.c

  \brief ZAppSI serializer module implementation.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-07  A. Razinkov - Created.
   Last change:
    $Id: zsiSerializer.c 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiSerializer.h>
#include <macAddr.h>
#include <zsiDbg.h>

/******************************************************************************
                              Implementations section
******************************************************************************/
/******************************************************************************
  \brief uint8 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint8(ZsiSerializer_t *const serializer, uint8_t value)
{
  sysAssert(serializer->pow, ZSISERIALIZER_ZSISERIALIZEUINT80);
  *serializer->pow = value;
  serializer->pow++;
}

/******************************************************************************
  \brief uint16 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint16(ZsiSerializer_t *const serializer, uint16_t value)
{
  sysAssert(serializer->pow, ZSISERIALIZER_ZSISERIALIZEUINT160);
  ((u16Packed_t *)(serializer->pow))->val = CPU_TO_LE16(value);
  serializer->pow += sizeof(value);
}

/******************************************************************************
  \brief uint32 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint32(ZsiSerializer_t *const serializer,
  const uint32_t *const value)
{
  sysAssert(serializer->pow, ZSISERIALIZER_ZSISERIALIZEUINT320);
  ((u32Packed_t *)(serializer->pow))->val = CPU_TO_LE32(*value);
  serializer->pow += sizeof(value);
}

/******************************************************************************
  \brief uint64 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint64(ZsiSerializer_t *const serializer,
  const uint64_t *const value)
{
  sysAssert(serializer->pow, ZSISERIALIZER_ZSISERIALIZEUINT640);
  COPY_64BIT_VALUE(((u64Packed_t *)(serializer->pow))->val, CPU_TO_LE64(*value));
  serializer->pow += sizeof(*value);
}

/******************************************************************************
  \brief Data block serialization routine.

  \param[in] serializer - serializer context.
  \param[in] data - pointer to data block.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiSerializeData(ZsiSerializer_t *const serializer, const void *const data,
  uint16_t size)
{
  sysAssert(serializer->pow && data && size, ZSISERIALIZER_ZSISERIALIZEDATA0);
  memcpy(serializer->pow, data, size);
  serializer->pow += size;
}

/******************************************************************************
  \brief uint8 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint8(ZsiSerializer_t *const serializer, uint8_t *const pow)
{
  sysAssert(pow && serializer->por, ZSISERIALIZER_ZSIDESERIALIZEUINT80);
  *pow = *serializer->por;
  serializer->por++;
}

/******************************************************************************
  \brief uint16 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint16(ZsiSerializer_t *const serializer, uint16_t *const pow)
{
  sysAssert(pow && serializer->por, ZSISERIALIZER_ZSIDESERIALIZEUINT160);
  *pow = LE16_TO_CPU(((u16Packed_t *)(serializer->por))->val);
  serializer->por += sizeof(*pow);
}

/******************************************************************************
  \brief uint32 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint32(ZsiSerializer_t *const serializer, uint32_t *const pow)
{
  sysAssert(pow && serializer->por, ZSISERIALIZER_ZSIDESERIALIZEUINT320);
  *pow = LE32_TO_CPU(((u32Packed_t *)(serializer->por))->val);
  serializer->por += sizeof(*pow);
}

/******************************************************************************
  \brief uint64 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint64(ZsiSerializer_t *const serializer, uint64_t *const pow)
{
  sysAssert(pow && serializer->por, ZSISERIALIZER_ZSIDESERIALIZEUINT640);
  COPY_64BIT_VALUE(*pow, LE64_TO_CPU(((u64Packed_t *)(serializer->por))->val));
  serializer->por += sizeof(*pow);
}

/******************************************************************************
  \brief Data block deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiDeserializeData(ZsiSerializer_t *const serializer, void *const pow,
  uint16_t size)
{
  sysAssert(pow && serializer->por && size, ZSISERIALIZER_ZSIDESERIALIZEDATA0);
  memcpy(pow, serializer->por, size);
  serializer->por += size;
}

/******************************************************************************
  \brief Returns a pointer to data block in serialized payload.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were pointer should be stored.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiDeserializeToPointer(ZsiSerializer_t *const serializer, uint8_t **const pow,
  uint16_t size)
{
  sysAssert(pow && serializer->por && size, ZSISERIALIZER_ZSIDESERIALIZEDATA0);
  *pow = (void *)serializer->por;
  serializer->por += size;
}

/* eof zsiSerializer.c */
