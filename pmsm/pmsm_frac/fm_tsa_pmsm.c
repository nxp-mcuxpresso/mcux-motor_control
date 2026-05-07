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

#include "freemaster.h"
#include "freemaster_tsa.h"

#include "mc_periph_init.h"

#ifdef PMSM_SNSLESS_ENC
#include "m1_sm_snsless_enc.h"
#else
#ifdef PMSM_SNSLESS
#include "m1_sm_snsless.h"
#endif
#endif

#ifdef MID_EN
#include "mid_sm_states.h"
#include "m1_mid_switch.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

#ifdef PMSM_SNSLESS_ENC
extern bool_t bDemoModePosition;
#endif

#ifdef MID_EN
/* MID 2.0 control commands */
extern mid_app_cmd_t g_eMidCmd;
#endif

#ifdef MC_EXAMPLE
/* Extern variables below are defined and used only in main.c, which is specific for each board. */

/* global control variables */
extern bool_t bDemoMode;

/* global used misc variables */
extern uint32_t g_ui32NumberOfCycles;
extern uint32_t g_ui32MaxNumberOfCycles;

/* Application and board ID  */
extern app_ver_t g_sAppIdFM;

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#ifdef PMSM_SNSLESS
/*!
 * @brief g_sM1Drive table structure
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_BEGIN(gsM1Drive_table)

/* gsM1Drive structure definition */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultIdEnable, FMSTR_TSA_UINT16)       /* M1 Fault Enable */
FMSTR_TSA_RW_VAR(g_sM1Drive.bFaultClearMan, FMSTR_TSA_UINT16)       /* M1 Fault Clear */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultIdCaptured, FMSTR_TSA_UINT16)     /* M1 Captured Fault */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultIdPending, FMSTR_TSA_UINT16)      /* M1 Pending Fault */
FMSTR_TSA_RW_VAR(g_sM1Drive.eControl, FMSTR_TSA_UINT16)             /* M1 MCAT Control */
FMSTR_TSA_RW_VAR(g_sM1Drive.ui16SlowCtrlLoopFreq, FMSTR_TSA_UINT16) /* M1 Slow Control Loop Frequency */
FMSTR_TSA_RW_VAR(g_sM1Drive.ui16FastCtrlLoopFreq, FMSTR_TSA_UINT16) /* M1 Fast Control Loop Frequency */

/* gsM1Drive.sOpenloop structure definition */
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.f16FreqReq, FMSTR_TSA_FRAC16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.sUDQReq.f16Q, FMSTR_TSA_FRAC16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.sUDQReq.f16D, FMSTR_TSA_FRAC16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.sIDQReq.f16Q, FMSTR_TSA_FRAC16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.sIDQReq.f16D, FMSTR_TSA_FRAC16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.bCurrentControl, FMSTR_TSA_UINT16)
FMSTR_TSA_RW_VAR(g_sM1Drive.sOpenloop.f16Theta, FMSTR_TSA_FRAC16)

/* gsM1Drive.sSpeed structure definition */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.f16SpeedFilt, FMSTR_TSA_FRAC16) /* M1 Speed filtered */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.f16Speed, FMSTR_TSA_FRAC16)     /* M1 Speed Estimated */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.f16SpeedRamp, FMSTR_TSA_FRAC16) /* M1 Speed Ramp */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.f16SpeedCmd, FMSTR_TSA_FRAC16)  /* M1 Speed Required */

/* sSpeed.sSpeedFilter.sSpeedFilter definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.f32A1, FMSTR_TSA_FRAC32) /* M1 Speed Filter A1 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.f32B0, FMSTR_TSA_FRAC32) /* M1 Speed Filter B0 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.f32B1, FMSTR_TSA_FRAC32) /* M1 Speed Filter B1 */

/* sSpeed.sSpeedFilter.sSpeedRampParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedRampParams.f32RampDown, FMSTR_TSA_FRAC32) /* M1 Speed Ramp Down */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedRampParams.f32RampUp, FMSTR_TSA_FRAC32)   /* M1 Speed Ramp Up */

/* sSpeed.sSpeedFilter.sSpeedRampParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedPiParams.a32IGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Speed Loop Ki Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedPiParams.a32PGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Speed Loop Kp Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedPiParams.f16UpperLim, FMSTR_TSA_FRAC16)        /* M1 Speed Loop Limit High */
FMSTR_TSA_RW_VAR(g_sM1Drive.sSpeed.sSpeedPiParams.f16LowerLim, FMSTR_TSA_FRAC16)        /* M1 Speed Loop Limit Low */

/* sSpeed.sAlignment definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sAlignment.ui16Time, FMSTR_TSA_UINT16) /* M1 Alignment Duration */
FMSTR_TSA_RW_VAR(g_sM1Drive.sAlignment.f16UdReq, FMSTR_TSA_FRAC16) /* M1 Alignment Voltage */

FMSTR_TSA_RW_VAR(g_sM1Drive.ui16TimeCalibration, FMSTR_TSA_UINT16) /* M1 Calibration duration */
FMSTR_TSA_RW_VAR(g_sM1Drive.ui16TimeFaultRelease, FMSTR_TSA_UINT16) /* M1 Fault state duration */

/* gsM1Drive.sFocPMSM structure definition */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16DutyCycleLimit, FMSTR_TSA_FRAC16) /* M1 Current Loop Limit */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16UDcBus, FMSTR_TSA_FRAC16)         /* M1 DCB Voltage */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16UDcBusFilt, FMSTR_TSA_SINT16)     /* M1 DCB Voltage Filtered */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16PosElExt, FMSTR_TSA_FRAC16)       /* M1 Posirtion External */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16PosEl, FMSTR_TSA_FRAC16)          /* M1 Position Electrical */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.f16PosElEst, FMSTR_TSA_FRAC16)       /* M1 Position Estimated */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.ui16SectorSVM, FMSTR_TSA_UINT16)     /* M1 SVM Sector */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32B0, FMSTR_TSA_FRAC32) 	/* M1 DcBus IIR B0 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32B1, FMSTR_TSA_FRAC32) 	/* M1 DcBus IIR B1 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32A1, FMSTR_TSA_FRAC32) 	/* M1 DcBus IIR A1 */

/* sFocPMSM.sIAlBe definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIAlBe.f16Alpha, FMSTR_TSA_FRAC16) /* M1 I alpha */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIAlBe.f16Beta, FMSTR_TSA_FRAC16)  /* M1 I beta */

/* sFocPMSM.sIDQ definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIDQ.f16D, FMSTR_TSA_FRAC16) /* M1 Id */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIDQ.f16Q, FMSTR_TSA_FRAC16) /* M1 Iq */

/* sFocPMSM.sIDQReq definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIDQReq.f16D, FMSTR_TSA_FRAC16) /* M1 Id req */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIDQReq.f16Q, FMSTR_TSA_FRAC16) /* M1 Iq req */

/* sFocPMSM.sIDQReq definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sUDQReq.f16D, FMSTR_TSA_FRAC16) /* M1 Ud req */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sUDQReq.f16Q, FMSTR_TSA_FRAC16) /* M1 Uq req */

/* sFocPMSM.sIdPiParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIdPiParams.a32IGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Id Ki Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIdPiParams.a32PGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Id Kp Gain */

/* sFocPMSM.sBemfObsrv definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.a32EGain, FMSTR_TSA_UFRAC_UQ(16, 15))       /* M1 Obsrv E gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.a32IGain, FMSTR_TSA_UFRAC_UQ(16, 15))       /* M1 Obsrv I gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.a32IGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Obsrv Ki gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.a32PGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Obsrv Kp gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.a32UGain, FMSTR_TSA_UFRAC_UQ(16, 15))       /* M1 Obsrv U gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sBemfObsrv.a32WIGain, FMSTR_TSA_UFRAC_UQ(16, 15))      /* M1 Obsrv WI gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.f32B0, FMSTR_TSA_FRAC32) 	/* M1 Obsrv To IIR B0 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.f32B1, FMSTR_TSA_FRAC32) 	/* M1 Obsrv To IIR B1 */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.f32A1, FMSTR_TSA_FRAC32) 	/* M1 Obsrv To IIR A1 */

/* sFocPMSM.sTo definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.f16IGain, FMSTR_TSA_FRAC16)    /* M1 Obsrv To Ki gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.i16IGainSh, FMSTR_TSA_SINT16)  /* M1 Obsrv To Ki shift */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.f16PGain, FMSTR_TSA_FRAC16)    /* M1 Obsrv To Kp gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.i16PGainSh, FMSTR_TSA_SINT16)  /* M1 Obsrv To Kp shift */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.f16ThGain, FMSTR_TSA_FRAC16)   /* M1 Obsrv To Theta gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sTo.i16ThGainSh, FMSTR_TSA_SINT16) /* M1 Obsrv To Theta shift */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.f32A1, FMSTR_TSA_FRAC32) 	/* M1 Obsrv To IIR A1 */
FMSTR_TSA_RW_VAR(g_sM1Drive.ui16TimeFullSpeedFreeWheel, FMSTR_TSA_UINT16) /* M1 Freewheel duration */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.ui16BlockedPerNum, FMSTR_TSA_UINT16) /* M1 Fault Threshold Blocked rotor period */

/* sFocPMSM.sIqPiParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIqPiParams.a32IGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Iq Ki Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIqPiParams.a32PGain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Iq Kp Gain */

/* sFocPMSM.sIABC definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIABC.f16A, FMSTR_TSA_FRAC16) /* M1 Phase Current A */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIABC.f16B, FMSTR_TSA_FRAC16) /* M1 Phase Current B */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sIABC.f16C, FMSTR_TSA_FRAC16) /* M1 Phase Current C */

/* sFocPMSM.sDutyABC definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sDutyABC.f16A, FMSTR_TSA_FRAC16) /* M1 Duty cycle A */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sDutyABC.f16B, FMSTR_TSA_FRAC16) /* M1 Duty cycle B */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFocPMSM.sDutyABC.f16C, FMSTR_TSA_FRAC16) /* M1 Duty cycle C */

/* sFaultThresholds definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16UqBemf, FMSTR_TSA_FRAC16)      /* M1 Fault Threshold BemfBlocked */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16UDcBusOver, FMSTR_TSA_FRAC16)  /* M1 Fault Threshold DcBusOver */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16UDcBusTrip, FMSTR_TSA_FRAC16)  /* M1 Fault Threshold DcBusTrip */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16UDcBusUnder, FMSTR_TSA_FRAC16) /* M1 Fault Threshold DcBusUnder */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16SpeedMin, FMSTR_TSA_FRAC16)    /* M1 Fault Threshold SpeedMin */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16SpeedNom, FMSTR_TSA_FRAC16)    /* M1 Fault Threshold SpeedNom */
FMSTR_TSA_RW_VAR(g_sM1Drive.sFaultThresholds.f16SpeedOver, FMSTR_TSA_FRAC16)   /* M1 Fault Threshold SpeedOver */

/* sStartUp definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16CoeffMerging, FMSTR_TSA_FRAC16)      /* M1 Merging Coefficient */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16RatioMerging, FMSTR_TSA_FRAC16)      /* M1 Merging Ratio */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16SpeedCatchUp, FMSTR_TSA_FRAC16)      /* M1 Merging Speed Catch Up */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16PosGen, FMSTR_TSA_FRAC16)            /* M1 Position Open Loop */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16SpeedCatchUp, FMSTR_TSA_FRAC16)      /* M1 Speed Merging Catch Up  */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16SpeedRampOpenLoop, FMSTR_TSA_FRAC16) /* M1 Speed Ramp Open Loop  */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.f16CurrentStartup, FMSTR_TSA_FRAC16)    /* M1 Startup Current  */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.sSpeedIntegrator.a32Gain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 StartUp Integ Ki  */

/* sStartUp.sSpeedRampOpenLoopParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.f32RampDown, FMSTR_TSA_FRAC32) /* M1 Startup Ramp Dec */
FMSTR_TSA_RW_VAR(g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.f32RampUp, FMSTR_TSA_FRAC32)   /* M1 Startup Ramp Inc */

/* sScalarCtrl definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.f16PosElScalar, FMSTR_TSA_FRAC16)  /* M1 Position Electrical Scalar */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.f16FreqRamp, FMSTR_TSA_FRAC16)     /* M1 Scalar Frequency Ramp */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.f16FreqCmd, FMSTR_TSA_FRAC16)      /* M1 Scalar speed */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.f16VHzGain, FMSTR_TSA_FRAC16)      /* M1 Scalar VHz Factor Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.f16UqMin, FMSTR_TSA_FRAC16)        /* M1 Minimal Uq voltage */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.sUDQReq.f16Q, FMSTR_TSA_FRAC16)    /* M1 Minimal Uq voltage */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.i16VHzGainShift, FMSTR_TSA_SINT16) /* M1 Scalar VHz Factor Gain Shift */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.sFreqIntegrator.a32Gain, FMSTR_TSA_UFRAC_UQ(16, 15)) /* M1 Scalar Integ Gain */

/* sScalarCtrl.sFreqRampParams definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.sFreqRampParams.f32RampDown, FMSTR_TSA_FRAC32) /* M1 Scalar Ramp Down */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.sFreqRampParams.f32RampUp, FMSTR_TSA_FRAC32)   /* M1 Scalar Ramp Up */

/* sScalarCtrl.sUDQReq definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sScalarCtrl.sUDQReq.f16Q, FMSTR_TSA_FRAC16) /* M1 Scalar volt */

/* sMCATctrl definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sMCATctrl.ui16PospeSensor, FMSTR_TSA_UINT16) /* M1 MCAT POSPE Sensor */

/* sMCATctrl.sIDQReqMCAT definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sMCATctrl.sIDQReqMCAT.f16D, FMSTR_TSA_FRAC16) /* M1 MCAT Id Required */
FMSTR_TSA_RW_VAR(g_sM1Drive.sMCATctrl.sIDQReqMCAT.f16Q, FMSTR_TSA_FRAC16) /* M1 MCAT Iq Required */

/* sMCATctrl.sUDQReqMCAT definitions */
FMSTR_TSA_RW_VAR(g_sM1Drive.sMCATctrl.sUDQReqMCAT.f16D, FMSTR_TSA_FRAC16) /* M1 MCAT Ud Required */
FMSTR_TSA_RW_VAR(g_sM1Drive.sMCATctrl.sUDQReqMCAT.f16Q, FMSTR_TSA_FRAC16) /* M1 MCAT Uq Required */

#ifdef PMSM_SNSLESS_ENC
/* gsM1Drive.sPosition structure definition */
FMSTR_TSA_RW_VAR(g_sM1Drive.sPosition.f16PositionPGain, FMSTR_TSA_FRAC16) /* M1 Position P conroller P Gain */
FMSTR_TSA_RW_VAR(g_sM1Drive.sPosition.a32Position, FMSTR_TSA_FRAC32)      /* M1 Position Actual */
FMSTR_TSA_RW_VAR(g_sM1Drive.sPosition.a32PositionError, FMSTR_TSA_FRAC32) /* M1 Position Error */
FMSTR_TSA_RW_VAR(g_sM1Drive.sPosition.a32PositionCmd, FMSTR_TSA_FRAC32)   /* M1 Position Required */
#endif

FMSTR_TSA_TABLE_END()
#endif

#ifdef MC_EXAMPLE
/*!
 * @brief Structure used in FM to get required ID's
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_BEGIN(sAppIdFM_table)

/* Board ID structure definition */
FMSTR_TSA_RW_MEM(g_sAppIdFM.cUserPath1, FMSTR_TSA_UINT8, &g_sAppIdFM.cUserPath1[0], 80)         /* User Path 1 */
FMSTR_TSA_RW_MEM(g_sAppIdFM.cUserPath2, FMSTR_TSA_UINT8, &g_sAppIdFM.cUserPath2[0], 80)         /* User Path 2 */
FMSTR_TSA_RO_MEM(g_sAppIdFM.cBoardID, FMSTR_TSA_UINT8, &g_sAppIdFM.cBoardID[0], 20)
FMSTR_TSA_RO_MEM(g_sAppIdFM.cExampleID, FMSTR_TSA_UINT8, &g_sAppIdFM.cExampleID[0], 30)
FMSTR_TSA_RO_MEM(g_sAppIdFM.cAppVer, FMSTR_TSA_UINT8, &g_sAppIdFM.cAppVer[0], 5)
FMSTR_TSA_RO_VAR(g_sAppIdFM.ui16FeatureSet, FMSTR_TSA_UINT16)

FMSTR_TSA_TABLE_END()
#endif


#ifdef MID_EN
/*!
 * @brief MID 2.0 table structure - usable if DEMO mode is not used
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_BEGIN(gsMID_table)

/* Measurement control from application */
FMSTR_TSA_RW_VAR(eUserMIDMeasType, FMSTR_TSA_UINT16)                            /* MID AP - Measurement type enumeration */

/* MID Status */
FMSTR_TSA_RW_VAR(sUserMIDStatus.eMIDState, FMSTR_TSA_UINT16)                    /* MID AP - Actual MID state-machine state */
FMSTR_TSA_RW_VAR(sUserMIDStatus.ui32AllFinishedMeas, FMSTR_TSA_UINT32)          /* MID AP - All finished measurements */
FMSTR_TSA_RW_VAR(sUserMIDStatus.ui32ActFinishedMeas, FMSTR_TSA_UINT32)          /* MID AP - Actual finished measurement */
FMSTR_TSA_RW_VAR(sUserMIDStatus.ui32FaultMID, FMSTR_TSA_UINT32)                 /* MID AP - MID fault flags */

/* MID known motor parameters. */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsKnown.ui32Pp, FMSTR_TSA_UINT32)             /* MID AP - Motor parameters set by user, number of pole-pairs */  
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsKnown.f32Rs, FMSTR_TSA_FRAC32)              /* MID AP - Motor parameters set by user, stator resistance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsKnown.f32Ld, FMSTR_TSA_FRAC32)              /* MID AP - Motor parameters set by user, direct-axis inductance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsKnown.f32Lq, FMSTR_TSA_FRAC32)              /* MID AP - Motor parameters set by user, quadrature-axis inductance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsKnown.f32Udt, FMSTR_TSA_FRAC32)             /* MID AP - Motor parameters set by user, dead time voltage drop of the power stage */

/* MID measured and/or known motor parameters. */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsMeas.ui32Pp, FMSTR_TSA_UINT32)              /* MID AP - Measured motor parameters, number of pole-pairs */  
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsMeas.f32Rs, FMSTR_TSA_FRAC32)               /* MID AP - Measured motor parameters, stator resistance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsMeas.f32Ld, FMSTR_TSA_FRAC32)               /* MID AP - Measured motor parameters, direct-axis inductance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsMeas.f32Lq, FMSTR_TSA_FRAC32)               /* MID AP - Measured motor parameters, quadrature-axis inductance */
FMSTR_TSA_RW_VAR(sUserMIDMotorParamsMeas.f32Udt, FMSTR_TSA_FRAC32)              /* MID AP - Measured motor parameters, dead time voltage drop of the power stage */

/* Global freemaster float variables */
FMSTR_TSA_RW_VAR(g_fltMIDcurrentScale, FMSTR_TSA_FLOAT)                         /* FMSTR_MID_currentScale */
FMSTR_TSA_RW_VAR(g_fltMIDDCBvoltageScale, FMSTR_TSA_FLOAT)                      /* FMSTR_MID_DCBvoltageScale */
FMSTR_TSA_RW_VAR(g_fltMIDspeedScale, FMSTR_TSA_FLOAT)                           /* FMSTR_MID_speedScale */
FMSTR_TSA_RW_VAR(g_fltMIDparamScale, FMSTR_TSA_FLOAT)                           /* FMSTR_MID_paramScale */

FMSTR_TSA_TABLE_END()


/* AP identification variables */
/*!
* @brief AP MID Pp table structure
*
* @param None
*
* @return None
*/
/* MID_AP_PP TSA table */
FMSTR_TSA_TABLE_BEGIN(ppAssist_table)

/* PpAssist - configuration measurement parameters */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.f16IdReqOpenLoop, FMSTR_TSA_FRAC16)	          /* MID AP - Openloop current */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.f16SpeedElReq, FMSTR_TSA_FRAC16)		          /* MID AP - Required Electrical Speed */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.u16RampTime, FMSTR_TSA_UINT16)		            /* MID AP - Frequency ramp time */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.u16ZeroPosTime, FMSTR_TSA_UINT16)		          /* MID AP - Steady position time */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.f16DutyCycleLimit, FMSTR_TSA_FRAC16)          /* MID AP - Maximum allowable duty cycle in frac */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.a32DPiPropGain, FMSTR_TSA_UFRAC_UQ(16, 15))	  /* MID AP - Proportional gain of the D-axis current loop controller */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.a32DPiIntegGain, FMSTR_TSA_UFRAC_UQ(16, 15))	/* MID AP - Integral gain of the D-axis current loop controller */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.a32QPiPropGain, FMSTR_TSA_UFRAC_UQ(16, 15))		/* MID AP - Proportional gain of the Q-axis current loop controller */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.a32QPiIntegGain, FMSTR_TSA_UFRAC_UQ(16, 15))	/* MID AP - Integral gain of the Q-axis current loop controller */
FMSTR_TSA_RW_VAR(g_sPpAssistInitFMSTR.a32SpeedIntegGain, FMSTR_TSA_UFRAC_UQ(16, 15))/* MID AP - Gain of speed integrator */

FMSTR_TSA_RW_VAR(g_sPpAssistStruct.pState, FMSTR_TSA_UINT16)                        /* MID AP - Status of pole-pair assistant internal state machine */       
FMSTR_TSA_RW_VAR(g_sPpAssistStruct.pFault, FMSTR_TSA_UINT8)                         /* MID AP - Fault of pole-pair assistant internal state machine */       

FMSTR_TSA_TABLE_END()

/*!
* @brief AP MID RL table structure
*
* @param None
*
* @return None
*/
/* MID_AP_RL TSA table */
FMSTR_TSA_TABLE_BEGIN(estimRL_table)

/* EstimRL - control in mode 3 (manual mode) */
FMSTR_TSA_RW_VAR(g_sEstimRLCtrlRun.f32IDcDReq, FMSTR_TSA_FRAC32)                /* MID: Config El I DC req (d-axis) */
FMSTR_TSA_RW_VAR(g_sEstimRLCtrlRun.f32IDcQReq, FMSTR_TSA_FRAC32)                /* MID: Config El I DC req (q-axis) */
FMSTR_TSA_RW_VAR(g_sEstimRLCtrlRun.f32IAcReq, FMSTR_TSA_FRAC32)                 /* MID: Config El I AC req */
FMSTR_TSA_RW_VAR(g_sEstimRLCtrlRun.u16FAc, FMSTR_TSA_UINT16)                    /* MID: Config El I AC frequency */
FMSTR_TSA_RW_VAR(g_sEstimRLCtrlRun.u8LdqSwitch, FMSTR_TSA_UINT8)                /* MID: Config El DQ-switch */
FMSTR_TSA_RW_VAR(u8ModeEstimRL, FMSTR_TSA_UINT8)                                /* MID: Estim RL mode select */
FMSTR_TSA_RW_VAR(f32IDcPlot, FMSTR_TSA_FRAC32)                                  /* EstimRL IDcPlot */
FMSTR_TSA_RW_VAR(f32LdPlot, FMSTR_TSA_FRAC32)                                   /* EstimRL LdPlot */
FMSTR_TSA_RW_VAR(f32LqPlot, FMSTR_TSA_FRAC32)                                   /* EstimRL LqPlot */

/* EstimRL - inner variables and signals */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32IDcDReq, FMSTR_TSA_FRAC32)     /* EstimRL IDcDReq */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32IDcD, FMSTR_TSA_FRAC32)        /* EstimRL IDcD */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32IDcQReq, FMSTR_TSA_FRAC32)     /* EstimRL IDcQReq */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32IDcQ, FMSTR_TSA_FRAC32)        /* EstimRL IDcQ */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32IAcReq, FMSTR_TSA_FRAC32)      /* EstimRL IAcReq */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.pIDQAcFilt.f32D, FMSTR_TSA_FRAC32)/* EstimRL IDQAcFilt_D */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.pIDQAcFilt.f32Q, FMSTR_TSA_FRAC32)/* EstimRL IDQAcFilt_Q */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32UDcDAcc, FMSTR_TSA_FRAC32)     /* EstimRL UDcDAcc */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.f32UDcQAcc, FMSTR_TSA_FRAC32)     /* EstimRL UDcQAcc */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.f32Ld, FMSTR_TSA_FRAC32)                      /* EstimRL estimated Ld */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.f32Lq, FMSTR_TSA_FRAC32)                      /* EstimRL estimated Lq  */
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.pState, FMSTR_TSA_UINT16)         /* EstimRL Status of electrical estimation internal state machine */       
FMSTR_TSA_RW_VAR(g_sEstimRLStruct.pInnerState.pFault, FMSTR_TSA_UINT8)          /* EstimRL Fault of electrical estimation internal state machine */       

/* EstimRL - configuration measurement parameters */
FMSTR_TSA_RW_VAR(g_sEstimRLInitFMSTR.f32IDcMeas, FMSTR_TSA_FRAC32)              /* MID: Config El I DC Meas */
FMSTR_TSA_RW_VAR(g_sEstimRLInitFMSTR.f32IDcPosMax, FMSTR_TSA_FRAC32)            /* MID: Config El I DC positive max */
FMSTR_TSA_RW_VAR(g_sEstimRLInitFMSTR.f32IDcNegMax, FMSTR_TSA_FRAC32)            /* MID: Config El I DC negative max */
FMSTR_TSA_RW_VAR(g_sEstimRLInitFMSTR.f32IDcLd, FMSTR_TSA_FRAC32)                /* MID: Config El I DC (estim Ld) */
FMSTR_TSA_RW_VAR(g_sEstimRLInitFMSTR.f32IDcLq, FMSTR_TSA_FRAC32)                /* MID: Config El I DC (estim Lq) */

FMSTR_TSA_TABLE_END()

/*!
 * @brief g_sMidDrive table structure
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_BEGIN(gsMidDrive_table)

FMSTR_TSA_RW_VAR(g_sMidDrive.bFaultClearMan, FMSTR_TSA_UINT16)                  /* DIAG: Fault clear */
FMSTR_TSA_RW_VAR(g_sMidDrive.sFaultIdCaptured, FMSTR_TSA_UINT16)                /* DIAG: Fault Captured */
FMSTR_TSA_RW_VAR(g_sMidDrive.sFaultIdPending, FMSTR_TSA_UINT16)                 /* DIAG: Pending Fault */

/* sFaultThresholds definitions */
FMSTR_TSA_RW_VAR(g_sMidDrive.sFaultThresholds.f16UDcBusOver, FMSTR_TSA_FRAC16)  /* MC MID Fault Threshold DcBusOver */
FMSTR_TSA_RW_VAR(g_sMidDrive.sFaultThresholds.f16UDcBusUnder, FMSTR_TSA_FRAC16) /* MC MID Fault Threshold DcBusUnder */

FMSTR_TSA_RW_VAR(g_sMidDrive.sFocPMSM.f16UDcBusFilt, FMSTR_TSA_FRAC16)          /* MC MID DCB Voltage Filtered */
FMSTR_TSA_RW_VAR(g_sMidDrive.ui16TimeCalibration, FMSTR_TSA_UINT16)             /* MID Calibration time count number */

FMSTR_TSA_TABLE_END()
#endif  /* MID_EN */

/*!
 * @brief g_sM1Enc driver structure
 *
 * @param None
 *
 * @return None
 */
#ifdef PMSM_SNSLESS_ENC
FMSTR_TSA_TABLE_BEGIN(gsM1Enc_table)

/* gsM1Enc structure definition */
FMSTR_TSA_RW_VAR(g_sM1Enc.f16SpdMeEst, FMSTR_TSA_FRAC16)      /* M1 Measured Mechanical Speed */
FMSTR_TSA_RW_VAR(g_sM1Enc.f16PosMe, FMSTR_TSA_FRAC16)         /* M1 Meassured Mechanical Position */
FMSTR_TSA_RW_VAR(g_sM1Enc.f16PosMeEst, FMSTR_TSA_FRAC16)      /* M1 Position Encoder Mechanical */

FMSTR_TSA_RW_VAR(g_sM1Enc.bDirection, FMSTR_TSA_UINT16)       /* M1 Encoder direction */
FMSTR_TSA_RW_VAR(g_sM1Enc.ui16Pp, FMSTR_TSA_UINT16)           /* M1 Pole pairs */

FMSTR_TSA_TABLE_END()
#endif


/*!
 * @brief Global table with global variables used in TSA
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_BEGIN(global_table)

#ifdef MC_EXAMPLE
/* global variables & control */
FMSTR_TSA_RW_VAR(bDemoMode, FMSTR_TSA_UINT16)               /* Demo Mode */
FMSTR_TSA_RW_VAR(g_ui32NumberOfCycles, FMSTR_TSA_UINT32)    /* Cycle Number */
FMSTR_TSA_RW_VAR(g_ui32MaxNumberOfCycles, FMSTR_TSA_UINT32) /* Cycle Number Maximum */
#endif

#ifdef PMSM_SNSLESS
/* global freemaster float variables */
FMSTR_TSA_RW_VAR(g_fltM1currentScale, FMSTR_TSA_FLOAT)    /* FMSTR_M1_currentScale */
FMSTR_TSA_RW_VAR(g_fltM1DCBvoltageScale, FMSTR_TSA_FLOAT) /* FMSTR_M1_DCBvoltageScale */
FMSTR_TSA_RW_VAR(g_fltM1frequencyScale, FMSTR_TSA_FLOAT)  /* FMSTR_M1_frequencyScale */
FMSTR_TSA_RW_VAR(g_fltM1speedScale, FMSTR_TSA_FLOAT)      /* FMSTR_M1_speedScale */
FMSTR_TSA_RW_VAR(g_fltM1voltageScale, FMSTR_TSA_FLOAT)    /* FMSTR_M1_voltageScale */

FMSTR_TSA_RW_VAR(g_bM1SwitchAppOnOff, FMSTR_TSA_UINT16)   /* Application Switch */
FMSTR_TSA_RW_VAR(g_sM1Ctrl.eState, FMSTR_TSA_UINT16)      /* Application State */
FMSTR_TSA_RW_VAR(g_eM1StateRun, FMSTR_TSA_UINT16) 			  /* State Run */

/* global freemaster float variables */
FMSTR_TSA_RW_VAR(g_fltM1bemfScale, FMSTR_TSA_FLOAT) /* FMSTR_M1_bemfScale */
#endif

#ifdef PMSM_SNSLESS_ENC
//FMSTR_TSA_RW_VAR(bDemoModePosition, FMSTR_TSA_UINT16) /* Demo Mode Position */
#endif

#ifdef MID_EN
/* Switch between M1/MID state machines */
FMSTR_TSA_RW_VAR(g_sM1toMID.eRequest, FMSTR_TSA_UINT16)         /* APP: Switch request Spin/MID */
FMSTR_TSA_RW_VAR(g_sM1toMID.eAppState, FMSTR_TSA_UINT16)        /* APP: State */
FMSTR_TSA_RW_VAR(g_sM1toMID.eFaultSwicth, FMSTR_TSA_UINT16)     /* APP: Fault */

/* MID control - variable is used only in m1_mid_switch.c */
FMSTR_TSA_RW_VAR(g_eMidCmd,     FMSTR_TSA_UINT16)
#endif

FMSTR_TSA_TABLE_END()

/*!
 * @brief TSA Table list required if TSA macro is enabled
 *
 * @param None
 *
 * @return None
 */
FMSTR_TSA_TABLE_LIST_BEGIN()

#ifdef MC_EXAMPLE
FMSTR_TSA_TABLE(sAppIdFM_table)
#endif

FMSTR_TSA_TABLE(global_table)

#ifdef PMSM_SNSLESS
FMSTR_TSA_TABLE(gsM1Drive_table)
#endif

#ifdef PMSM_SNSLESS_ENC
FMSTR_TSA_TABLE(gsM1Enc_table)
#endif

#ifdef MID_EN
FMSTR_TSA_TABLE(gsMID_table)
FMSTR_TSA_TABLE(ppAssist_table)
FMSTR_TSA_TABLE(estimRL_table)
FMSTR_TSA_TABLE(gsMidDrive_table)
#endif

FMSTR_TSA_TABLE_LIST_END()
