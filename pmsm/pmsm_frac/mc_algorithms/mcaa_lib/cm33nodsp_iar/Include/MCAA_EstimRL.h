/*
* Copyright 2022, 2024 NXP
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
* @file       MCAA_EstimRL.h
*
* @version    1.0.0.1
*
* @date       20-March-2022
*
* @brief      Header file for mcaa_estimrl function
*
******************************************************************************/
#ifndef MCAA_ESTIMRL_H_
#define MCAA_ESTIMRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Includes
*******************************************************************************/
#include "gflib.h"
#include "gmclib.h"
#include "gdflib.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define FRAC32CONST(x) ((frac32_t)((x)*2147483648.0f)) /* Conversion of floating-point constant to frac32_t */

#define MCAA_ESTIMRL_FSN            (10000u) /* maximal sampling frequency [Hz] */
#define MCAA_ESTIMRL_FSN_FLT        ((frac32_t)(10000.0f)) /* maximal sampling frequency [Hz] in floating-point type */
#define MCAA_ESTIMRL_ACF            ((frac32_t)(500.0f)) /* Frequency of the injected AC current [Hz] */
#define MCAA_ESTIMRL_TIMEOUT        (2u)  /* Convergence detector timeout [s] */
#define MCAA_ESTIMRL_DCB_THR        (10u) /* DC bus limit counter threshold */
#define MCAA_ESTIMRL_IDC            (0u) /* Row of pLdqTable containing the DC current */
#define MCAA_ESTIMRL_LD             (1u) /* Row of pLdqTable containing the d-axis inductance */
#define MCAA_ESTIMRL_LQ             (2u) /* Row of pLdqTable containing the q-axis inductance */
#define MCAA_ESTIMRL_ROWS           (3u) /* Number of rows in pLdqTable */
#define MCAA_FLOAT_1_OVER_PI        (0.3183098861837907f) /* 1/pi */
#define MCAA_ESTIMRL_CD_LIML        ((frac32_t)(-1000.0f)) /* Convergence detector lower limit */
#define MCAA_ESTIMRL_CD_LIMH        ((frac32_t)(-100.0f))  /* Convergence detector coarse/fine tracking threshold */
#define MCAA_ESTIMRL_CD_COARSE_THR  (FRAC32CONST(0.1f)) /* Convergence detector coarse threshold */
#define MCAA_ESTIMRL_CD_FINE_THR    (FRAC32CONST(3.90625e-03f)) /* Convergence detector fine threshold */
#define MCAA_ESTIMRL_AC_IAMP        (FRAC32CONST(0.1f))  /* AC current amplitude fraction of the maximal current */
#define MCAA_ESTIMRL_PLL_LAMBDA_DQ  (FRAC32CONST(0.05f)) /* PLL d/q axis MA filter coefficient */
#define MCAA_ESTIMRL_PLL_LAMBDA_DC  (FRAC32CONST(0.05f)) /* PLL DC current MA filter coefficient */
#define MCAA_ESTIMRL_PLL_LAMBDA_STD (FRAC32CONST(0.01f)) /* PLL current noise standard deviation MA filter coefficient */
#define MCAA_ESTIMRL_PLL_KP         ((frac32_t)(50.0f)) /* PLL phase controller proportional gain */
#define MCAA_ESTIMRL_DC_IRAMP 		  (FRAC32CONST(0.002f))	 /* DC current ramp factor */
#define MCAA_ESTIMRL_ACDC_KI 		    (FRAC32CONST(0.0001f))	 /* AC/DC voltage controller integral gain (for Idcmax = IDCMAX_NOMINAL) */
#define IDCMAX_NOMINAL 				      ((frac32_t)(100.0f))	 /* Nominal value of the max. DC current for scaling of controller time constants */
#define UDCBUS_NOMINAL 				      ((frac32_t)(12.0f))	 /* Nominal value of the DC bus voltage for scaling of controller time constants */
#define MCAA_ESTIMRL_DCB_ELIM_IDX 	((frac32_t)(0.866025403784439f))	 /* DC bus eliminator inverse modulation index */
#define MCAA_ESTIMRL_AVG_MUL 		    ((frac32_t)(300.0f))	 /* Averaging window length multiplier */
#define MCAA_ESTIMRL_AVG_MIN 		    (FRAC32CONST(0.4f))	 /* Averaging window minimum length [seconds] */
#define MCAA_ESTIMRL_AVG_MAX 		    ((frac32_t)(1.0f))	 /* Maximum length of the noise averaging window [seconds]. Must be <= 1 for the fixed-point implementation. */

/* Default values for the advanced tuning parameters structure. */
#define ESTIMRL_ADV_TUNE_DEFAULT {\
  MCAA_ESTIMRL_ACF,\
  MCAA_ESTIMRL_TIMEOUT,\
  MCAA_ESTIMRL_DCB_THR,\
  MCAA_ESTIMRL_CD_LIML,\
  MCAA_ESTIMRL_CD_LIMH,\
  MCAA_ESTIMRL_CD_COARSE_THR,\
  MCAA_ESTIMRL_CD_FINE_THR,\
  MCAA_ESTIMRL_AC_IAMP,\
  MCAA_ESTIMRL_PLL_LAMBDA_DQ,\
  MCAA_ESTIMRL_PLL_LAMBDA_DC,\
  MCAA_ESTIMRL_PLL_LAMBDA_STD,\
  MCAA_ESTIMRL_PLL_KP,\
  MCAA_ESTIMRL_DC_IRAMP,\
  MCAA_ESTIMRL_ACDC_KI,\
  IDCMAX_NOMINAL,\
  UDCBUS_NOMINAL,\
  MCAA_ESTIMRL_DCB_ELIM_IDX,\
  MCAA_ESTIMRL_AVG_MUL,\
  MCAA_ESTIMRL_AVG_MIN,\
  MCAA_ESTIMRL_AVG_MAX}
  
#define MCAA_EstimRLInit_F16_C(psParam, psCtrl, psAdvTune)               \
        MCAA_EstimRLInit_F16_FC(psParam, psCtrl, psAdvTune)
#define MCAA_EstimRL_F16_C(f16UDcBus, psIAlBeFbck, psCtrl, psParam, psAdvTune, psUAlBeReq) \
        MCAA_EstimRL_F16_FC(f16UDcBus, psIAlBeFbck, psCtrl, psParam, psAdvTune, psUAlBeReq)

/*******************************************************************************
* Types
*******************************************************************************/
/* MCAA_EstimRLInit return value enum. */
typedef enum
{
  ESTIMRL_RET_INIT_OK = 0,              /* Initialization successful. */
  ESTIMRL_RET_INIT_ERROR = 1,           /* Invalid inputs. */
} MCAA_ESTIMRL_INIT_RET_T;

/* MCAA_EstimRL return value enum. */
typedef enum
{
  ESTIMRL_RET_IN_PROGRESS = 0,          /* Parameter estimation is in progress. The MCAA_EstimRL must be called again in the next sampling period. */
  ESTIMRL_RET_DONE = 1,                 /* Parameter estimation has finished. */
  ESTIMRL_RET_ERROR = 2,                /* Parameter estimation has failed. */
} MCAA_ESTIMRL_RET_T;

/* MCAA_EstimRL internal state enum. */
typedef enum
{
  ESTIMRL_STATE_UNINITIALIZED = 0,      /* RL estimator is not initialized. */
  ESTIMRL_STATE_MEAS_LD,                /* Measurement of d-axis inductance. */
  ESTIMRL_STATE_MEAS_LQ,                /* Measurement of q-axis inductance. */
  ESTIMRL_STATE_LD_TO_LQ,               /* Transition between axes. */
  ESTIMRL_STATE_POSTPROCESS_LD,         /* Postprocessing of accumulated values for d-axis measurement. */
  ESTIMRL_STATE_POSTPROCESS_LQ,         /* Postprocessing of accumulated values for q-axis measurement. */
  ESTIMRL_STATE_UNWIND,                 /* Gradual turn-off of the DC current at the end of measurement. */
  ESTIMRL_STATE_DONE,                   /* Measurement finished. */
  ESTIMRL_STATE_ERROR,                  /* Failure. */
} MCAA_ESTIMRL_STATE_T;

/* RL estimator fault enum, each bit of fault variable represents defined fault. */
typedef uint16_t estimrldef_fault_t; 
typedef enum
{
  ESTIMRL_FAULT_INIT_MISSING_FS = 0,    /* Missing sampling frequency */
  ESTIMRL_FAULT_INIT_FS_LIMIT,          /* Sampling frequency below limit */
  ESTIMRL_FAULT_INIT_I_DC_POS_LIMIT,    /* Invalid positive DC current */
  ESTIMRL_FAULT_INIT_I_DC_NEG_LIMIT,    /* Invalid negative DC current */
  ESTIMRL_FAULT_INIT_L_DQ_TABLE,        /* Invalid number of Ld/Lq table measurements */
  ESTIMRL_FAULT_INIT_I_DC_LD_POS_LIMIT, /* Ld measurement current exceedes maximal positive DC current */
  ESTIMRL_FAULT_INIT_I_DC_LD_NEG_LIMIT, /* Ld measurement current exceedes maximal negative DC current */
  ESTIMRL_FAULT_INIT_I_DC_LQ_POS_LIMIT, /* Lq measurement current exceedes maximal positive DC current */
  ESTIMRL_FAULT_INIT_I_DC_LQ_NEG_LIMIT, /* Lq measurement current exceedes maximal negative DC current */
  ESTIMRL_FAULT_RUN_UNINITIALIZED,      /* Invalid usage, must call init first */
  ESTIMRL_FAULT_RUN_FS_FINJ_RATIO,      /* Low ratio between sampling frequency and Hf current frequency */
  ESTIMRL_FAULT_RUN_I_AC_FREQ,          /* Invalid AC current frequency */
} MCAA_ESTIMRL_FAULT_T_FLT;

/* Internal state of the RL estimator structure. */
typedef struct
{
  MCAA_ESTIMRL_STATE_T pState;          /* RL estimator state */
  estimrldef_fault_t pFault;            /* Faults of electrical estimation internal state machine */
  uint16_t u16SamplingFreq;             /* Sampling frequency [Hz] */
  frac32_t f32FAc;                      /* Normalized fraction of the AC current frequency to be integrated in each iteration */
  frac32_t f32IDcMax;                   /* Maximum DC of current */
  frac32_t f32IDcNegMax;                /* Maximum allowed negative DC current */
  frac32_t f32IAcReq;                   /* Amplitude of the required AC current */
  int32_t u32IAcReqInv;                 /* Inverse of the amplitude of the required AC current */
  frac32_t f32IDcDReq;                  /* Required DC current in D axis */
  frac32_t f32IDcDReqk_1;               /* Required DC current in previous step in D axis */
  frac32_t f32IDcQReq;                  /* Required DC current in Q axis */
  frac32_t f32IDcQReqk_1;               /* Required DC current in previous step in Q axis */
  frac32_t f32IDcReqStep;               /* Measurement step of the required DC current */
  frac32_t f32IAlBeEstErr;              /* Current estimation error */
  GMCLIB_2COOR_ALBE_T_F32 pIAlBeAc;     /* Alpha/beta components of the estimated AC current */
  GMCLIB_2COOR_SINCOS_T_F16 pSinCosEst; /* Sine/Cosine of the estimated current phase */
  GMCLIB_2COOR_DQ_T_F32 pIDQAcRaw;      /* d/q components of the estimated AC current */
  GMCLIB_2COOR_DQ_T_F32 pIDQAcFilt;     /* Filtered d/q components of the estimated AC current */
  GDFLIB_FILTER_MA_T_A32 pDAxisFilter;  /* Parameters structure of the MA filter in the d axis in the PLL */
  GDFLIB_FILTER_MA_T_A32 pQAxisFilter;  /* Parameters structure of pDcFilter in the PLL */
  GDFLIB_FILTER_MA_T_A32 pDcFilter;     /* Parameters structure of the MA filter filtering the DC current in PLL */
  GDFLIB_FILTER_MA_T_A32 pStdFilter;    /* Parameters structure of the MA filter filtering the standard deviation of current noise */
  frac32_t f32PLLPropGain;              /* Gain of the P controller in the PLL */
  frac32_t f32ThAc;                     /* High-frequency AC voltage phase; <-PI, PI) range normalized into <-1, 1) */
  frac32_t f32ThEst;                    /* Estimated AC current phase; <-PI, PI) range normalized into <-1, 1) */
  frac32_t f32IDcD;                     /* Estimated DC current in D axis */
  frac32_t f32IDcQ;                     /* Estimated DC current in Q axis */
  int32_t i32IDcDAvg;                   /* Averaging accumulator for the estimated DC current in D axis */
  frac32_t f32IDcDAvgk_1;               /* Averaged estimated DC current in D axis in the last step */
  frac32_t f32PhComp;                   /* Compensation of phase error due to sampling lag, scaled by phi */
  int32_t i32ConvDetState;              /* Convergence detector state */
  int32_t i32ConvDetStateLimL;          /* Convergence detector lower limit */
  int32_t i32ConvDetStateLimH;          /* Convergence detector coarse/fine tracking threshold */
  uint32_t u32ConvDetToutCnt;           /* Convergence detector timeout counter */
  uint32_t u32ConvDetToutReload;        /* Convergence detector counter reload value */
  uint32_t u32AvgCnt;                   /* Noise averaging counter */
  uint32_t u32AvgN;                     /* Noise averaging window length */
  uint32_t u32AvgN1;                    /* Noise averaging window length in the first iteration */
  uint32_t u32AvgCntMax;                /* Maximum length of the noise averaging window */
  frac32_t f32PhAvg;                    /* Averaging accumulator for phase shift, scaled by phi */
  frac32_t f32IAcAvg;                   /* Averaging accumulator for AC current */
  frac32_t f32UAcAvg;                   /* Averaging accumulator for AC voltage */
  int32_t i32AvgMul;                    /* Averaging window length multiplier */
  int32_t i32AvgMin;                    /* Averaging window minimum length */
  uint16_t u16LdqNumMeas;               /* Number of Ld/Lq table measurements */
  uint32_t u32TblCounter;               /* Ld/Lq table counter */
  uint8_t u8RotorFixed;                 /* 0 == rotor is not mechanically fixed, 1 == rotor is mechanically fixed */
  frac32_t f32IDcRampCoef;              /* DC current ramp coefficient */
  frac32_t f32IDcRampCoefMax;           /* Maximum DC current ramp coefficient */
  frac32_t f32IDcDRampAcc;              /* DC current ramp accumulator in D axis */
  frac32_t f32IDcQRampAcc;              /* DC current ramp accumulator in Q axis */
  frac32_t f32UDcDAcc;                  /* DC voltage integrator in D axis */
  int32_t i32UDcDAccAvg;                /* Averaging accumulator for DC voltage in D axis */
  frac32_t f32UDcDAccAvgk_1;            /* Averaged DC voltage in D axis in previous step */
  frac32_t f32UDcQAcc;                  /* DC voltage integrator in Q axis */
  frac32_t f32UAcAcc;                   /* AC voltage integrator  */
  frac32_t f32UDcKi;                    /* DC voltage integrator gain */
  frac32_t f32UAcKi;                    /* AC voltage integrator gain */
  frac32_t f32UDcKiNominal;             /* Nominal DC voltage integrator gain */
  frac32_t f32UAcKiNominal;             /* Nominal AC voltage integrator gain */
  uint16_t u16RampSteady;               /* Indicator of the steady state of the voltage controller ramp */
  uint16_t u16DcbLimitFlag;             /* DC bus voltage limitation indicator */
  uint16_t u16DcbLimitFlagk_1;          /* DC bus voltage limitation indicator in previous step */
  uint16_t u16DcbLimitCnt;              /* DC bus voltage limitation counter */
  frac32_t f32PhaseShift;               /* Voltage/current phase shift, scaled by phi */
  frac32_t f32UdcHalf;                  /* Minimum DC bus voltage, halved and divided by modulation index */
  frac32_t f32FsInv;                    /* Inverse of the sampling frequency */
  frac32_t f32ImpConst;                 /* Constant used for calculation of impedance */
  frac32_t f32Ldk_1;                    /* Estimated d-axis inductance in previous step */
  frac32_t f32Lqk_1;                    /* Estimated q-axis inductance in previous step */
  frac32_t f32IDcLd;                    /* DC current level to use for measurement of scalar Ld */
  frac32_t f32IDcLq;                    /* DC current level to use for measurement of scalar Lq */
  frac32_t f32UdtAcc;                   /* Dead time voltage drop averaging accumulator */
  frac32_t f32RAcc;                     /* Resistance averaging accumulator, format Q33.31 */
  uint16_t u16LdqNumMeasR;              /* Number of measurements of R */
  frac32_t f32UDcBusMax;                /* Maximum observed DC bus voltage */
  frac32_t f32CDFineThr;                /* Convergence detector fine threshold */
  frac32_t f32CDCoarseThr;              /* Convergence detector coarse threshold */
  uint16_t u16ScComp;                   /* Scaling compensation flag */
} MCAA_ESTIMRL_INNERST_T_F32;

/* Estimator configuration structure. */
typedef struct
{
  MCAA_ESTIMRL_INNERST_T_F32 pInnerState;  /* Internal state of the RL estimator. No user-editable values. */
  frac32_t f32Ld;           /* Estimated d-axis inductance at zero DC current scaled in the [-1, 1) normalized range. Provided result must be multiplied by \f$U_{MAX}/I_{MAX}\f$ to get the inductance in [H]. */
  frac32_t f32Lq;           /* Estimated q-axis inductance at maximal DC current scaled in the [-1, 1) normalized range. Provided result must be multiplied by \f$U_{MAX}/I_{MAX}\f$ to get the inductance in [H]. */
  frac32_t f32R;            /* Estimated resistance scaled in the [-1, 1) normalized range. Provided result must be multiplied by \f$U_{MAX}/I_{MAX}\f$ to get the resistance in [Ohm]. */
  frac32_t f32Udt;          /* Estimated dead time voltage drop of the power stage scaled in the [-1, 1) normalized range. Provided result must be multiplied by \f$U_{MAX}\f$ to get the voltage in [V]. */
} MCAA_ESTIMRL_T_F32;

/* Initialization parameters of the RL estimator structure. */
  typedef struct
{
  uint32_t u16SamplingFreq; /* Sampling frequency [1/s]. The sampling frequency must be at least four times higher than the injected AC current frequency. */
  frac32_t f32IDcMax;       /* Maximum DC current. The value is scaled by \f$I_{MAX}.\f$ */
  frac32_t f32IDcLd;        /* DC current used for Ld measurement. The value is scaled by \f$I_{MAX}.\f$ */
  frac32_t f32IDcLq;        /* DC current used for Lq measurement. The value is scaled by \f$I_{MAX}.\f$ */
  frac32_t f32IDcNegMax;    /* Maximum allowed negative d-axis DC current. The value is scaled by \f$I_{MAX}.\f$ Note that too large negative DC current can cause permanent damage to the motor. Setting f32IDcNegMax to a safe value prevents irreversible demagnetization of the PMSM's magnets. The value of f32IDcNegMax must be negative or zero. */
  uint16_t u16LdqNumMeas;   /* Number of measured d-axis DC current levels. If this parameter is not 1, the function will perform automatic measurement for a range of DC currents. Otherwise, only one measurement will be performed. */
} MCAA_ESTIMRL_INIT_T_F32;

/* Run-time parameters of the RL estimator structure. */
typedef struct
{
  frac32_t *pLdqTable;      /* Pointer to a table with measured values. The table has 3 rows and u16LdqNumMeas columns stored in column-major order. Parameter u16LdqNumMeas of the init function MCAA_EstimRLInit_F16 determines the number of used DC current levels. The array pointed to by pLdqTable must provide room for 3*u16LdqNumMeas 32-bit values. The first row of the table contains the DC current levels, the second row contains the estimated d-axis inductances, and the third row contains the estimated q-axis inductances. */
  frac32_t f32IDcDReq;      /* Required DC current in the d-axis. The value is scaled by \f$I_{MAX}.\f$ */
  frac32_t f32IDcQReq;      /* Required DC current in the q-axis. The value is scaled by \f$I_{MAX}.\f$ */
  frac32_t f32IAcReq;       /* Amplitude of the required AC current. The value is scaled by \f$I_{MAX}.\f$ */
  uint16_t u16FAc;          /* AC current frequency [Hz] */
  uint8_t u8LdqSwitch;      /* Switches between Ld (u8LdqSwitch = 0) and Lq (u8LdqSwitch = 1) measurement */
} MCAA_ESTIMRL_RUN_T_F32;

/* Parameters for advanced tuning algorithm. */
typedef struct
{
  frac32_t f32ACf;          // MCAA_ESTIMRL_ACF ((frac32_t)(500.0f)) /* Frequency of the injected AC current [Hz] */
  uint16_t u16Timeout;      // MCAA_ESTIMRL_TIMEOUT (2u) /* Convergence detector timeout [s] */
  uint16_t u16DCBusThr;     // MCAA_ESTIMRL_DCB_THR (10u) /* DC bus limit counter threshold */
  frac32_t f32CdLimL;       // MCAA_ESTIMRL_CD_LIML ((frac32_t)(-1000.0f)) /* Convergence detector lower limit */
  frac32_t f32CdLimH;       // MCAA_ESTIMRL_CD_LIMH ((frac32_t)(-100.0f)) /* Convergence detector coarse/fine tracking threshold */
  frac32_t f32CdCoarseThr;  // MCAA_ESTIMRL_CD_COARSE_THR (FRAC32CONST(0.1f)) /* Convergence detector coarse threshold */ 
  frac32_t f32CdFineThr;    // MCAA_ESTIMRL_CD_FINE_THR (FRAC32CONST(3.90625e-03f)) /* Convergence detector fine threshold */
  frac32_t f32ACIamp;       // MCAA_ESTIMRL_AC_IAMP (FRAC32CONST(0.1f)) /* AC current amplitude fraction of the maximal current */
  frac32_t f32PllLambdaDQ;  // MCAA_ESTIMRL_PLL_LAMBDA_DQ (FRAC32CONST(0.05f)) /* PLL d/q axis MA filter coefficient */
  frac32_t f32PllLambdaDC;  // MCAA_ESTIMRL_PLL_LAMBDA_DC (FRAC32CONST(0.05f)) /* PLL DC current MA filter coefficient */
  frac32_t f32PllLambdaStd; // MCAA_ESTIMRL_PLL_LAMBDA_STD (FRAC32CONST(0.01f)) /* PLL current noise standard deviation MA filter coefficient */
  frac32_t f32PllKp;        // MCAA_ESTIMRL_PLL_KP ((frac32_t)(50.0f)) /* PLL phase controller proportional gain */
  frac32_t f32_DCIRamp;     // MCAA_ESTIMRL_DC_IRAMP (FRAC32CONST(0.002f)) /* DC current ramp factor */
  frac32_t f32ACDCKi;       // MCAA_ESTIMRL_ACDC_KI (FRAC32CONST(0.0001f)) /* AC/DC voltage controller integral gain (for Idcmax = IDCMAX_NOMINAL) */
  frac32_t f32IDCMaxNom;    // IDCMAX_NOMINAL ((frac32_t)(100.0f)) /* Nominal value of the max. DC current for scaling of controller time constants */
  frac32_t f32UDCBusNom;    // UDCBUS_NOMINAL ((frac32_t)(12.0f)) /* Nominal value of the DC bus voltage for scaling of controller time constants */
  frac32_t f32DCBusElimIdx; // MCAA_ESTIMRL_DCB_ELIM_IDX ((frac32_t)(0.866025403784439f)) /* DC bus eliminator inverse modulation index */
  frac32_t f32AvgMul;       // MCAA_ESTIMRL_AVG_MUL ((frac32_t)(300.0f)) /* Averaging window length multiplier */
  frac32_t f32AvgMin;       // MCAA_ESTIMRL_AVG_MIN (FRAC32CONST(0.4f)) /* Averaging window minimum length [seconds] */
  frac32_t f32AvgMax;       // MCAA_ESTIMRL_AVG_MAX ((frac32_t)(1.0f)) /* Maximum length of the noise averaging window [seconds]. Must be <= 1 for the fixed-point implementation. */
} MCAA_ESTIMRL_ADV_TUNE_T_F32;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
extern MCAA_ESTIMRL_INIT_RET_T MCAA_EstimRLInit_F16_FC(MCAA_ESTIMRL_INIT_T_F32 *psParam,
                                                       MCAA_ESTIMRL_T_F32 *const psCtrl,
                                                       MCAA_ESTIMRL_ADV_TUNE_T_F32 *psAdvTune);

extern MCAA_ESTIMRL_RET_T MCAA_EstimRL_F16_FC(frac16_t f16UDcBus,
                                              const GMCLIB_2COOR_ALBE_T_F16 *const psIAlBeFbck,
                                              MCAA_ESTIMRL_T_F32 *const psCtrl,
                                              MCAA_ESTIMRL_RUN_T_F32 *psParam,
                                              MCAA_ESTIMRL_ADV_TUNE_T_F32 *psAdvTune,
                                              GMCLIB_2COOR_ALBE_T_F16 *const psUAlBeReq);

/****************************************************************************
* Inline functions
****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif/* MCAA_ESTIMRL_H */