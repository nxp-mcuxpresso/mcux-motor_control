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

#ifndef _MID_DEF_H_
#define _MID_DEF_H_

#include "m1_pmsm_appconfig.h"
#include "mc_periph_init.h"

/* RTCESL fix libraries. */
#include "mlib.h"
#include "gflib.h"
#include "amclib.h"

#include "mcaa_lib.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* MID common parameters. */
#define I_NOMINAL 		    M1_I_MAX            /* AP MID - Nominal current [A]. */
#define N_NOMINAL         M1_N_MAX            /* AP MID - Nominal speed [rpm]. */
#define F_SAMPLING 		    M1_FAST_LOOP_FREQ   /* AP MID - Sampling frequency [Hz]. */

/* Maximal measuring signal levels. */
#define MID_I_MEAS_MAX    M1_I_MAX                      /* AP MID - Current sensing HW scale [A]. */
#define SCALE_I_F16(x)    (FRAC16(x / MID_I_MEAS_MAX))  /* AP MID - Fixed point current scale (frac16) */
#define SCALE_I_F32(x)    (FRAC32(x / MID_I_MEAS_MAX))  /* AP MID - Fixed point current scale (frac32) */
#define MID_N_MAX         M1_N_MAX                      /* AP MID - Speed scale [rpm] */
#define SCALE_N_F16(x)    (FRAC16(x / MID_N_MAX))       /* AP MID - Fixed point speed scale (frac16_t) */

/* Pp Assist measurement parameters. */
#define I_PP_ASSIST       I_NOMINAL  * 0.1F   /* PpAssist - Current for pole-pair assistant measurement [A] */
#define N_PP_ASSIST       N_NOMINAL  * 0.1F   /* PpAssist - Electrical Speed [rpm] */

/* EstimRL measurement parameters. */ 
#define NUM_MEAS 		      20U                 /* EstimRL - Number of measurement. */
#define I_RL_ESTIM        I_NOMINAL  * 0.5F   /* EstimRL - Current for electrical parameters measurement [A] */
#define I_POSMAX 		      I_NOMINAL  * 0.9F   /* EstimRL - Maximum positive current [A]. */
#define I_NEGMAX 		      -I_NOMINAL * 0.9F   /* EstimRL - Minimum positive current [A]. */
#define I_LD			        0.0F                /* EstimRL - Current to determine inductance in d-axis [A]. */
#define I_LQ			        I_NOMINAL  * 0.5F   /* EstimRL - Current to determine inductance in q-axis [A]. */
#define ESTIMRL_TIMEOUT   30.0F               /* EstimRL - Estimation timeout time [s]. */

/* MID measurements faults flags. */
#define MID_START_SUCCESSFUL  (0UL)
#define MID_PP_INIT_FAIL      (1UL << 0U)     /* Fault during PpAssist init */
#define MID_PP_MEAS_FAIL      (1UL << 1U)     /* Fault during PpAssist runtime */
#define MID_RL_INIT_FAIL      (1UL << 2U)     /* Fault during EstimRL init */
#define MID_RL_ESTIM_FAIL     (1UL << 3U)     /* Fault during EstimRL runtime */

/* MID measurements finished flags. */
#define MID_PP_FINISH   (1UL << 0U)           /* PpAssit finished (stopped by user) */
#define MID_RL_FINISH   (1UL << 1U)           /* EstimRL finished estimation */

/* Sets the fault bit defined by faultid in the faults variable */
#define MID_FAULT_SET(faults, faultid) ((faults) |= (((middef_fault_t)1U) << (faultid)))

/* Clears all fault bits in the faults variable */
#define MID_FAULT_CLEAR_ALL(faults) ((faults) = 0U)

/* Check if a fault bit is set in the faults variable, 0 = no fault */
#define MID_FAULT_ANY(faults) ((faults) > 0U)

#define MID_FAULT_I_DCBUS_OVER  (0U) /* OverCurrent fault flag */
#define MID_FAULT_U_DCBUS_UNDER (1U) /* Undervoltage fault flag */
#define MID_FAULT_U_DCBUS_OVER  (2U) /* Overvoltage fault flag */

/* States of machine enumeration. */
typedef enum _mid_sm_app_state_t{
    kMID_Start  = 0U,
    kMID_Pp     = 1U,
    kMID_RL     = 2U,
    kMID_Stop   = 3U,
    kMID_Fault  = 4U,
    kMID_Calib  = 5U,
} mid_sm_app_state_t;

/* Pointer to function with a pointer to state machine control structure. */
typedef void (*mid_pfcn_void_pms)(void);

/* Device fault typedef */
typedef uint16_t middef_fault_t;

/* Device fault thresholds */
typedef struct _middef_fault_thresholds_t
{
    frac16_t f16UDcBusOver;     /* DC bus over voltage level */
    frac16_t f16UDcBusUnder;    /* DC bus under voltage level */
} middef_fault_thresholds_t;

/* Measurement type enumeration. */
typedef enum _mid_meas_type_t
{
    kMID_PolePairs        = 0U,
    kMID_ElectricalParams = 1U,
} mid_meas_type_t;

/* Float Motor parameters for internal MID structures and calculations. */
typedef struct _mid_motor_params
{
    uint32_t  ui32Pp; /* Number of pole-pairs. [-] */
    frac32_t  f32Rs;  /* Stator resistance. [Ohm] */
    frac32_t  f32Ld;  /* Direct-axis inductance. [H] */
    frac32_t  f32Lq;  /* Quadrature axis inductance. [H] */
    frac32_t  f32Udt; /* Dead time voltage drop of the power stage [V]. */
} mid_motor_params_t;

/* Motor parameters for user setting. */
typedef struct _mid_motor_params_user
{
    uint32_t  ui32Pp; /* Number of pole-pairs. [-] */
    frac32_t  f32Rs;  /* Stator resistance. [Ohm] */
    frac32_t  f32Ld;  /* Direct-axis inductance. [H] */
    frac32_t  f32Lq;  /* Quadrature-axis inductance. [H] */
    frac32_t  f32Udt; /* Dead time voltage drop of the power stage [V]. */
} mid_motor_params_user_t;

/* MID FOC structure */
typedef struct _mid_pmsm_foc
{
    GDFLIB_FILTER_IIR1_T_F32 sUDcBusFilter;   /* Dc bus voltage filter */
    GMCLIB_3COOR_T_F16 sIABC;                 /* Measured 3-phase current */
    GMCLIB_2COOR_ALBE_T_F16 sIAlBe;           /* Alpha/Beta current */
    GMCLIB_3COOR_T_F16 sDutyABC;              /* Applied duty cycles ABC */
    GMCLIB_2COOR_ALBE_T_F16 sUAlBeReq;        /* Required Alpha/Beta voltage */
    GMCLIB_2COOR_ALBE_T_F16 sUAlBeComp;       /* Compensated to DC bus Alpha/Beta voltage */
    uint16_t ui16SectorSVM;                   /* SVM sector */
    frac16_t f16UDcBus;                       /* DC bus voltage */
    frac16_t f16UDcBusFilt;                   /* Filtered DC bus voltage */
} mid_pmsm_foc_t;

/* Measurement status. */
typedef struct _mid_status_t
{
    mid_sm_app_state_t eMIDState;            /* Actual MID state-machine state. */
    uint32_t           ui32AllFinishedMeas;  /* All finished measurements. */
    uint32_t           ui32ActFinishedMeas;  /* Actual finished measurement. */
    uint32_t           ui32FaultMID;         /* MID fault flags. */
} mid_status_t;


/* Measurement global structure. */
typedef struct _mid_struct_t
{
    mid_meas_type_t     eMeasurementType;   /* Measurement type. */
    mid_motor_params_t  sMotorParams;       /* Motor Parameters. */
    mid_status_t        sMIDMeasStatus;     /* Measurement status. */
    bool_t              bMIDStart;          /* MID trigger variable. */
} mid_struct_t;

/* MID and FOC global structure. */
typedef struct _mid_pmsm_t
{
    mid_pmsm_foc_t            sFocPMSM;             /* Field Oriented Control structure */
    middef_fault_t            sFaultIdCaptured;     /* Captured faults (must be cleared manually) */
    middef_fault_t            sFaultIdPending;      /* Fault pending structure */
    middef_fault_thresholds_t sFaultThresholds;     /* Fault thresholds */
    bool_t                    bFaultClearMan;       /* Manual fault clear detection */
    uint16_t                  ui16CounterState;     /* Main state counter */
    uint16_t                  ui16TimeCalibration;  /* Calibration time count number */
    frac16_t                  f16AdcAuxSample;      /* Auxiliary ADC sample  */
} mid_pmsm_t;

/* MID application command enum. */
typedef enum _foc_mid_cmd
{
    kMID_Cmd_Stop      = 0U,  /* STOP command. */
    kMID_Cmd_Start     = 1U,  /* START command. */
    kMID_Cmd_Executing = 2U   /* EXECUTING command. */
} mid_app_cmd_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/
/* The MID control ctructure. */
extern mid_struct_t g_sMID;

#endif /* _MID_DEF_H_ */
