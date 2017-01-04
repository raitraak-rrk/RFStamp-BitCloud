/******************************************************************************
  \file machwiTransactionQueue.h

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      25/06/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWITRANSACTIONQUEUE_H
#define _MACHWITRANSACTIONQUEUE_H
#ifdef _FFD_

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysTypes.h>
#include <macData.h>
#include <macFrame.h>
#include <macAssociate.h>
#include <macDisassociate.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
// Transaction event.
typedef enum
{
  MAC_TRANSACTION_EXPIRED_EVENT = MAC_TRANSACTION_EXPIRED_STATUS,
  MAC_TRANSACTION_ACTIVATED_EVENT
} MachwiTransactionEvent_t;

// Transaction.
typedef union
{
  MAC_DataReq_t         dataReq;
  MAC_AssociateResp_t   associateResp;
  MAC_DisassociateReq_t disassociateReq;
} MachwiTransaction_t;

/******************************************************************************
                    Constants section
******************************************************************************/

/******************************************************************************
                    External variables
******************************************************************************/

/******************************************************************************
                    Prototypes section
******************************************************************************/
/******************************************************************************
  TBD.
******************************************************************************/
void machwiPutTransactionToQueue(MachwiTransaction_t *transaction);

/******************************************************************************
  TBD.
******************************************************************************/
result_t machwiPurgeTransactionFromQueue(uint8_t msduHandle);

/**************************************************************************//**
 \brief Compares MAC Data Request command's parameters with transactions in the
  transaction queue. If matched - transaction is marked as "activated".

 \param frameDescr - Data Request MAC command descriptor.
 \return true if transaction to be transmitted is found, false otherwise
******************************************************************************/
bool machwiTestAndKickQueueTransactions(MAC_FrameDescr_t *frameDescr);

/**************************************************************************//**
 \brief Finds trunsactions which were already activated.

  \return Activated transaction pointer if found, NULL otherwise.
******************************************************************************/
MachwiTransaction_t *machwiGetActivatedTransactionFromQueue(void);

/******************************************************************************
  TBD.
******************************************************************************/
uint8_t* machwiFillPendingAddrSpecAndList(uint8_t *beaconPayload);

/******************************************************************************
  Resets transaction queue.
  Parameters:
    none.
  Returns:
    none.
******************************************************************************/
void machwiResetTransactionQueue(void);

/******************************************************************************
 Deletes transaction from queue.
 Parameters:
   transaction - pointer to transaction wich should be deleted.
 Returns:
  none.
******************************************************************************/
void machwiDeleteTransactionFromQueue(MachwiTransaction_t *transaction);

/******************************************************************************
 Checks if MAC transaction queue is empty
 Parameters:
   none
 Returns:
  true, if MAC transaction queue is empty, false otherwise.
******************************************************************************/
bool machwiTransactionQueueEmpty(void);

#endif //_FFD_
#endif /* _MACHWITRANSACTIONQUEUE_H */

// eof machwiTransactionQueue.h
