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

#ifndef _MID_SM_STATES_H_
#define _MID_SM_STATES_H_

#include "mid_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Pp Assist config params structure. */
typedef struct _pp_assist_cfg_params_t
{
    frac16_t f16IdReqOpenLoop;   /* Openloop current [A]. */
    frac16_t f16SpeedElReq;      /* Required electrical speed [rpm]. */
    /* Optional (advanced) parameters */
    uint16_t u16RampTime;        /* Frequency ramp time [ms]. */
    uint16_t u16ZeroPosTime;     /* Steady position time [ms]. */
    frac16_t f16DutyCycleLimit;  /* Maximum allowable duty cycle in frac [-]. */
    acc32_t  a32DPiPropGain;     /* Proportional gain of the D-axis current loop controller [-]. */
    acc32_t  a32DPiIntegGain;    /* Integral gain of the D-axis current loop controller [-]. */
    acc32_t  a32QPiPropGain;     /* Proportional gain of the Q-axis current loop controller [-]. */
    acc32_t  a32QPiIntegGain;    /* Integral gain of the Q-axis current loop controller [-]. */
    acc32_t  a32SpeedIntegGain;  /* Gain of speed integrator [-]. */
}pp_assist_cfg_params_t;

/* RL Estim config params structure. */
typedef struct _rl_estim_cfg_params_t
{
    frac32_t f32IDcMeas;           /* Scaled measurement DC current [A]. */
    frac32_t f32IDcPosMax;         /* Scaled maximum DC current [A]. */
    frac32_t f32IDcNegMax;         /* Scaled maximum allowed negative d-axis DC current [A]. The value of f32IDcNegMax must be negative or zero. */
    frac32_t f32IDcLd;             /* Scaled DC current used for Ld measurement [A]. */
    frac32_t f32IDcLq;             /* Scaled DC current used for Lq measurement [A]. */
}rl_estim_cfg_params_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief User control variables: */

/* MID measurement type selection user variable. */
extern mid_meas_type_t         eUserMIDMeasType;

/* MID known motor parameters set by user structure. */
extern mid_motor_params_user_t sUserMIDMotorParamsKnown;
/* MID measured motor parameters structure. */
extern mid_motor_params_user_t sUserMIDMotorParamsMeas;

/* MID measurement status user variable. */
extern mid_status_t            sUserMIDStatus;

/* Control motor during MID */
extern mid_pmsm_t              g_sMidDrive;

extern volatile float g_fltMIDDCBvoltageScale;
extern volatile float g_fltMIDcurrentScale;
extern volatile float g_fltMIDparamScale;
extern volatile float g_fltMIDspeedScale;

/* Pp Assist variables */
extern MCAA_PPASSIST_T_F16    g_sPpAssistStruct;
extern pp_assist_cfg_params_t g_sPpAssistInitFMSTR;

/* EstimRL variables */
extern MCAA_ESTIMRL_T_F32 g_sEstimRLStruct;
extern MCAA_ESTIMRL_RUN_T_F32 g_sEstimRLCtrlRun;
extern rl_estim_cfg_params_t g_sEstimRLInitFMSTR;
extern frac32_t f32IDcPlot;
extern frac32_t f32LdPlot;
extern frac32_t f32LqPlot;
extern uint8_t u8ModeEstimRL;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief  MID after-reset initialization function.
 *
 * @param  void
 *
 * @return none
 */
void MID_Init_AR(void);

/*!
 * @brief  MID fast-loop process function.
 *
 * @param  void
 *
 * @return none
 */
void MID_ProcessFast_FL(void);

/*!
 * @brief   MID measurement start function.
 *
 * @param   eMeasurementType - measurement type that will be done.
 *
 * @return  none
 */
RAM_FUNC_LIB
void MID_Start_BL(mid_meas_type_t eMeasurementType);

/*!
 * @brief  MID measurement stop function.
 *
 * @param  void
 *
 * @return none
 */
RAM_FUNC_LIB
void MID_Stop_BL(void);

/*!
 * @brief  Functions returns MID status.
 *
 * @param  psMIDStatus - Pointer to the MID status structure, which is updated
 *                       during the function call.
 *
 * @retval TRUE  - Measurement is ongoing.
 * @retval FALSE - MID is idle.
 */
RAM_FUNC_LIB
bool_t MID_GetStatus_BL(mid_status_t *psMIDStatus);

/*!
 * @brief   Function sets known machine parameters.
 *
 * @details The function can be called to provide known machine parameters prior
 *          measurement start.
 *
 * @note    This function MUST be called at least to set the non-zero number of
 *          pole-pairs.
 *          Providing correct parameter does not affect the electrical and
 *          mechanical parameter scheduling.
 *
 * @param   sMotorParams - Pointer to motor parameters provided by the user.
 *
 * @return  None
 */
RAM_FUNC_LIB
void MID_SetKnownMotorParams_BL(mid_motor_params_user_t *psMotorParams);

/*!
 * @brief   Function gets measured and/or known machine parameters.
 *
 * @param   sMotorParams - Pointer to the motor parameters structure destination.
 *
 * @return  None
 */
RAM_FUNC_LIB
void MID_GetMotorParams_BL(mid_motor_params_user_t *psMotorParams);

/*!
 * @brief   Function starts or stops MID.
 *
 * @param   pMidCmd - Pointer to commnad for start/stop MID.
 *
 * @return  None
 */
RAM_FUNC_LIB
void MID_Process_BL(mid_app_cmd_t *pMidCmd);

/*!
 * @brief   Function returns actual MID state.
 *
 * @param   None
 *
 * @return  Actual MID state
 */
uint16_t MID_GetActualState(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MID_SM_STATES_ */
