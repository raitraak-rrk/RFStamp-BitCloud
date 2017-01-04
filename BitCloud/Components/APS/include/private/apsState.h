/**************************************************************************//**
  \file  apsState.h

  \brief Set and check APS state.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2015, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
   History:
    2010-10-09 Max Gekk - Created.
   Last change:
    $Id: apsState.h 27584 2015-01-09 14:45:42Z unithra.c $
 ******************************************************************************/
#if !defined _APS_STATE_H
#define _APS_STATE_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <sysTypes.h>

/******************************************************************************
                                 Types section
 ******************************************************************************/
/** Identifiers of APS states. */
typedef uint16_t ApsStateId_t;

#define APS_INITIAL_STATE_ID    0x100
#define APS_RESETTING_STATE_ID  0x200
#define APS_RESET_STATE_ID      0x300
#define APS_START_STATE_ID      0x400
#define APS_STOPPING_STATE_ID   0x500
#define APS_STOP_STATE_ID       0x600
#define APS_HOT_RESETTING_STATE_ID  0x700

/** Identifiers of APS components. */
typedef uint8_t ApsComponentId_t;

#define APS_CONTROL_MANAGER_ID   0x00
#define APS_DATA_MANAGER_ID      0x01
#define APS_SECURITY_MANAGER_ID  0x02
#define APS_BIND_MANAGER_ID      0x03
#define APS_AIB_MANAGER_ID       0x04
#define APS_KEY_ESTABLISHMENT_ID 0x05
#define APS_MAX_COMPONENT_ID     6

/** Identifier masks of APS components. */
typedef ApsStateId_t ApsComponent_t;

#define APS_DENIED_ACCESS       0U
#define APS_CONTROL_MANAGER    (1U << APS_CONTROL_MANAGER_ID)
#define APS_DATA_MANAGER       (1U << APS_DATA_MANAGER_ID)
#define APS_SECURITY_MANAGER   (1U << APS_SECURITY_MANAGER_ID)
#define APS_BIND_MANAGER       (1U << APS_BIND_MANAGER_ID)
#define APS_AIB_MANAGER        (1U << APS_AIB_MANAGER_ID)
#define APS_KEY_ESTABLISHMENT  (1U << APS_KEY_ESTABLISHMENT_ID)

/** APS component's permitions. */
typedef ApsComponent_t ApsPermitions_t;

#define APS_INITIAL_STATE_PERM   APS_AIB_MANAGER
#define APS_RESETING_STATE_PERM  APS_DENIED_ACCESS
#define APS_RESET_STATE_PERM     (APS_AIB_MANAGER | APS_KEY_ESTABLISHMENT | APS_DATA_MANAGER)
#define APS_START_STATE_PERM \
  (APS_CONTROL_MANAGER | APS_AIB_MANAGER  | APS_SECURITY_MANAGER \
   | APS_DATA_MANAGER  | APS_BIND_MANAGER | APS_KEY_ESTABLISHMENT)
#define APS_STOPPING_STATE_PERM  APS_DENIED_ACCESS
#define APS_STOP_STATE_PERM      APS_AIB_MANAGER


/** Global states of APS layer. */
typedef ApsStateId_t ApsState_t;

#define APS_NO_INIT_STATE    0U
#define APS_INITIAL_STATE    (APS_INITIAL_STATE_ID   | APS_INITIAL_STATE_PERM)
#define APS_RESETTING_STATE  (APS_RESETTING_STATE_ID | APS_RESETING_STATE_PERM)
#define APS_RESET_STATE      (APS_RESET_STATE_ID     | APS_RESET_STATE_PERM)
#define APS_START_STATE      (APS_START_STATE_ID     | APS_START_STATE_PERM)
#define APS_STOPPING_STATE   (APS_STOPPING_STATE_ID  | APS_STOPPING_STATE_PERM)
#define APS_STOP_STATE       (APS_STOP_STATE_ID      | APS_STOP_STATE_PERM)
#define APS_HOT_RESETTING_STATE \
  (APS_HOT_RESETTING_STATE_ID | APS_RESETING_STATE_PERM)
#define IS_VALID_APS_STATE(state) \
  ((APS_INITIAL_STATE <= (state)) && ((state) <= APS_HOT_RESETTING_STATE))

/******************************************************************************
                          Global variables section
 ******************************************************************************/
/** Current state of APS layer. */
extern ApsState_t apsState;

/******************************************************************************
                          Inline functions section
 *****************************************************************************/
/*****************************************************************************
  \brief Peforms the setting of APS current state to specified state.

  \param[in] newState - state will be set.
  \return None.
 *****************************************************************************/
INLINE void apsSetState(const ApsState_t newState)
{
  apsState = newState;
}

/*****************************************************************************
  \brief Returns the APS State Machine current state.
 *****************************************************************************/
INLINE ApsState_t apsGetState(void)
{
  return apsState;
}
/*****************************************************************************
  \brief  The function performs checking the certain layer component
         availability in depending on the APS State Machine.

  \param[in] component - identifier mask of APS component.
  \return true  - component specified is available,
          false - component specified is unavailable.
 *****************************************************************************/
INLINE bool apsCheckState(const ApsComponent_t component)
{
  return APS_DENIED_ACCESS != (apsState & component);
}

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/*****************************************************************************
  \brief Main task handler of APS State Machine.
 *****************************************************************************/
APS_PRIVATE void apsStateMachineTaskHandler(void);

/******************************************************************************
  \brief Check stop procedure complete.

  Checking all APS components are stopped and peform APS_StopConf if it is true.

  \param[in] component - already stopped APS component.

  \return None.
 ******************************************************************************/
APS_PRIVATE void apsCompleteStop(const ApsComponent_t component);

#endif /* _APS_STATE_H */
/** eof apsState.h */

