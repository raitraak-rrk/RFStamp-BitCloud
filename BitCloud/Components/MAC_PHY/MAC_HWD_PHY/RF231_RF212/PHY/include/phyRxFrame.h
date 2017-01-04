/******************************************************************************
  \file phyRxFrame.h

  \brief Frame receive routines.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      26/05/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _PHYRXFRAME_H
#define _PHYRXFRAME_H

/******************************************************************************
                        Includes section.
******************************************************************************/
#include <phyStateMachine.h>
#include <phyRfSpiProto.h>
#include <at86rf2xx.h>
#include <phy.h>
#include <machwdRxBuffer.h>
#include <machwdRxFrameHandler.h>
#include <machwiTransactionHash.h>
#include <phyMemAccess.h>

/******************************************************************************
                        Definitions section.
******************************************************************************/
#define ADDITIONAL_FIELDS_SIZE 4    // FrameLength + LQI + RSSI + internal flag
#define DATA_REQ_CMD_ID        0x04

/******************************************************************************
                        Prototypes section.
******************************************************************************/
/******************************************************************************
  Finish frame receive operations.
  Returns: none.
******************************************************************************/
void PHY_DownloadFrame(uint8_t *frame);

/******************************************************************************
                 Inline static functions prototypes section.
******************************************************************************/
#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
/**************************************************************************//**
\brief Returns the address and address mode for data request frame.
\param[in, out] addr - is used to return address
\param[in, out] addrMode - is used to return address mode.
\return true if the frame is Data Request and it was parsed successfully.
******************************************************************************/
INLINE bool parseDataRequest(MAC_Addr_t *addr, uint8_t *addrMode)
{
  uint8_t lenMacFcf[3];                             // [0] - LEN; [1-2] - FCF [LSByte-MSByte]
  uint8_t addrOffset = 1 + 2 + 1 + sizeof(PanId_t); // LEN + FCF + SeqNum + PanID (atleast one)
  uint8_t cmdOffset;
  uint8_t cmd;
  MAC_Addr_t address;

  phyStartReadFrameInline();
  phyReadFrameInline(lenMacFcf, PHY_FRAME_LENGTH_FIELD_SIZE + 2);
  phyStopReadFrameInline();

  /* Parse LSB of Frame Control Field */
  if (MAC_CMD_FRAME_CODE != (lenMacFcf[1] & 0x03))       // Not a MAC command
    return false;

  if (!(lenMacFcf[1] & (1 << 6)))                        // Second PanID is present
    addrOffset += sizeof(PanId_t);

  /* Parse MSB of Frame Control Field */
  lenMacFcf[2] >>= 2;                                    // Move DstAddr to bit positions <1-0>
  if (MAC_SHORT_ADDR_MODE == (lenMacFcf[2] & 0x03))      // DstAddr is SHORT_ADDR (16-bit)
    addrOffset += sizeof(ShortAddr_t);                   // Now addrOffset points to source address
  else if (MAC_EXT_ADDR_MODE == (lenMacFcf[2] & 0x03))   // DstAddr is EXT_ADDR (64-bit)
    addrOffset += sizeof(ExtAddr_t);                     // Now addrOffset points to source address
  else if (MAC_RSRV_ADDR_MODE == (lenMacFcf[2] & 0x03))  // Let native handler take care of that
    return false;                                        // Otherwise it is MAC_NO_ADDR_MODE

  lenMacFcf[2] >>= 4;                                    // Move SrcAddr to bit positions <1-0>
  if (MAC_SHORT_ADDR_MODE == lenMacFcf[2])
    cmdOffset = addrOffset + sizeof(ShortAddr_t);        // Now cmdOffset points to command byte
  else if (MAC_EXT_ADDR_MODE == lenMacFcf[2])
    cmdOffset = addrOffset + sizeof(ExtAddr_t);          // Now cmdOffset points to command byte
  else
    return false;                                        // Other varaiants are impossible for MacDataReq

  /* Verify whether the command is MacDataReq */
  phyStartReadSramInline();
  phyReadSramInline(&cmd, cmdOffset, 1);
  phyStopReadSramInline();
  if (DATA_REQ_CMD_ID != cmd)
    return false;

  if (MAC_SHORT_ADDR_MODE == lenMacFcf[2])
  {
    phyStartReadSramInline();
    phyReadSramInline((uint8_t*)&address.sh, addrOffset, sizeof(ShortAddr_t));
    phyStopReadSramInline();
  }
  else if (MAC_EXT_ADDR_MODE == lenMacFcf[2])
  {
    phyStartReadSramInline();
    phyReadSramInline((uint8_t*)&address.ext, addrOffset, sizeof(ExtAddr_t));
    phyStopReadSramInline();
  }

  *addr = address;
  *addrMode = lenMacFcf[2];
  return true;
}
#endif

/******************************************************************************
  Initiates frame receive activities in extended RX_AACK_ON RF mode.
  Parameters: none.
  Returns: none.
******************************************************************************/
static inline void phyReceiveFrameInExtendedRfMode(void)
{
#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
  uint8_t tmpRegValue;
  MAC_Addr_t addr;
  uint8_t addrMode;
#endif
  FrameRxBufferType_t rxBufferType = FRAME_RX_BUFFER_TYPE_DATA;

  // To switch to PLL_ON after BUSY_RX_AACK state (deferred operation for RF chip).
  phyWriteRegisterInline(TRX_STATE_REG, TRX_CMD_PLL_ON);
  *phyMemRssi() = (int8_t) phyReadRegisterInline(PHY_ED_LEVEL_REG);

#if defined(_FFD_)
  rxBufferType = parseDataRequest(&addr, &addrMode) ? FRAME_RX_BUFFER_TYPE_DATA_REQ : FRAME_RX_BUFFER_TYPE_DATA;

#if defined(_CONTROL_FRAME_PENDING_BIT_)
  if (FRAME_RX_BUFFER_TYPE_DATA_REQ == rxBufferType && machwiIsNoDataPending(&addr, (MAC_AddrMode_t)addrMode))
  {
    *phyMemAckedWithoutPendBit() = true;
    // Clear frame pending bit within RX_AACK_ON for AT86RF233
    tmpRegValue = phyReadRegister(CSMA_SEED_1_REG);
    phyWriteRegister(CSMA_SEED_1_REG, (~REG_CSMA_SEED_1_AACK_SET_PD_MASK & tmpRegValue));
  }
  else
    *phyMemAckedWithoutPendBit() = false;
#endif
#endif

  phyStartReadFrameInline();

  // Read out length
  phyReadFrameInline((uint8_t *)phyMemRxByteCounter(), PHY_FRAME_LENGTH_FIELD_SIZE);

  if (machwdCheckBufferSpace(rxBufferType, *phyMemRxByteCounter() + ADDITIONAL_FIELDS_SIZE))
  { // Buffer was found.
#if defined(_FFD_)
    if (FRAME_RX_BUFFER_TYPE_DATA_REQ == rxBufferType)
      machwdSetHandlerId(MACHWD_DATA_REQ_IND_HNDLR_ID);
    else
#endif // defined(_FFD_)
    machwdSetHandlerId(MACHWD_RX_FRAME_IND_HNDLR_ID);
    SYS_PostTask(MAC_PHY_HWD_TASK_ID);
  }
  else
  { // There is no memory for the incomming frame.
    phyStopReadFrameInline();
    phySetState(PHY_IDLE_STATE);
    // Ack suppression.
    phyWriteRegisterInline(TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);
    phyWriteRegisterInline(TRX_STATE_REG, TRX_CMD_RX_AACK_ON);

#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
    // Restore frame pending bit within RX_AACK_ON for AT86RF233
    tmpRegValue = phyReadRegister(CSMA_SEED_1_REG);
    phyWriteRegister(CSMA_SEED_1_REG, (REG_CSMA_SEED_1_AACK_SET_PD_MASK | tmpRegValue));
#endif
  }
}

/******************************************************************************
  \brief On RX finished handling.
  \param None.
  \return None.
******************************************************************************/
static inline void phyRxFinishedHandler(void)
{
#if defined(_FFD_) && defined(_CONTROL_FRAME_PENDING_BIT_)
  // Set frame pending bit within RX_AACK_ON for AT86RF23
  uint8_t tmpRegValue = phyReadRegister(CSMA_SEED_1_REG);
  // Set the pending bit only only if it is cleared
  // Because it might be set on phyPollTrxStateHandler() before
  if (!(tmpRegValue & REG_CSMA_SEED_1_AACK_SET_PD_MASK))
    phyWriteRegister(CSMA_SEED_1_REG, (REG_CSMA_SEED_1_AACK_SET_PD_MASK | tmpRegValue));
#endif

  // To switch back to RX_AACK_ON after receive procedure.
  phyWriteRegisterInline(TRX_STATE_REG, TRX_CMD_RX_AACK_ON);
  phySetState(PHY_IDLE_STATE);
  // Send confirm to HWD layer
  machwdSetHandlerId(MACHWD_RX_FRAME_FINISHED_HNDLR_ID);
  SYS_PostTask(MAC_PHY_HWD_TASK_ID);
}

#endif /* _PHYRXFRAME_H */

// eof phyRxFrame.h
