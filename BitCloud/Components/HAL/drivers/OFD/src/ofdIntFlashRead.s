/**************************************************************************//**
  \file  ofdIntFlashRead.s

  \brief Implementation of internal flash reading.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      13/08/09 A. Khromykh - Created
*******************************************************************************/

/*******************************************************************************
Reads byte from internal flash
Parameters:
  R25:R24:R23:R22 - Byte address into flash.
Returns:
  R24 - read byte from flash.
*******************************************************************************/
.section .text
.global ofdReadInternalFlash
ofdReadInternalFlash:
    push  r31                                    ; Store Z to stack
    push  r30                                    ; Store Z to stack
    in    r25,  0x3B                             ; Save RAMPZ.
    out	  0x3B, r24                              ; Load RAMPZ with the MSB of the address.
    movw  r30,  r22                              ; Move low bytes of address to ZH:ZL from R23:R22
    elpm  r24,  Z                                ; Extended load program memory from Z address
    out	  0x3B, r25                              ; Restore RAMPZ register.
    pop   r30                                    ; Restore Z
    pop   r31                                    ; Restore Z
    ret                                          ; return from function

; eof ofdIntFlashRead.s

