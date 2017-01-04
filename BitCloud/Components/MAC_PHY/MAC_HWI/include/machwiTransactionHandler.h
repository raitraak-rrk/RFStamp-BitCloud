/******************************************************************************
  \file machwiTransactionHandler.h

  \brief machwiTransactionHandler header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/06/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWITRANSACTIONHANDLER_H
#define _MACHWITRANSACTIONHANDLER_H
#ifdef _FFD_

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <sysTypes.h>
#include <machwiTransactionQueue.h>

/******************************************************************************
                    Types section
******************************************************************************/
typedef enum
{
  MACHWI_IDLE_TRANSACTION_HANDLER_STATE,
  MACHWI_BUSY_ASSOCIATE_TRANSACTION_HANDLER_STATE,
#ifdef _MAC_DISASSOCIATE_

  MACHWI_BUSY_DISASSOCIATE_TRANSACTION_HANDLER_STATE,
#endif //_MAC_DISASSOCIATE_

  MACHWI_BUSY_DATA_TRANSACTION_HANDLER_STATE,
} MachwiTransactionHandlerState_t;

/******************************************************************************
                        Prototypes section.
******************************************************************************/

/******************************************************************************
  Indicates, that transaction event happened (it was requested or just expired).
  Parameters:
    transaction - transaction descriptor.
    event - type of the event (MAC_TRANSACTION_EXPIRED_EVENT or
            MAC_TRANSACTION_READY_EVENT).
  Returns:
    true - activate the kicked transaction, false - otherwise.
******************************************************************************/
bool machwiSendTransactionEvent(MachwiTransaction_t *transaction, MachwiTransactionEvent_t event);


/******************************************************************************
  Resets transaction handler.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetTransactionHandler(void);

#endif //_FFD_
#endif //_MACHWITRANSACTIONHANDLER_H

// eof machwiTransactionHandler.h
