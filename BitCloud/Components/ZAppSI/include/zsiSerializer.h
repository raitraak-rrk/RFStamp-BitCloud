/******************************************************************************
  \file zsiSerializer.h

  \brief
    ZAppSI serializer module interface.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2010 , Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-09-07  A. Razinkov - Created.
   Last change:
    $Id: zsiSerializer.h 24441 2013-02-07 12:02:29Z akhromykh $
******************************************************************************/

#ifndef _ZSISERIALIZER_H_
#define _ZSISERIALIZER_H_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct _ZsiSerializer_t
{
  uint8_t *pow;
  const uint8_t *por;
} ZsiSerializer_t;

/******************************************************************************
                    Defines section
******************************************************************************/
#define zsiSerializeBitfield zsiSerializeUint8
#define zsiDeserializeBitfield zsiDeserializeUint8

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  \brief uint8 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint8(ZsiSerializer_t *const serializer, uint8_t value);

/******************************************************************************
  \brief uint16 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint16(ZsiSerializer_t *const serializer, uint16_t value);

/******************************************************************************
  \brief uint32 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint32(ZsiSerializer_t *const serializer,
  const uint32_t *const value);

/******************************************************************************
  \brief uint64 value serialization routine.

  \param[in] serializer - serializer context.
  \param[in] value - value to serialize.

  \return None.
 ******************************************************************************/
void zsiSerializeUint64(ZsiSerializer_t *const serializer,
  const uint64_t *const value);

/******************************************************************************
  \brief Data block serialization routine.

  \param[in] serializer - serializer context.
  \param[in] data - pointer to data block.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiSerializeData(ZsiSerializer_t *const serializer, const void *const data,
  uint16_t size);

/******************************************************************************
  \brief uint8 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint8(ZsiSerializer_t *const serializer, uint8_t *const pow);

/******************************************************************************
  \brief uint16 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint16(ZsiSerializer_t *const serializer, uint16_t *const pow);

/******************************************************************************
  \brief uint32 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint32(ZsiSerializer_t *const serializer, uint32_t *const pow);

/******************************************************************************
  \brief uint64 value deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.

  \return None.
 ******************************************************************************/
void zsiDeserializeUint64(ZsiSerializer_t *const serializer, uint64_t *const pow);

/******************************************************************************
  \brief Data block deserialization routine.

  \param[in] serializer - serializer context.
  \param[out] pow - pointer to region were deserialized data should be stored.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiDeserializeData(ZsiSerializer_t *const serializer, void *const pow,
  uint16_t size);

/******************************************************************************
  \brief Returns a pointer to data block in serialized payload.

  \param[in] serializer - serializer context.
  \param[out] pow - region were pointer should be stored.
  \param[out] size - data block size.

  \return None.
 ******************************************************************************/
void zsiDeserializeToPointer(ZsiSerializer_t *const serializer, uint8_t **const pow,
  uint16_t size);

#endif /* _ZSISERIALIZER_H_ */
