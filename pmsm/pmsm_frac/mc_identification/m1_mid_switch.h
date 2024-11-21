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
#ifndef _M1_MID_SWITCH_H_
#define _M1_MID_SWITCH_H_

#ifdef PMSM_SNSLESS_ENC
#include "m1_sm_snsless_enc.h"
#define MC_TEMP 1
#else
#ifdef PMSM_SNSLESS
#include "m1_sm_snsless.h"
#define MC_TEMP 1
#endif
#endif

#ifdef MID_EN
#include "mid_sm_states.h"
#define MID_TEMP 1
#endif

#if (MC_TEMP == 1 && MID_TEMP == 1)
#define M1_MID_SWITCH_SUPPORT
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* M1/MID switch request enumeration. */
typedef enum _m1_mid_request_t{
    kNoRequest  = 0U,
    kM1toMID    = 1U,
    kMIDtoM1    = 2U,
} m1_mid_request_t;

/* M1/MID application state enumeration. */
typedef enum _m1_mid_state_t{
    kStateM1     = 0U,
    kStateMID    = 1U,
} m1_mid_state_t;

/* M1/MID switch fault enumeration. */
typedef enum _m1_mid_fault_t{
    kNoFault         = 0U,
    kM1toMIDFault    = 1U,
    kMIDtoM1Fault    = 2U,
} m1_mid_fault_t;

/* M/MID switching structure. */
typedef struct _m1_mid_struct_t
{
    m1_mid_request_t eRequest;
    m1_mid_state_t   eAppState;
    m1_mid_fault_t   eFaultSwicth;
} m1_mid_struct_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern m1_mid_struct_t g_sM1toMID;

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
* @brief  Check M1/MID switch request.
*
* @param  void
*
* @return none
*/
void M1_MID_Switch_BL(void);

/*!
* @brief  Get actual application state (M1 or MID).
*
* @param  void
*
* @return Actual application state
*/
uint16_t M1_MID_Get_State(void);

#ifdef __cplusplus
}
#endif

#endif /* M1_MID_SWITCH */
