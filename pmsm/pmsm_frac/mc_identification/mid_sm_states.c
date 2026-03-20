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

#include "mid_sm_states.h"

#define M1_SVM_SECTOR_DEFAULT (2)        /* default SVM sector */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* (user) Motor parameters identification state machine functions */
static void MID_StateStart(void);
static void MID_StatePp(void);
static void MID_StateRL(void);
static void MID_StateStop(void);
static void MID_StateFault(void);
static void MID_StateCalib(void);

/* (user) Motor parameters identification state-transition functions */
static void MID_TransStart2Pp(void);
static void MID_TransStart2RL(void);
static void MID_TransAll2Stop(void);
static void MID_TransAll2Fault(void);
static void MID_TransStop2Calib(void);
static void MID_TransCalib2Start(void);

static void MID_ClearFOCVariables(void);
static void MID_FaultDetection(void);
static void MID_ReadSignals(void);
static void MID_ApplySignals(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* State machine functions field */
const mid_pfcn_void_pms g_MID_SM_STATE_TABLE[7U] = {MID_StateStart, MID_StatePp, MID_StateRL, MID_StateStop, MID_StateFault, MID_StateCalib};

/* User Measurement Type selection */
mid_meas_type_t eUserMIDMeasType;

/* MID measurement status. */
mid_status_t sUserMIDStatus;

/* User params for setting motor params */
mid_motor_params_user_t sUserMIDMotorParamsKnown = {M1_MOTOR_PP, (frac32_t)0, (frac32_t)0, (frac32_t)0, (frac32_t)0};
mid_motor_params_user_t sUserMIDMotorParamsMeas  = {M1_MOTOR_PP, (frac32_t)0, (frac32_t)0, (frac32_t)0, (frac32_t)0};

/* Global structure for all measurements */
mid_struct_t g_sMID;

/* Global structure for the motor FOC */
mid_pmsm_t g_sMidDrive;

/* FreeMASTER scales - DO NOT USE THEM in the code to avoid float library include */
volatile float g_fltMIDDCBvoltageScale;
volatile float g_fltMIDcurrentScale;
volatile float g_fltMIDparamScale;
volatile float g_fltMIDspeedScale;

/* Variables for Pp Assist */
MCAA_PPASSIST_INIT_RET_T_F16 ePpAssistInitRetVal;     /* Return value of the MCAA_PpAssistInit_FLT() */
MCAA_PPASSIST_RET_T_F16      ePpAssistRetVal;         /* Return value of the MCAA_PpAssist_FLT() */ 
MCAA_PPASSIST_INIT_T_F16     g_sPpAssistInitCfg;      /* Pp Assistant initialization structure */
MCAA_PPASSIST_T_F16          g_sPpAssistStruct;       /* Pp Assistant configuration structure */
pp_assist_cfg_params_t       g_sPpAssistInitFMSTR = { /* Control structure used in FreeMASTER */
    SCALE_I_F16(I_PP_ASSIST), SCALE_N_F16(N_PP_ASSIST),
    0U, 0U, (frac16_t)0, (acc32_t)0, (acc32_t)0, (acc32_t)0, (acc32_t)0, (acc32_t)0};

/* Variables for EstimRL */
MCAA_ESTIMRL_INIT_RET_T     eEstimRLInitRetVal;       /* Return value of the MCAA_EstimRLInit() */
MCAA_ESTIMRL_RET_T          eEstimRLRetVal;           /* Return value of the MCAA_EstimRL() */
MCAA_ESTIMRL_INIT_T_F32     g_sEstimRLInitCfg;        /* RL estimation initialization structure */
MCAA_ESTIMRL_T_F32          g_sEstimRLStruct;         /* RL estimation configuration structure */
MCAA_ESTIMRL_RUN_T_F32      g_sEstimRLCtrlRun;        /* Control manual mode and measured values in modes 1, 2 */
MCAA_ESTIMRL_ADV_TUNE_T_F32 g_sEstimRLAdvTune = ESTIMRL_ADV_TUNE_DEFAULT;      /* Advanced tuning parameters EstimRL algorithm */
uint8_t  u8ModeEstimRL;                               /* Selected identification mode. */
uint8_t  u8PlotCnt;                                   /* Plot counter. */
frac32_t f32IDcPlot;                                  /* DC current [A]. */ 
frac32_t f32LdPlot;                                   /* Estimated d-axis inductance. */        
frac32_t f32LqPlot;                                   /* Estimated q-axis inductance. */
frac32_t f32LdqTable[3*NUM_MEAS];                     /* Array for measuring DC current and estimated inductances. */
uint32_t u32EstimRLTimeoutCnt;                        /* Estimation timeout counter [-]. */
rl_estim_cfg_params_t g_sEstimRLInitFMSTR = {
    SCALE_I_F32(I_RL_ESTIM), SCALE_I_F32(I_POSMAX), SCALE_I_F32(I_NEGMAX), SCALE_I_F32(I_LD), SCALE_I_F32(I_LQ)};     /* Control structure used in FreeMASTER */


/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief MID START state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_StateStart(void)
{
    /* Type the code to do when in the START state */
    
    /* Transition to required state */
    switch (g_sMID.eMeasurementType)
    {
        case kMID_PolePairs:
            /* if kMID_PolePairs go to PP state */
            MID_TransStart2Pp();
            break;
            
        case kMID_ElectricalParams:
            /* if kMID_ElectricalParams go to RL state */
            MID_TransStart2RL();
            break;
            
        default:
            /* if none of above eMeasurementType applies, go to STOP */
            MID_TransAll2Stop();
            break;
    }
}

/*!
 * @brief MID PP state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_StatePp(void)
{  
    /* Type the code to do when in the Pp state */

    /* Call Pp measurement routine */
    ePpAssistRetVal = MCAA_PpAssist_F16(g_sMidDrive.sFocPMSM.f16UDcBusFilt,
                                       &g_sMidDrive.sFocPMSM.sIAlBe,
                                       &g_sPpAssistStruct, 
                                       &g_sMidDrive.sFocPMSM.sUAlBeReq);
    switch(ePpAssistRetVal)
    {
        case PPASSIST_RET_IN_PROGRESS:
            break;
            
        case PPASSIST_RET_DONE:
            /* Indicate finished measurement. */
            g_sMID.sMIDMeasStatus.ui32ActFinishedMeas |= MID_PP_FINISH;
            
            /* Go to STOP state */
            MID_TransAll2Stop();
            break;
            
        default:
            /* Error during parameters estimation */
            g_sMID.sMIDMeasStatus.ui32FaultMID = MID_PP_MEAS_FAIL; 
            MID_TransAll2Fault();
    }
}

/*!
 * @brief MID RL state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_StateRL(void)
{   
    /* Type the code to do when in the RL state */
    if(u32EstimRLTimeoutCnt > 0U)
    {
        if(--u32EstimRLTimeoutCnt <= 0U)
        {
            /* Electrical estimator timeout fault */
            g_sMID.sMIDMeasStatus.ui32FaultMID = MID_RL_ESTIM_FAIL; 
            MID_TransAll2Fault();
        }
    }

    /* Call RL measurement routine */
    eEstimRLRetVal = MCAA_EstimRL_F16(g_sMidDrive.sFocPMSM.f16UDcBus,
                                     &g_sMidDrive.sFocPMSM.sIAlBe, 
                                     &g_sEstimRLStruct,
                                     &g_sEstimRLCtrlRun,
                                     &g_sEstimRLAdvTune,
                                     &g_sMidDrive.sFocPMSM.sUAlBeReq);

    switch(eEstimRLRetVal)
    {
        case ESTIMRL_RET_IN_PROGRESS:
            break;
          
        case ESTIMRL_RET_DONE: 
              /* Store estimated parameters */
            g_sMID.sMotorParams.f32Rs  = g_sEstimRLStruct.f32R;
            g_sMID.sMotorParams.f32Ld  = g_sEstimRLStruct.f32Ld;
            g_sMID.sMotorParams.f32Lq  = g_sEstimRLStruct.f32Lq;
            g_sMID.sMotorParams.f32Udt = g_sEstimRLStruct.f32Udt;
            
            /* Indicate finished measurement */
            g_sMID.sMIDMeasStatus.ui32ActFinishedMeas |= MID_RL_FINISH;
            
            /* Go to STOP state */
            MID_TransAll2Stop();  
          break;
            
        default:
            /* Error during parameters estimation */
            g_sMID.sMIDMeasStatus.ui32FaultMID = MID_RL_ESTIM_FAIL; 
            MID_TransAll2Fault();
        break;
    }
}

/*!
 * @brief MID STOP state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_StateStop(void)
{
    /* Type the code to do when in the STOP state */

    /* Plot dq-inductance */
    f32IDcPlot = f32LdqTable[u8PlotCnt*3]; 
    f32LdPlot  = f32LdqTable[u8PlotCnt*3 + 1];
    f32LqPlot  = f32LdqTable[u8PlotCnt*3 + 2];

    u8PlotCnt++;
    if(u8PlotCnt>=NUM_MEAS)
        u8PlotCnt=0U;

    /* Wait in STOP unless measurement is triggered. */
    if(g_sMID.bMIDStart == TRUE)
    {     
        /* Go to MID CALIB (ADC calibration) */
        MID_TransStop2Calib();
    }
}

/*!
 * @brief MID FAULT state
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_StateFault(void)
{
    /* Type the code to do when in the FAULT state */
  
    /* If fault is no longer pending go to STOP state */
    if(!MID_FAULT_ANY(g_sMidDrive.sFaultIdPending))
    {
        MID_TransAll2Stop();
    }
}

/*!
 * @brief MID CALIB state
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_StateCalib(void)
{
    if (--g_sMidDrive.ui16CounterState == 0U)
    {
        /* Write calibrated offset values */
        M1_MCDRV_CURR_3PH_CALIB_SET(&g_sM1Curr3phDcBus);

        /* To switch to the START state */
        MID_TransCalib2Start();
    }
    else
    {
        /* Call offset measurement */
        M1_MCDRV_CURR_3PH_CALIB(&g_sM1Curr3phDcBus);

        /* Change SVM sector in range <1;6> to measure all AD channel mapping combinations */
        if (++g_sMidDrive.sFocPMSM.ui16SectorSVM > 6U)
            g_sMidDrive.sFocPMSM.ui16SectorSVM = 1U;
    }
}

/*!
 * @brief MID START to PP transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_TransStart2Pp(void)
{
    /* Type the code to do when going from the Start to the Pp state */  
      
    /* Pass parmeters to initialization structure */  
    g_sPpAssistInitCfg.u32SamplingFreq   = F_SAMPLING;                              /* Sampling frequency [Hz]. */
    g_sPpAssistInitCfg.f16IdReqOpenLoop  = g_sPpAssistInitFMSTR.f16IdReqOpenLoop;   /* Openloop current [A]. */
    g_sPpAssistInitCfg.f16SpeedElReq     = g_sPpAssistInitFMSTR.f16SpeedElReq;      /* Required Electrical Speed [rpm]. */
    /* Advanced parameters */
    g_sPpAssistInitCfg.u16RampTime       = g_sPpAssistInitFMSTR.u16RampTime;        /* Frequency ramp time [s]. */
    g_sPpAssistInitCfg.u16ZeroPosTime    = g_sPpAssistInitFMSTR.u16ZeroPosTime;     /* Steady position time [s]. */
    g_sPpAssistInitCfg.f16DutyCycleLimit = g_sPpAssistInitFMSTR.f16DutyCycleLimit;  /* Maximum allowable duty cycle in frac [-]. */
    g_sPpAssistInitCfg.a32DPiPropGain    = g_sPpAssistInitFMSTR.a32DPiPropGain;     /* Proportional gain of the D-axis current loop controller [-]. */
    g_sPpAssistInitCfg.a32DPiIntegGain   = g_sPpAssistInitFMSTR.a32DPiIntegGain;    /* Integral gain of the D-axis current loop controller [-]. */
    g_sPpAssistInitCfg.a32QPiPropGain    = g_sPpAssistInitFMSTR.a32QPiPropGain;     /* Proportional gain of the Q-axis current loop controller [-]. */
    g_sPpAssistInitCfg.a32QPiIntegGain   = g_sPpAssistInitFMSTR.a32QPiIntegGain;    /* Integral gain of the Q-axis current loop controller [-]. */
    g_sPpAssistInitCfg.a32SpeedIntegGain = g_sPpAssistInitFMSTR.a32SpeedIntegGain;  /* Gain of speed integrator [-]. */
    
    /* Initialize the state variables and set algorithm parameters */
    ePpAssistInitRetVal = MCAA_PpAssistInit_F16(&g_sPpAssistInitCfg,
                                                &g_sPpAssistStruct);
    
    switch(ePpAssistInitRetVal)
    {
      case PPASSIST_RET_INIT_OK:
          /* Next is PP state */
          g_sMID.sMIDMeasStatus.ui32AllFinishedMeas &= ~MID_PP_FINISH; 
          M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
          g_sMID.sMIDMeasStatus.eMIDState = kMID_Pp;
          break;
      
      default:
          /* Next is FAULT state - initialization failed */
          g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_PP_INIT_FAIL;
          MID_TransAll2Fault(); 
          break;
    }  
}

/*!
 * @brief MID START to RL transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_TransStart2RL(void)
{
    /* Type the code to do when going from the Start to the RL state */   

    /* Parameters with invalid values are reset to default values */
    if(g_sEstimRLInitFMSTR.f32IDcPosMax <= 0)
        g_sEstimRLInitFMSTR.f32IDcPosMax = SCALE_I_F32(I_POSMAX);

    if(g_sEstimRLInitFMSTR.f32IDcMeas <= 0)
        g_sEstimRLInitFMSTR.f32IDcMeas = SCALE_I_F32(I_RL_ESTIM);

    if(0 <= g_sEstimRLInitFMSTR.f32IDcNegMax)
        g_sEstimRLInitFMSTR.f32IDcNegMax = SCALE_I_F32(I_NEGMAX);
    
    /* Identification init */    
    g_sEstimRLInitCfg.u16SamplingFreq = F_SAMPLING;
    switch(u8ModeEstimRL)
    {
      case 1:
          /* Mode 1 */ 
          g_sEstimRLInitCfg.f32IDcMax = g_sEstimRLInitFMSTR.f32IDcPosMax;
          g_sEstimRLInitCfg.f32IDcLd = 0;
          g_sEstimRLInitCfg.f32IDcLq = g_sEstimRLInitFMSTR.f32IDcLq;
          g_sEstimRLInitCfg.f32IDcNegMax = 0;
          g_sEstimRLInitCfg.u16LdqNumMeas = NUM_MEAS;
          
          g_sEstimRLCtrlRun.pLdqTable = f32LdqTable;
          g_sEstimRLCtrlRun.f32IDcDReq = 0;
          g_sEstimRLCtrlRun.f32IDcQReq = 0;
          g_sEstimRLCtrlRun.f32IAcReq = 0;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;      
          
          u32EstimRLTimeoutCnt = 0U;
          break;
          
      case 2:
          /* Mode 2 */
          g_sEstimRLInitCfg.f32IDcMax = g_sEstimRLInitFMSTR.f32IDcPosMax;
          g_sEstimRLInitCfg.f32IDcLd = g_sEstimRLInitFMSTR.f32IDcLd;
          g_sEstimRLInitCfg.f32IDcLq = g_sEstimRLInitFMSTR.f32IDcLq;
          g_sEstimRLInitCfg.f32IDcNegMax = g_sEstimRLInitFMSTR.f32IDcNegMax;
          g_sEstimRLInitCfg.u16LdqNumMeas = NUM_MEAS;
          
          g_sEstimRLCtrlRun.pLdqTable = f32LdqTable;
          g_sEstimRLCtrlRun.f32IDcDReq = 0;
          g_sEstimRLCtrlRun.f32IDcQReq = 0;
          g_sEstimRLCtrlRun.f32IAcReq = 0;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;
          
          u32EstimRLTimeoutCnt = 0U;
          break;
          
      case 3:
          /* Mode 3 */
          g_sEstimRLInitCfg.f32IDcMax = 0;
          g_sEstimRLInitCfg.f32IDcLd = 0;
          g_sEstimRLInitCfg.f32IDcLq = 0;
          g_sEstimRLInitCfg.f32IDcNegMax = 0;
          g_sEstimRLInitCfg.u16LdqNumMeas = 1U;
          
          g_sEstimRLCtrlRun.pLdqTable = 0;
          g_sEstimRLCtrlRun.f32IDcDReq = 0;
          g_sEstimRLCtrlRun.f32IDcQReq = 0;
          g_sEstimRLCtrlRun.f32IAcReq = 0;
          g_sEstimRLCtrlRun.u16FAc = 1U;                /* Set frequency greater than zero to avoid returning error from EstimRL. */
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;
          
          u32EstimRLTimeoutCnt = 0U;
          break;
          
      default:
          /* Mode 0 */
          g_sEstimRLInitCfg.f32IDcMax = g_sEstimRLInitFMSTR.f32IDcMeas;
          g_sEstimRLInitCfg.f32IDcLd = 0;
          g_sEstimRLInitCfg.f32IDcLq = 0;
          g_sEstimRLInitCfg.f32IDcNegMax = 0;
          g_sEstimRLInitCfg.u16LdqNumMeas = 0U;
          
          g_sEstimRLCtrlRun.pLdqTable = 0;
          g_sEstimRLCtrlRun.f32IDcDReq = 0;
          g_sEstimRLCtrlRun.f32IDcQReq = 0;
          g_sEstimRLCtrlRun.f32IAcReq = 0;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;                
          
          u32EstimRLTimeoutCnt = (uint32_t)(ESTIMRL_TIMEOUT * F_SAMPLING);
          break;
    }

    eEstimRLInitRetVal = MCAA_EstimRLInit_F16(&g_sEstimRLInitCfg,
                                              &g_sEstimRLStruct,
                                              &g_sEstimRLAdvTune);

    switch(eEstimRLInitRetVal)
    {
      case ESTIMRL_RET_INIT_OK:      
          /* Next is RL state */
          g_sMID.sMIDMeasStatus.ui32AllFinishedMeas &= ~MID_RL_FINISH;
          M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
          g_sMID.sMIDMeasStatus.eMIDState = kMID_RL;
          break;
          
      default:
          /* Next is FAULT state - initialization failed */
          g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_RL_INIT_FAIL;
          MID_TransAll2Fault();            
          break;
    }
}

/*!
 * @brief (general) Any state to MID STOP transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_TransAll2Stop(void)
{
    /* Type the code to do when going to the STOP state */  
    
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    
    /* Clear the measurement trigger if measurement has been finished. */
    if((g_sMID.eMeasurementType == kMID_ElectricalParams) ||
       (g_sMID.eMeasurementType == kMID_PolePairs))
    {
        g_sMID.bMIDStart = FALSE;
    }

    MID_ClearFOCVariables();
    
    /* Next is STOP state */
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Stop;
}

/*!
 * @brief (general) Any state to FAULT transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_TransAll2Fault(void)
{
    /* Type the code to do when going to the FAULT state */  
    
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    
    /* Set off measurement */
    g_sMID.bMIDStart = FALSE;
    
    /* Next is FAULT state */
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Fault;
}

/*!
 * @brief (general) MID STOP to CALIB transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_TransStop2Calib(void)
{
    /* Type the code to do when going from the STOP to the CALIB state */

    /* 50% duty cycle */
    g_sMidDrive.sFocPMSM.sDutyABC.f16A = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16B = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16C = FRAC16(0.5);

    /* PWM duty cycles calculation and update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);

    /* Clear offset filters */
    M1_MCDRV_CURR_3PH_CALIB_INIT(&g_sM1Curr3phDcBus);

    /* Pass calibration routine duration to state counter*/
	  g_sMidDrive.ui16CounterState = g_sMidDrive.ui16TimeCalibration;
    
    /* Next is CALIB state */
    M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Calib;
}

/*!
 * @brief (general) MID CALIB to START transition
 *
 * @param None
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransCalib2Start(void)
{
    /* Type the code to do when going to the START state */
    
    /* Next is START state */
    M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Start;
}

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * MID after-reset initialization function.
 */
void MID_Init_AR(void)
{
    /* Type the code to do when in the INIT routine */
  
    /* Clean the internal parameters. */
    g_sMID.sMotorParams.ui32Pp = M1_MOTOR_PP;
    g_sMID.sMotorParams.f32Rs  = 0;
    g_sMID.sMotorParams.f32Ld  = 0;
    g_sMID.sMotorParams.f32Lq  = 0;
    g_sMID.sMotorParams.f32Udt = 0;
    
    /* Clear status variables. */
    g_sMID.bMIDStart                          = FALSE;
    g_sMID.sMIDMeasStatus.ui32FaultMID        = 0UL;
    g_sMID.sMIDMeasStatus.ui32AllFinishedMeas = 0UL;
    g_sMID.sMIDMeasStatus.ui32ActFinishedMeas = 0UL;
    
    /* Set the initial MID state. */
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Stop;
  
    /**** Init FOC structure and pointer to drivers *****/
    g_sMidDrive.sFocPMSM.ui16SectorSVM = M1_SVM_SECTOR_DEFAULT;    
    g_sMidDrive.sFocPMSM.f16UDcBus     = 0;
    g_sMidDrive.sFocPMSM.f16UDcBusFilt = 0;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32B0 = M1_UDCB_IIR_B0;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32B1 = M1_UDCB_IIR_B1;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.f32A1 = M1_UDCB_IIR_A1;
    /* Filter init not to enter to fault */
    g_sMidDrive.sFocPMSM.sUDcBusFilter.f16FltBfrX[0] = (frac16_t)           ((M1_U_DCB_UNDERVOLTAGE / 2) + (M1_U_DCB_OVERVOLTAGE / 2));
    g_sMidDrive.sFocPMSM.sUDcBusFilter.f32FltBfrY[0] = (frac32_t)((uint32_t)((M1_U_DCB_UNDERVOLTAGE / 2) + (M1_U_DCB_OVERVOLTAGE / 2)) << 16);
    
    /* Timing control and general variables */
    g_sMidDrive.ui16CounterState    = 0U;
    g_sMidDrive.ui16TimeCalibration = M1_CALIB_DURATION;
    
    /* fault set to init states */
    MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdCaptured);
    MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdPending);
    
    /* Fault thresholds */
    g_sMidDrive.sFaultThresholds.f16UDcBusOver  = M1_U_DCB_OVERVOLTAGE;
    g_sMidDrive.sFaultThresholds.f16UDcBusUnder = M1_U_DCB_UNDERVOLTAGE;
    
    /* Defined scaling for FreeMASTER */
    g_fltMIDcurrentScale    = M1_I_MAX;
    g_fltMIDDCBvoltageScale = M1_U_DCB_MAX;
    g_fltMIDparamScale	    = (float)(M1_U_MAX / M1_I_MAX);
    g_fltMIDspeedScale      = M1_N_MAX;

    /* Clear rest of variables  */
    MID_ClearFOCVariables();

    /* Init sensors/actuators pointers */
    /* For PWM driver */
    g_sM1Pwm3ph.psUABC = &(g_sMidDrive.sFocPMSM.sDutyABC);
    
    /* For ADC driver */
    g_sM1Curr3phDcBus.pf16UDcBus     = &(g_sMidDrive.sFocPMSM.f16UDcBus);
    g_sM1Curr3phDcBus.psIABC         = &(g_sMidDrive.sFocPMSM.sIABC);
    g_sM1Curr3phDcBus.pui16SVMSector = &(g_sMidDrive.sFocPMSM.ui16SectorSVM);
    g_sM1Curr3phDcBus.pui16AuxChan   = &(g_sMidDrive.f16AdcAuxSample);
    
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
}

/*!
 * MID fast-loop process function.
 */
void MID_ProcessFast_FL(void)
{   
    /* Read measurements from FOC motor-control module. */
    MID_ReadSignals();
    
    /* Detects faults */
    MID_FaultDetection();
    
    /* If a fault is pending go to Fault. Higher priority then stopping by the user. */
    if (g_sMidDrive.sFaultIdPending != 0U)
    {
        MID_TransAll2Fault();
    }    
    
    /* Execute the MID state machine. */
    g_MID_SM_STATE_TABLE[g_sMID.sMIDMeasStatus.eMIDState]();
    
    /* Apply control signals results to FOC motor-control module. */    
    MID_ApplySignals();
}

/*!
 * MID start function.
 */
RAM_FUNC_LIB
void MID_Start_BL(mid_meas_type_t eMeasurementType)
{
    /* Pass the measurement type */
    g_sMID.eMeasurementType = eMeasurementType;

    /* Clear the fault registers. */
    g_sMID.sMIDMeasStatus.ui32FaultMID = 0U;

    /* Trigger the measurement */
    g_sMID.bMIDStart = TRUE;
}

/*!
 * MID stop function
 */
RAM_FUNC_LIB
void MID_Stop_BL(void)
{
    /* Check whether the pole-pair measurement is ongoing. */
    if(g_sPpAssistStruct.pState == PPASSIST_STATE_ROTATE)
    {
        /* Stop the Pp assistant */
        g_sPpAssistStruct.ui16PpDetermined = TRUE;
    }
    else
    {
        /* Go to STOP state immediately */
        MID_TransAll2Stop();
    }
}

/*!
 * Return the MID status.
 */
RAM_FUNC_LIB
bool_t MID_GetStatus_BL(mid_status_t *psMIDStatus)
{
    /* Copy the statuses. */
    psMIDStatus->eMIDState           = g_sMID.sMIDMeasStatus.eMIDState;
    psMIDStatus->ui32AllFinishedMeas = g_sMID.sMIDMeasStatus.ui32AllFinishedMeas;
    psMIDStatus->ui32ActFinishedMeas = g_sMID.sMIDMeasStatus.ui32ActFinishedMeas;
    psMIDStatus->ui32FaultMID        = g_sMID.sMIDMeasStatus.ui32FaultMID;

    /* Return the MID start status variable. */
    return g_sMID.bMIDStart;
}

/*!
 * Update finish status flags of all measurements.
 */
RAM_FUNC_LIB
void MID_UpdateMeasFlags(void)
{
    /* Set finished measurement status flag */
    g_sMID.sMIDMeasStatus.ui32AllFinishedMeas |= g_sMID.sMIDMeasStatus.ui32ActFinishedMeas;
    g_sMID.sMIDMeasStatus.ui32ActFinishedMeas  = 0UL;
}

/*!
 * Function sets known motor parameters before the measurement.
 */
RAM_FUNC_LIB
void MID_SetKnownMotorParams_BL(mid_motor_params_user_t *psMotorParams)
{
    /* Copy the parameters into the internal structure. */
    /* If Pp non-zero. */
    if(psMotorParams->ui32Pp != 0UL)
    {
        g_sMID.sMotorParams.ui32Pp = psMotorParams->ui32Pp;
    }
    /* If Rs non-zero. */
    if(psMotorParams->f32Rs != 0)
    {
        g_sMID.sMotorParams.f32Rs = psMotorParams->f32Rs;
    }
    /* If Ld non-zero. */
    if(psMotorParams->f32Ld != 0)
    {
        g_sMID.sMotorParams.f32Ld = psMotorParams->f32Ld;
    }
    /* If Lq non-zero. */
    if(psMotorParams->f32Lq != 0)
    {
        g_sMID.sMotorParams.f32Lq = psMotorParams->f32Lq;
    }
    /* If Udt non-zero. */
    if(psMotorParams->f32Udt != 0)
    {
        g_sMID.sMotorParams.f32Udt = psMotorParams->f32Udt;
    }
}

/*!
 * Function gets motor parameters (both measured and known).
 */
RAM_FUNC_LIB
void MID_GetMotorParams_BL(mid_motor_params_user_t *psMotorParams)
{
    /* Copy the parameters into the internal structure. */
    psMotorParams->ui32Pp = g_sMID.sMotorParams.ui32Pp;
    psMotorParams->f32Rs  = g_sMID.sMotorParams.f32Rs;
    psMotorParams->f32Ld  = g_sMID.sMotorParams.f32Ld;
    psMotorParams->f32Lq  = g_sMID.sMotorParams.f32Lq;
    psMotorParams->f32Udt = g_sMID.sMotorParams.f32Udt;
}

/*!
 * Function starts or stops MID.
 */
RAM_FUNC_LIB
void MID_Process_BL(mid_app_cmd_t *pMidCmd)
{
    /* Get the MID status and check whether the measurement is currently active. */
    if(TRUE == MID_GetStatus_BL(&sUserMIDStatus))
    {
        /* Check whether the measurement stop was requested. */
        if(kMID_Cmd_Stop == *pMidCmd)
        {
            /* Stop the identification. */
            MID_Stop_BL();
        }
        /* Enter Executing command. */
        else if(*pMidCmd != kMID_Cmd_Executing)
            *pMidCmd = kMID_Cmd_Executing;
    }
    else
    {
        /* Check if any DIAG fault is pending */
        if(!MID_FAULT_ANY(g_sMidDrive.sFaultIdPending))
        {
            /* Check whether the measurement start was requested. */
            if(kMID_Cmd_Start == *pMidCmd)
            {
                /* Update motor parameters in the MID. Known parameters set by user
                   have higher priority. Otherwise already estimated parameters
                   will be used. */
                MID_SetKnownMotorParams_BL(&sUserMIDMotorParamsMeas);
                MID_SetKnownMotorParams_BL(&sUserMIDMotorParamsKnown);
                
                /* Init MID state machine to STOP. */
                MID_TransAll2Stop();

                /* Start/trigger the MID. */
                MID_Start_BL(eUserMIDMeasType);  
            }
            /* Check whether the measurements were completed. */
            else if(0UL < sUserMIDStatus.ui32ActFinishedMeas)
            {
                /* Update finished measurements status flags */
                MID_UpdateMeasFlags();
                
                /* Get the measured motor parameters. */
                MID_GetMotorParams_BL(&sUserMIDMotorParamsMeas);
                
                /* Stop the measurement. */
                *pMidCmd = kMID_Cmd_Stop;
            }
            /* Defaultly is Stop command. */
            else if(*pMidCmd != kMID_Cmd_Stop)
                *pMidCmd = kMID_Cmd_Stop;
        }
    }
}

/*!
 * Function returns actual state of MID.
 */
uint16_t MID_GetActualState(void)
{
  return ((uint16_t)g_sMID.sMIDMeasStatus.eMIDState);
}

/*!
 * @brief Clear FOC variables in global variable
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_ClearFOCVariables(void)
{
    /* Clear FOC variables */
    g_sMidDrive.sFocPMSM.sIABC.f16A         = 0;
    g_sMidDrive.sFocPMSM.sIABC.f16B         = 0;
    g_sMidDrive.sFocPMSM.sIABC.f16C         = 0;
    g_sMidDrive.sFocPMSM.sIAlBe.f16Alpha    = 0;
    g_sMidDrive.sFocPMSM.sIAlBe.f16Beta     = 0;
    g_sMidDrive.sFocPMSM.sDutyABC.f16A      = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16B      = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16C      = FRAC16(0.5); 
    g_sMidDrive.sFocPMSM.sUAlBeReq.f16Alpha = 0;
    g_sMidDrive.sFocPMSM.sUAlBeReq.f16Beta  = 0;
}

/*!
 * @brief Fault detention routine - check various faults
 *
 * @param void  No input parameter
 *
 * @return None
 */
static void MID_FaultDetection(void)
{
    /* Clearing actual faults before detecting them again  */
    /* Clear all pending faults */
    MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdPending);

    /* Clear fault captured manually if required. */
    if (g_sMidDrive.bFaultClearMan)
    {
        /* Clear fault captured */
        MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdCaptured);
        g_sMidDrive.bFaultClearMan = FALSE;
    }

    /* Fault:   DC-bus over-current */
    if (M1_MCDRV_PWM3PH_FLT_GET(&g_sM1Pwm3ph))
    {
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_I_DCBUS_OVER);
    }

    /* Fault:   DC-bus over-voltage */
    if (g_sMidDrive.sFocPMSM.f16UDcBusFilt > g_sMidDrive.sFaultThresholds.f16UDcBusOver)
    {
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_U_DCBUS_OVER);
    }

    /* Fault:   DC-bus under-voltage */
    if (g_sMidDrive.sFocPMSM.f16UDcBusFilt < g_sMidDrive.sFaultThresholds.f16UDcBusUnder)
    {
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_U_DCBUS_UNDER);
    }

    /* pass fault to Fault ID Captured */
    g_sMidDrive.sFaultIdCaptured |= g_sMidDrive.sFaultIdPending;
}

/*!
 * Function reads voltages, currents, etc. for MID. Called before MID state
 * machine. User defined.
 */
RAM_FUNC_LIB
static void MID_ReadSignals(void)
{
    /* get all adc samples - DC-bus voltage, current, bemf and aux sample */
    M1_MCDRV_CURR_3PH_VOLT_DCB_GET(&g_sM1Curr3phDcBus);

    /* 3-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_Clark_F16(&g_sMidDrive.sFocPMSM.sIABC, &g_sMidDrive.sFocPMSM.sIAlBe);
    
    /* Sampled DC-Bus voltage filter */
    g_sMidDrive.sFocPMSM.f16UDcBusFilt =
        GDFLIB_FilterIIR1_F16(g_sMidDrive.sFocPMSM.f16UDcBus, &g_sMidDrive.sFocPMSM.sUDcBusFilter);
}

/*!
 * Function applying results/output from MID. Called after MID state
 * machine. User defined.
 */
RAM_FUNC_LIB
static void MID_ApplySignals(void)
{  
    if(g_sMID.sMIDMeasStatus.eMIDState != kMID_Calib)
    {
        /* DCBus ripple elimination */
        GMCLIB_ElimDcBusRipFOC_F16(g_sMidDrive.sFocPMSM.f16UDcBus, &g_sMidDrive.sFocPMSM.sUAlBeReq, &g_sMidDrive.sFocPMSM.sUAlBeComp);
        
        /* Space vector modulation */
        g_sMidDrive.sFocPMSM.ui16SectorSVM = GMCLIB_SvmStd_F16(&g_sMidDrive.sFocPMSM.sUAlBeComp, &g_sMidDrive.sFocPMSM.sDutyABC);
    }
    
    /* PWM peripheral update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);

    /* set current sensor for  sampling */
    M1_MCDRV_CURR_3PH_CHAN_ASSIGN(&g_sM1Curr3phDcBus);
}