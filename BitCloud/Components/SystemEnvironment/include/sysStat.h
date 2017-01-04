/**************************************************************************//**
\file  sysStat.h

\brief Collection of internal runtime data for report

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal
  History:
    15/04/11 D. Loskutnikov - Created
  Last Change:
    $Id: sysStat.h 27648 2015-02-24 05:42:55Z mukesh.basrani $
*******************************************************************************/
#ifndef _SYS_STAT_H
#define _SYS_STAT_H

/******************************************************************************
                   Define section
******************************************************************************/
#define SYS_STACK_CHECK_DISABLED (0xFFFFu)


#if  defined(_STATS_ENABLED_)
/******************************************************************************
                   Types section
******************************************************************************/
typedef struct _SYS_StatGenerator_t
{
  void *next;
  uint8_t (*gen)(uint8_t *buf, uint8_t maxSize);
} SYS_StatGenerator_t;

/******************************************************************************
                   Prototypes section
******************************************************************************/

/**************************************************************************//**
\brief Register new generator of stat data

\param[in] gen - generator
******************************************************************************/
void SYS_RegisterStatGenerator(SYS_StatGenerator_t *gen);

/**************************************************************************//**
\brief Collect stats from registered generators into the supplied buffer

\param[out] buf - buffer to place stat report
\param[in] maxSize - size limit of buffer
\return number of actually placed bytes
******************************************************************************/
uint8_t SYS_GenerateStatReport(uint8_t *buf, uint8_t maxSize);


/**************************************************************************//**
\brief Measures stack and returns

\param[out] None
\param[in] None
\return number of actually placed bytes
******************************************************************************/
uint16_t SYS_StatGetMinimumStackLeft(void);

/**************************************************************************//**
\brief Measures Current stack left and returns

\param[out] None
\param[in] None
\return  Remaining Stack left
******************************************************************************/

uint16_t Stat_GetCurrentStackLeft(void);

#endif // _STATS_ENABLED_

#endif // _SYS_STAT_H
// eof sysStat.h
