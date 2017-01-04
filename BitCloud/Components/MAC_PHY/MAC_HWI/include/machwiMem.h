/******************************************************************************
  \file machwiMem.h

  \brief MACHWI variable's memory description.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      03/11/07 A. Mandychev - Created.
******************************************************************************/

#ifndef _MACHWIMEM_H
#define _MACHWIMEM_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <sysQueue.h>
#include <appTimer.h>
#include <machwiManager.h>
#include <machwiHwdReqMemPool.h>
#include <machwiAssociateHandler.h>
#include <machwiDisassociateHandler.h>
#include <machwiScanHandler.h>
#include <machwiStartHandler.h>
#include <machwiTransactionHandler.h>
#include <machwiPollHandler.h>
#include <macBanNode.h>

/******************************************************************************
                    Define(s) section
******************************************************************************/

/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{
  union
  {
    MachwiAssociateState_t    assoc;
#ifdef _MAC_DISASSOCIATE_
    MachwiDisassociateState_t disassoc;
#endif // _MAC_DISASSOCIATE_
    MachwiScanState_t         scan;
#ifdef _FFD_
    MachwiStartState_t        start;
#endif // _FFD_
  } state;
  MachwiState_t             managerState; // For the manager needs.
  MachwiPollHandlerState_t  pollState; // For the poll handler needs.
  void                      *machwiReq; // Used by all modules which need to save requests' parameters.
  Timer_t                   comAppTimer; // Used by all modules except machwiTransactionQueue.
  QueueDescriptor_t         reqQueueDescr;
  bool                      txCoordRealigCmd; // For the start handler needs.
  MAC_DisassociateInd_t     disassociateInd; // For the disassociate handler needs.
  MachwiHwdReqDescr_t       machwdReq;
#ifdef _FFD_
  QueueDescriptor_t         transactionQueueDescr; // For the transaction queue needs - transaction queue descriptor.
  HAL_AppTimer_t            transTimer; // For the transaction queue needs - transaction queue timer.
  MachwiTransactionHandlerState_t transactionHandlerState; // For the transaction handler needs.
  uint8_t                   activatedTransactionsCounter;
  MachwiTransaction_t       *currentTransaction;
#endif //_FFD_
  bool                      switchIsLocked; // For the switching Rx Control needs.
  void                      (*sendSwitchConfirm)(void);
  uint8_t                   nextChannel; // For the scan handler needs.
  uint8_t                   postponedAction; // MAC postponed actions bit field.
#ifdef _MAC_BAN_NODE_
  MAC_BanTable_t            banTable;
#endif /* _MAC_BAN_NODE_ */
#ifdef _FFD_
#ifdef _TEST_EMPTY_DATA_FEATURE_
  HAL_AppTimer_t            emptyDataDelayTimer;
#endif /*_TEST_EMPTY_DATA_FEATURE_*/
#endif /*_FFD_*/
} MachwiMem_t;

#endif /* _MACHWIMEM_H */

// eof machwiMem.h
