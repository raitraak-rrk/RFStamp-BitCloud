/***************************************************************************//**
  \file zsiMem.c

  \brief ZAppSI memory.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2011-08-26  A. Razinkov - Created.
   Last change:
    $Id: zsiMem.c 24441 2013-02-07 12:02:29Z akhromykh $
 ******************************************************************************/

/******************************************************************************
                               Includes section
 ******************************************************************************/
#include <zsiMem.h>

/*****************************************************************************
                              Global variables section
******************************************************************************/
ZsiMem_t zsiMem =
{
  .taskManager = {
#if defined(ZAPPSI_HOST)
    .pendingTasks = (1U << ZSI_INIT_TASK_ID)
#else
    .pendingTasks = 0U
#endif /* ZAPPSI_HOST */
  }
};

/* eof zsiMem.c */
