/*
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2024 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may
* only be used strictly in accordance with the applicable license terms. 
* By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that
* you have read, and that you agree to comply with and are bound by,
* such license terms.  If you do not agree to be bound by the applicable
* license terms, then you may not retain, install, activate or otherwise
* use the software.
 */

#include "m1_mid_switch.h"
   
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
m1_mid_struct_t g_sM1toMID;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
   
/*!
 * MID fast-loop process function.
 */
void M1_MID_Switch_BL(void)
{
#ifdef M1_MID_SWITCH_SUPPORT
  switch (g_sM1toMID.eAppState)
  {
  case kStateM1:
    /* Spin state machine is running. */
    if(g_sM1toMID.eRequest == kM1toMID)
    {
      if(M1_GetAppState() == kSM_AppStop)
      {
        MID_Init();                             /* Call MID init routine. */
        g_sM1toMID.eFaultSwicth = kNoFault;     /* Clear fault flag. */
        g_sM1toMID.eAppState = kStateMID;       /* Switch application state to MID */
      }
      else
      {
        g_sM1toMID.eFaultSwicth = kM1toMIDFault; /* M1 is not in STOP state. */
      }    
    }    
    g_sM1toMID.eRequest = kNoRequest;            /* Always clear request. */
    break;
    
  case kStateMID:
    /* MID state machine is running. */
    if(g_sM1toMID.eRequest == kMIDtoM1)
    {
      if(MID_GetActualState() == kMID_Stop)
      {
        g_sM1Ctrl.eState = kSM_AppInit;         /* Set Init state for M1 state machine */
        g_sM1toMID.eFaultSwicth = kNoFault;     /* Clear fault flag. */
        g_sM1toMID.eAppState = kStateM1;       /* Switch application state to M1 */
      }
      else
      {
        g_sM1toMID.eFaultSwicth = kMIDtoM1Fault; /* MID is not in STOP state. */
      }  
    } 
    g_sM1toMID.eRequest = kNoRequest;            /* Always clear request. */
    break;
    
  default:
    ;
  }
#endif
}

/*!
 * Function returns actual state of the application.
 */
uint16_t M1_MID_Get_State(void)
{
  return ((uint16_t)g_sM1toMID.eAppState);
}
