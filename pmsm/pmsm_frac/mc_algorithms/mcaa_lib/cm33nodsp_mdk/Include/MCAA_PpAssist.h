/*
* Copyright 2026 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may
* only be used strictly in accordance with the applicable license terms. 
* By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that
* you have read, and that you agree to comply with and are bound by,
* such license terms.  If you do not agree to be bound by the applicable
* license terms, then you may not retain, install, activate or otherwise
* use the software.
*
*/
/**
*
* @file       MCAA_PpAssist.h
*
* @version    1.0.0.0
*
* @date       20-March-2026
*
* @brief      Header file for mcaa_ppassist functions
*
******************************************************************************/
#ifndef MCAA_PP_ASSIST_H_
#define MCAA_PP_ASSIST_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Includes
*******************************************************************************/
#include "gflib.h"
#include "gmclib.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define MCAA_PpAssistInit_F16_C(psParam, psCtrl)               \
        MCAA_PpAssistInit_F16_FC(psParam, psCtrl)
#define MCAA_PpAssist_F16_C(f16UDcBus, psIAlBeFbck, psCtrl, psUAlBeReq) \
        MCAA_PpAssist_F16_FC(f16UDcBus, psIAlBeFbck, psCtrl, psUAlBeReq)
            
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* MCAA_PpAssistInit return value enum. */
typedef enum
{
  PPASSIST_RET_INIT_OK = 0,                    /* Initialization successful. */
  PPASSIST_RET_INIT_ERROR = 1,                 /* Invalid inputs. */
} MCAA_PPASSIST_INIT_RET_T_F16;

/* MCAA_PpAssist return value enum. */
typedef enum
{
  PPASSIST_RET_IN_PROGRESS = 0,                /* Assistant is in progress. The MCAA_PpAssist must be called again in the next sampling period. */
  PPASSIST_RET_DONE = 1,                       /* Assistant has finished (user passed determined number of pole-pairs to the assistant's structure). */
  PPASSIST_RET_ERROR = 2,                      /* Assistant has failed. */
} MCAA_PPASSIST_RET_T_F16;

/* Pp assistant fault enum, each bit of fault variable represents defined fault. */
typedef uint8_t ppassistdef_fault_t; 
typedef enum
{
  PPASSIST_FAULT_INIT_MISSING_I_D = 0,         /* Missing openloop d-axis current */
  PPASSIST_FAULT_INIT_MISSING_F_EL,            /* Missing required electrical speed */
  PPASSIST_FAULT_INIT_MISSING_FS,              /* Missing sampling frequency */
  PPASSIST_FAULT_RUN_UNINITIALIZED,            /* Invalid usage, must call init first */
} MCAA_PPASSIST_FAULT_T_F16;

/* Pp assistant internal state enum. */
typedef enum
{
  PPASSIST_STATE_UNINITIALIZED = 0,            /* PpAssistant is not initialized. */
  PPASSIST_STATE_ROTATE,                       /* Rotor rotating. */
  PPASSIST_STATE_DONE,                         /* Assistant finished. */
  PPASSIST_STATE_ERROR,                        /* Failure. */
} MCAA_PPASSIST_STATE_T_F16;

/* Pp assistant structure. */
typedef struct
{
  MCAA_PPASSIST_STATE_T_F16 pState;             /* Status of pole-pair assistant */
  ppassistdef_fault_t       pFault;             /* Fault of pole-pair assist */
  GFLIB_RAMP_T_F16          sSpeedElRampParam;   /* Ramp Up + Down coefficients for f16Speed. */
  GFLIB_INTEGRATOR_T_A32    sSpeedIntegrator;    /* Speed integrator coefficients. */
  frac16_t                  f16SpeedElReq;       /* Required Electrical Speed. */
  frac16_t                  f16IdReqOpenLoop;   /* Openloop current. */
  frac16_t                  f16SpeedElRamp;      /* Ramped f16SpeedElReq, this speed is integrated to get position. */
  uint16_t                  ui16PpDetermined;   /* Indicates whether the user already set pp in MCAT (true) or not yet (false). */
  uint16_t                  ui16WaitingSteady;  /* Indicates that motor is waiting in steady state (when electrical position is zero). */
  uint16_t                  ui16LoopCounter;    /* Serves for timing to determine e.g. 300ms. */
  frac16_t                  f16PosEl;           /* Rotor angular position */
  frac16_t                  f16PosElCurrent;    /* Current value of electrical position. */
  frac16_t                  f16PosElLast;       /* Last value of electrical position. */
  uint16_t                  ui16ZeroPosTime;    /* Time spent in zero position after overflow [s] */
  /* Current control */                       
  GMCLIB_2COOR_DQ_T_F16     pUDQReq;            /* Required DQ voltage */
  GFLIB_CTRL_PI_P_AW_T_A32  pPIpAWD;            /* D-axis ControllerPIpAW paremeters structure. */
  GFLIB_CTRL_PI_P_AW_T_A32  pPIpAWQ;            /* Q-axis ControllerPIpAW paremeters structure. */
  GMCLIB_2COOR_DQ_T_F16     pIDQReq;            /* Required DQ current structure. */
  GMCLIB_2COOR_DQ_T_F16     pIDQFbck;           /* Feedback DQ current structure. */
  GMCLIB_2COOR_SINCOS_T_F16 pThTransform;       /* Sine and cosine of rotor angular position */
  frac16_t                  f16DutyCycleLimit;  /* Maximum allowable duty cycle in frac [-] */
} MCAA_PPASSIST_T_F16;

/* Pp assistant init structure. */
typedef struct
{
  uint32_t  u32SamplingFreq;    /* Sampling frequency [1/s]. */
  frac16_t  f16IdReqOpenLoop;   /* Openloop current [A]. */
  frac16_t  f16SpeedElReq;      /* Required electrical speed [rpm]. */
  /* Optional (advanced) parameters */
  uint16_t  u16RampTime;        /* Frequency ramp time [ms]. */
  uint16_t  u16ZeroPosTime;     /* Steady position time [ms]. */
  frac16_t  f16DutyCycleLimit;  /* Maximum allowable duty cycle in frac [-]. */
  acc32_t   a32DPiPropGain;     /* Proportional gain of the D-axis current loop controller [-]. */
  acc32_t   a32DPiIntegGain;    /* Integral gain of the D-axis current loop controller [-]. */
  acc32_t   a32QPiPropGain;     /* Proportional gain of the Q-axis current loop controller [-]. */
  acc32_t   a32QPiIntegGain;    /* Integral gain of the Q-axis current loop controller [-]. */
  acc32_t   a32SpeedIntegGain;  /* Gain of speed integrator [-]. */
} MCAA_PPASSIST_INIT_T_F16;

/*******************************************************************************
 * Exported function prototypes
 ******************************************************************************/
extern MCAA_PPASSIST_INIT_RET_T_F16 MCAA_PpAssistInit_F16_FC(MCAA_PPASSIST_INIT_T_F16 *psParam,
                                                             MCAA_PPASSIST_T_F16 *const pcCtrl);
     
extern MCAA_PPASSIST_RET_T_F16 MCAA_PpAssist_F16_FC(frac16_t f16UDcBus, 
                                                    const GMCLIB_2COOR_ALBE_T_F16 *const pIAlBeFbck,
                                                    MCAA_PPASSIST_T_F16 *psCtrl,
                                                    GMCLIB_2COOR_ALBE_T_F16 *const pUAlBeReq);

/*******************************************************************************
* Inline functions
*******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MCAA_PP_ASSIST_H_ */
