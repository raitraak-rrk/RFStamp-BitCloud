/************************************************************************//**
  \file machwiTransactionHash.h

  \brief
    Header for transaction hash table for fast lookup of pending data

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
    27.03.12 D. Loskutnikov - Created.
******************************************************************************/

#ifndef _MACHWI_TRANSACTION_HASH_H
#define _MACHWI_TRANSACTION_HASH_H

#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)

/*****************************************************************************
                              External variables section
******************************************************************************/
extern uint8_t pendingTransactionsHash[32]; // 256 bits

/*****************************************************************************
                              Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Insert new transaction to the hash table

\param[in] trans - transaction
******************************************************************************/
void machwiTransctionHashInsert(MachwiTransaction_t *trans);

/**************************************************************************//**
\brief Remove transaction from hash table
 Note: there is an assumption that transaction is removed from queue already

\param[in] trans - transaction
******************************************************************************/
void machwiTransactionHashRemove(MachwiTransaction_t *trans);

/**************************************************************************//**
\brief Perform slow (and always correct) search of transaction for
 given address

\param[in] addr - address
\param[in] addrMode - address mode
\return result
******************************************************************************/
bool machwiIsDataPending(const MAC_Addr_t *addr, MAC_AddrMode_t addrMode);

/**************************************************************************//**
\brief Removes all entries from transaction hash
******************************************************************************/
void machwiTransactionHashClear(void);

/*****************************************************************************
                              Inline static functions section
******************************************************************************/
/**************************************************************************//**
\brief Calculate hash for supplied address. Simple sum of bytes may be
  replaced with a more advanced hasher in the future.

\param[in] addr - address
\param[in] addrMode - address mode
\return hash
******************************************************************************/
INLINE uint8_t machwiCalculateAddressHash(const MAC_Addr_t *addr, MAC_AddrMode_t addrMode)
{
  if (addrMode == MAC_SHORT_ADDR_MODE)
  {
    return (addr->sh >> 8) + (addr->sh & 0xFF);
  }
  else
  {
    const uint8_t *p = (const uint8_t*)&addr->ext;
    return p[0] + p[1] + p[2] + p[3] + p[4] + p[5] + p[6] + p[7];
  }
}

/**************************************************************************//**
\brief Perform fast lookup if no transaction is pending for given address.
  There could be no false positives, but may be false negatives.

\param[in] addr - address
\param[in] addrMode - address mode
\return result
******************************************************************************/
INLINE bool machwiIsNoDataPending(const MAC_Addr_t *addr, MAC_AddrMode_t addrMode)
{
  const uint8_t hash = machwiCalculateAddressHash(addr, addrMode);
  return !(pendingTransactionsHash[hash / 8] & (1 << (hash % 8)));
}


#endif // defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
#endif // _MACHWI_TRANSACTION_HASH_H
//eof machwiTransactionHash.h
