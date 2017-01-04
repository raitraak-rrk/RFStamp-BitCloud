/**********************************************************************//**
  \file sysUtils.c

  \brief

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    24/07/09 D. Kasyanov - Created
**************************************************************************/

/******************************************************************************
                                   Include section
******************************************************************************/
#include <sysUtils.h>
#include <rfRandom.h>
#include <appTimer.h>
#include <macEnvironment.h>
#include <configServer.h>

/******************************************************************************
                        Definitions section
******************************************************************************/
#define SYS_UPDATE_RAND_SEED_TIMEOUT  1000u
#define SYS_VERSION_ZERO_ASCII                    48
#define SYS_VERSION_P_ASCII                       80
#define SYS_VERSION_M_ASCII                       77
#define SYS_VERSION_E_ASCII                       69
#define SYS_VERSION_QUALIFIER_COMBI_MAX           4
#define SYS_VERSION_CONV_INT_TO_ASCII(intver, asciiver)         \
    if (intver > 9)                                             \
    {                                                           \
      asciiver[i++] = SYS_VERSION_ZERO_ASCII+(intver/10);       \
      asciiver[i++] = SYS_VERSION_ZERO_ASCII+(intver%10);       \
    }                                                           \
    else                                                        \
    {                                                           \
      asciiver[i++] = SYS_VERSION_ZERO_ASCII+intver;            \
    }

/******************************************************************************
                                   Prototypes section
******************************************************************************/
static void sysUpdateRndSeedTimerFired(void);
static void sysRndConfirm(RF_RandomConf_t *conf);
static uint16_t sysPseudoRandomSeed(void);

/******************************************************************************
                    Static variables section
******************************************************************************/
static HAL_AppTimer_t sysRndSeedTimer =
{
  .mode = TIMER_ONE_SHOT_MODE,
  .interval = SYS_UPDATE_RAND_SEED_TIMEOUT,
  .callback = sysUpdateRndSeedTimerFired
};

static RF_RandomReq_t sysRfRndReq =
{
  .disableRx = true
};

/******************************************************************************
                                   Implementation section
******************************************************************************/
/******************************************************************************
  Performs swap bytes in array of length
  Parameters:
    array  - pointer to array.
    length  - array length
  Returns:
    None
****************************************************************************/
void SYS_Swap(uint8_t *array, uint8_t length)
{
  uint8_t *dst = array + length - 1;

  uint8_t tmp;
  length >>= 1;

  while (length--)
  {
    tmp = *array;
    *array = *dst;
    *dst = tmp;

    array++;
    dst--;
  }
}

/*************************************************************************//**
\brief Updates rnd with new random values according to time interval
*****************************************************************************/
static void sysUpdateRndSeedTimerFired(void)
{
  RF_RandomReq(&sysRfRndReq);
}

/**************************************************************************//**
\brief Confirm from random number generation updater

\param[in]
    conf - confirm parameters
******************************************************************************/
static void sysRndConfirm(RF_RandomConf_t *conf)
{
  if (conf->value)
    srand(conf->value);
  else
    srand(sysPseudoRandomSeed());

  sysRfRndReq.disableRx = true;
  HAL_StartAppTimer(&sysRndSeedTimer);
}

/**************************************************************************//**
\brief  Starts timer to update rand
******************************************************************************/
void sysStartUpdatingRandSeed(void)
{
  if (false == sysRfRndReq.disableRx)
    return; // random request has been started already

  HAL_StopAppTimer(&sysRndSeedTimer);
  sysRfRndReq.RF_RandomConf = sysRndConfirm;
  sysRfRndReq.disableRx = false;
  sysUpdateRndSeedTimerFired();
}

/**************************************************************************//**
\brief Calculates a pseudo random value for seed if HW random is not available.

\return:
    16-bit value which was calculated by using current time and other params.
******************************************************************************/
static uint16_t sysPseudoRandomSeed(void)
{
  uint16_t result = (uint16_t)HAL_GetSystemTime();
  const uint16_t *extAddr = (const uint16_t *)MAC_GetExtAddr();
  uint8_t i;

  for (i = 0; i < sizeof(ExtAddr_t)/sizeof(result); ++i)
    result ^= extAddr[i];

  return result;
}

/**************************************************************************//**
  \brief This function copies size bytes of random data into buffer.

  \param: buffer - This is an unsigned char array of size at least sz to hold
   the random data.
  \param: size - The number of bytes of random data to compute and store.

  \return:  0 Indicates successful completion.
******************************************************************************/
int SYS_GetRandomSequence(uint8_t *buffer, unsigned long size)
{
  uint16_t i;
  u16Packed_t *u16ptr = (u16Packed_t*)buffer;

  for(i = 0; i < size/2; i++, u16ptr++)
    u16ptr->val = SYS_GetRandomNumber();

  if (size & (1<<0))
    buffer[size-1] = (uint8_t)SYS_GetRandomNumber();

  return 0;
}

#if defined(_SLEEP_WHEN_IDLE_)
/**************************************************************************//**
  \brief Stops priodic updating of the random seed.
 ******************************************************************************/
void sysStopUpdatingRandSeed(void)
{
  HAL_StopAppTimer(&sysRndSeedTimer);
}
#endif /* _SLEEP_WHEN_IDLE_ */

#ifndef _MAC2_
/**************************************************************************//**
\brief This function reads version number in CS and returns as string

\param[out] strVersion - string version. intVersion - integer version

\return:  bool. true indicates success. false indicates failure,
          which means version is unavailable
******************************************************************************/
bool SYS_GetBitCloudRevision(uint8_t *strVersion, uint32_t *intVersion)
{
  typedef union {
    uint32_t stackVer;
    struct {
      uint8_t reserved:8;
      uint8_t qualifier:2;
      uint8_t branchIter:6;
      uint8_t branchId:4;
      uint8_t minorDerv:4;
      uint8_t minor:4;
      uint8_t major:4;
    } stackBits;
  } stackVersion_t;
  stackVersion_t stackVersion;
  uint8_t i = 0;
  uint8_t qual[4] = {SYS_VERSION_ZERO_ASCII, SYS_VERSION_P_ASCII, SYS_VERSION_M_ASCII, SYS_VERSION_E_ASCII};

  CS_ReadParameter(CS_STACK_VERSION_ID, &stackVersion.stackVer);

  if (!stackVersion.stackVer || (NULL == strVersion) || (NULL == intVersion))
  {
    if (intVersion)
      *intVersion = 0;
    if (strVersion)
      strVersion[0] = '\0';

    return false;
  }
  else
  {
    // convert to ascii
    strVersion[i++] = 'B';
    strVersion[i++] = 'C';
    strVersion[i++] = '_';
    strVersion[i++] = 'V';
    SYS_VERSION_CONV_INT_TO_ASCII(stackVersion.stackBits.major, strVersion);

    strVersion[i++] = '.';
    SYS_VERSION_CONV_INT_TO_ASCII(stackVersion.stackBits.minor, strVersion);

    strVersion[i++] = '.';
    SYS_VERSION_CONV_INT_TO_ASCII(stackVersion.stackBits.minorDerv, strVersion);

    strVersion[i++] = '_';
    SYS_VERSION_CONV_INT_TO_ASCII(stackVersion.stackBits.branchId, strVersion);

    strVersion[i++] = '.';
    SYS_VERSION_CONV_INT_TO_ASCII(stackVersion.stackBits.branchIter, strVersion);

    strVersion[i++] = qual[stackVersion.stackBits.qualifier];
    strVersion[i] = '\0';

    if (intVersion)
      *intVersion = stackVersion.stackVer;
  }

  return true;
}
#endif // _MAC2_

// eof sysUtils.c
