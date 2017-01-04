/******************************************************************************
  \file machwiPanIdConflictHandler.h

  \brief PAN ID coflict service routine header file.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      19/10/07 ALuzhetsky - Created.
******************************************************************************/

#ifndef _MACHWIPANIDCONFLICTHANDLER_H
#define _MACHWIPANIDCONFLICTHANDLER_H

/******************************************************************************
  Checks  accordance between PAN ID and coordinator addresses.
  Parameters:
    beaconDescr - beacon frame description.
  Return:
    none.
******************************************************************************/
void machwiCheckPanIdConflict(MAC_FrameDescr_t *beaconDescr);

/******************************************************************************
  Indicates that PAN iD conflict notification command is received.
  Parameters:
    frameDescr - pointer to the command's frame descriptor.
  Returns:
    none.
******************************************************************************/
void machwiPanIdConflictNotifInd(MAC_FrameDescr_t *beaconDescr);

#endif //_MACHWIPANIDCONFLICTHANDLER_H

// eof machwiPollHandler.h
