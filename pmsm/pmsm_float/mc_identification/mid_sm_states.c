/*
* Copyright 2020, 2024-2025 NXP
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
#include "mid_auxiliary.h"
#include "pp_measure.h"
#include "mlib.h"
#include "amclib_FP.h"
#include "mcaa_lib_fp.h"

#define M1_SVM_SECTOR_DEFAULT (2)        /* default SVM sector */
#define M1_BLOCK_ROT_FAULT_SH (0.03125F) /* filter window */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* (user) Motor parameters identification state machine functions */
RAM_FUNC_LIB
static void MID_StateStart(void);
RAM_FUNC_LIB
static void MID_StatePp(void);
RAM_FUNC_LIB
static void MID_StateRL(void);
RAM_FUNC_LIB
static void MID_StateBJ(void);
RAM_FUNC_LIB
static void MID_StateStop(void);
RAM_FUNC_LIB
static void MID_StateFault(void);
RAM_FUNC_LIB
static void MID_StateCalib(void);

/* (user) Motor parameters identification state-transition functions */
RAM_FUNC_LIB
static void MID_TransStart2Pp(void);
RAM_FUNC_LIB
static void MID_TransStart2RL(void);
RAM_FUNC_LIB
static void MID_TransStart2BJ(void);
RAM_FUNC_LIB
static void MID_TransAll2Stop(void);
RAM_FUNC_LIB
static void MID_TransAll2Fault(void);
RAM_FUNC_LIB
static void MID_TransStop2Calib(void);
RAM_FUNC_LIB
static void MID_TransCalib2Start(void);

RAM_FUNC_LIB
static void MID_ClearFOCVariables(void);
RAM_FUNC_LIB
static void MID_FaultDetection(void);
RAM_FUNC_LIB
static void MID_ReadSignals(void);
RAM_FUNC_LIB
static void MID_ApplySignals(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* User Measurement Type selection */
mid_meas_type_t eUserMIDMeasType;

/* MID measurement status. */
mid_status_t   sUserMIDStatus;

/* MID measurement parameters */
mid_config_t   sUserMIDMeasConfig = MID_DEFAULT_MEAS_CONFIG;

/* User params for setting motor params */
mid_motor_params_user_t sUserMIDMotorParamsKnown = {1UL,  0.0F, 0.0F, 0.0F, 0.0F,
                                                    0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
mid_motor_params_user_t sUserMIDMotorParamsMeas  = {1UL,  0.0F, 0.0F, 0.0F, 0.0F,
                                                    0.0F, 0.0F, 0.0F, 0.0F, 0.0F};

/* Global structure for all measurements */
mid_struct_t g_sMID;

/* FOC and MID */
mid_pmsm_t g_sMidDrive;

volatile float g_fltMIDvoltageScale;
volatile float g_fltMIDDCBvoltageScale;
volatile float g_fltMIDcurrentScale;
volatile float g_fltMIDspeedScale;
volatile float g_fltMIDspeedAngularScale;
volatile float g_fltMIDspeedMechanicalScale;

/* Variables for RL Estim */
MCAA_ESTIMRLINIT_RET_T_FLT  eEstimRetValInit;   /* Return value of the MCAA_EstimRLInit() */
MCAA_ESTIMRL_RET_T_FLT 	  eEstimRetVal;         /* Return value of the MCAA_EstimRL() */
MCAA_ESTIMRL_INIT_T_FLT g_sEstimRLInitCfg;      /* RL estimation initialization structure */
MCAA_ESTIMRL_T_FLT 	  g_sEstimRLStruct;         /* RL estimation configuration structure */
MCAA_ESTIMRL_RUN_T_FLT  g_sEstimRLCtrlRun;      /* Control manual mode and measured values in modes 1, 2 */
MCAA_ESTIMRL_ADV_TUNE_T g_sEstimRLAdvTune = ESTIMRL_ADV_TUNE_DEFAULT;      /* Advanced tuning parameters EstimRL algorithm */
uint8_t u8ModeEstimRL;                          /* Selected identification mode */
uint8_t u8PlotCnt;                              /* Plot counter. */
float_t	fltIDcPlot;                             /* DC current [A]. */ 
float_t	fltLdPlot;                              /* Estimated d-axis inductance [H]. */        
float_t	fltLqPlot;                              /* Estimated q-axis inductance [H]. */
float_t fltLdqTable[3*NUM_MEAS];                /* Array for measuring DC current and estimated inductances. */    
rl_estim_cfg_params_t g_sEstimRLInitFMSTR = {
    I_NOMINAL * 0.5, I_POSMAX, I_NEGMAX, I_LD, I_LQ}; /* Control structure used in FreeMASTER */

/* Variables for BJ Estim */
MCAA_ESTIMBJ_INIT_RET_T_FLT eEstimBJRetValInit;     /* Return value of the MCAA_EstimRLInit() */
MCAA_ESTIMBJ_RET_T_FLT      eEstimBJRetVal;         /* Return value of the MCAA_EstimBJ() */
MCAA_ESTIMBJ_INIT_T_FLT     g_sEstimBJInitCfg;      /* BJ estimation initialization structure */
MCAA_ESTIMBJ_T_FLT 	        g_sEstimBJStruct;       /* BJ estimation configuration structure */
bj_estim_cfg_params_t       g_sEstimBJInitFMSTR = { /* Control structure used in FreeMASTER */
    I_NOMINAL, N_NOMINAL, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*! @brief MID state machine function */
const mid_pfcn_void_pms g_MID_SM_STATE_TABLE[7U] = {MID_StateStart,
                                                    MID_StatePp,
                                                    MID_StateRL,
                                                    MID_StateBJ,
                                                    MID_StateStop,
                                                    MID_StateFault,
                                                    MID_StateCalib};

/*!
 * @brief MID START state
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_StateStart(void)
{
    /* Type the code to do when in the START state */

    /* MID alignment */
    MID_alignment(&g_sMID.sMIDAlignment, &g_sMidDrive.sFocPMSM);

    /* When MID_alignment is done and Motor Control SM proceeded to SPIN */
    if (g_sMID.sMIDAlignment.bActive == FALSE)
    {
        /* if kMID_PolePairs go to PP state */
        if(g_sMID.eMeasurementType == kMID_PolePairs)
        {
            MID_TransStart2Pp();
        }

        /* if kMID_ElectricalParams go to RL state */
        else if(g_sMID.eMeasurementType == kMID_ElectricalParams)
        {
            MID_TransStart2RL();
        }
        
        /* if kMID_MechanicalParams go to MECH state */
        else if(g_sMID.eMeasurementType == kMID_MechanicalParams)
        {
            MID_TransStart2BJ();
        }

        /* if none of above eMeasurementType applies, go to STOP */
        else
        {
            MID_TransAll2Stop();
        }
    }   
}

/*!
 * @brief MID PP state
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_StatePp(void)
{
    /* Type the code to do when in the PP state */

    /* Call Pp measurement routine */
    MID_getPp(&g_sMID.sMIDPp, &g_sMidDrive.sFocPMSM);

    /* Escape MID_StatePp when measurement ends */
    if(g_sMID.sMIDPp.bActive == FALSE)
    {
        /* Go to STOP state */
        MID_TransAll2Stop();
    }
}

/*!
 * @brief MID RL state
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_StateRL(void)
{
    /* Type the code to do when in the RL state */
    
    eEstimRetVal = MCAA_EstimRL_FLT(g_sMidDrive.sFocPMSM.fltUDcBus,
                                   &g_sMidDrive.sFocPMSM.sIAlBe,
                                   &g_sEstimRLStruct,
                                   &g_sEstimRLCtrlRun,
                                   &g_sEstimRLAdvTune,
                                   &g_sMidDrive.sFocPMSM.sUAlBeReq);
    
    switch(eEstimRetVal)
    {
        case ESTIMRL_RET_IN_PROGRESS:
        break;
        case ESTIMRL_RET_DONE: 
              /* Store estimated parameters */
              g_sMID.sMotorParams.fltRs  = g_sEstimRLStruct.fltR;
              g_sMID.sMotorParams.fltLd  = g_sEstimRLStruct.fltLd;
              g_sMID.sMotorParams.fltLq  = g_sEstimRLStruct.fltLq;
              g_sMID.sMotorParams.fltUdt = g_sEstimRLStruct.fltUdt;
              
              /* Indicate finished measurement. */
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
 * @brief MID BJ state
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_StateBJ(void)
{    
    /* Type the code to do when in the MECH state */

    eEstimBJRetVal = MCAA_EstimBJ_FLT(g_sMidDrive.sFocPMSM.fltUDcBus,
                                     &g_sMidDrive.sFocPMSM.sIAlBe,
                                     &g_sEstimBJStruct,
                                     &g_sMidDrive.sFocPMSM.sUAlBeReq);
   
    switch(eEstimBJRetVal)
    {
        case ESTIMBJ_RET_IN_PROGRESS:
            break;
            
        case ESTIMBJ_RET_DONE:
            /* Store estimated parameters */
            g_sMID.sMotorParams.fltKe = g_sEstimBJStruct.fltKe;
            g_sMID.sMotorParams.fltKt = g_sEstimBJStruct.fltKt;
            g_sMID.sMotorParams.fltJ  = g_sEstimBJStruct.fltJ;
            g_sMID.sMotorParams.fltA  = g_sEstimBJStruct.fltA;
            g_sMID.sMotorParams.fltB  = g_sEstimBJStruct.fltB;
            
            /* Indicate finished measurement. */
            g_sMID.sMIDMeasStatus.ui32ActFinishedMeas |= MID_BJ_FINISH;
            
            /* Go to STOP state */
            MID_TransAll2Stop();
            break;
          
        default:
            /* Error during parameters estimation */
            g_sMID.sMIDMeasStatus.ui32FaultMID = MID_BJ_ESTIM_FAIL;
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
RAM_FUNC_LIB
static void MID_StateStop(void)
{
    /* Type the code to do when in the STOP state */
  
    /* Plot dq-inductance */
    fltIDcPlot = fltLdqTable[u8PlotCnt*3]; 
    fltLdPlot  = fltLdqTable[u8PlotCnt*3 + 1];
    fltLqPlot  = fltLdqTable[u8PlotCnt*3 + 2];

    u8PlotCnt++;
    if(u8PlotCnt>=NUM_MEAS)
        u8PlotCnt=0;      
    
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
RAM_FUNC_LIB
static void MID_StateFault(void)
{
    /* After manual clear fault go to STOP state */
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
      if (++g_sMidDrive.sFocPMSM.ui16SectorSVM > 6)
          g_sMidDrive.sFocPMSM.ui16SectorSVM = 1;
  }
}

/*!
 * @brief MID START to PP transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransStart2Pp(void)
{
    /* Type the code to do when going from the Start to the Pp state */
    g_sMID.sMIDPp.bActive = FALSE;
    g_sMID.sMIDPp.ui16PpDetermined = FALSE;

    /* Enable FOC current loop */
    g_sMidDrive.sFocPMSM.bCurrentLoopOn = TRUE;

    /* Use OL position for Park transformation. */
    g_sMidDrive.sFocPMSM.bOpenLoop = TRUE;
    g_sMidDrive.sFocPMSM.bPosExtOn = TRUE;
    
    /* Indicate new measurement. */
    g_sMID.sMIDMeasStatus.ui32AllFinishedMeas &= ~MID_PP_FINISH;    
    
    /* Check range of the measurement configuration. 
       Frequency decided to be FAST_LOOP_FREQ / 10 */
    if((sUserMIDMeasConfig.fltPpIdReqOpenLoop <= I_NOMINAL) &&
       (sUserMIDMeasConfig.fltPpIdReqOpenLoop >  0.0F)           &&
       (sUserMIDMeasConfig.fltPpFreqElReq <= ((float_t)M1_PWM_FREQ / 10.0F))  &&
       (sUserMIDMeasConfig.fltPpFreqElReq >  0.0F))
    {
        /* Set the Pp measurement configuration. */
        g_sMID.sMIDPp.fltIdReqOpenLoop = sUserMIDMeasConfig.fltPpIdReqOpenLoop;
        g_sMID.sMIDPp.fltFreqElReq     = sUserMIDMeasConfig.fltPpFreqElReq;
        
        /* Next is PP state */
        g_sMID.sMIDMeasStatus.eMIDState = kMID_Pp;
    }
    else
    {
        /* Report Pp parameter measurement configuration out of range. */
        g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_PP_MEAS_OUT_OF_RANGE;
        MID_TransAll2Fault();
    }
}

/*!
 * @brief MID START to RL transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransStart2RL(void)
{
    /* Type the code to do when going from the Start to the RL state */  
    
    /* Disable FOC current loop */
    g_sMidDrive.sFocPMSM.bCurrentLoopOn = FALSE;
    
    /* Check selected current range */
    if(g_sEstimRLInitFMSTR.fltIDcPosMax > I_NOMINAL)
    {
      g_sEstimRLInitFMSTR.fltIDcPosMax = I_NOMINAL;
    }

    if(g_sEstimRLInitFMSTR.fltIDcNom > I_NOMINAL)
    {
      g_sEstimRLInitFMSTR.fltIDcNom = I_NOMINAL;
    }    

    if(g_sEstimRLInitFMSTR.fltIDcNegMax < (-I_NOMINAL))
    {
      g_sEstimRLInitFMSTR.fltIDcNegMax = -I_NOMINAL;
    }     
    
    /* Identification init */    
    switch(u8ModeEstimRL)
    {
      case 1:
          /* Mode 1 */ 
          g_sEstimRLInitCfg.fltIDcMax = g_sEstimRLInitFMSTR.fltIDcPosMax;
          g_sEstimRLInitCfg.fltIDcLd = 0.0F;
          g_sEstimRLInitCfg.fltIDcLq = g_sEstimRLInitFMSTR.fltIDcLq;
          g_sEstimRLInitCfg.fltIDcNegMax = 0.0F;
          g_sEstimRLInitCfg.u16LdqNumMeas = NUM_MEAS;
          
          g_sEstimRLCtrlRun.pLdqTable = fltLdqTable;
          g_sEstimRLCtrlRun.fltIDcDReq = 0.0F;
          g_sEstimRLCtrlRun.fltIDcQReq = 0.0F;
          g_sEstimRLCtrlRun.fltIAcReq = 0.0F;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;      
          break;
      case 2:
          /* Mode 2 */
          g_sEstimRLInitCfg.fltIDcMax = g_sEstimRLInitFMSTR.fltIDcPosMax;
          g_sEstimRLInitCfg.fltIDcLd = g_sEstimRLInitFMSTR.fltIDcLd;
          g_sEstimRLInitCfg.fltIDcLq = g_sEstimRLInitFMSTR.fltIDcLq;
          g_sEstimRLInitCfg.fltIDcNegMax = g_sEstimRLInitFMSTR.fltIDcNegMax;
          g_sEstimRLInitCfg.u16LdqNumMeas = NUM_MEAS;
          
          g_sEstimRLCtrlRun.pLdqTable = fltLdqTable;
          g_sEstimRLCtrlRun.fltIDcDReq = 0.0F;
          g_sEstimRLCtrlRun.fltIDcQReq = 0.0F;
          g_sEstimRLCtrlRun.fltIAcReq = 0.0F;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;
          break;
      case 3:
          /* Mode 3 */
          g_sEstimRLInitCfg.fltIDcMax = 0.0F;
          g_sEstimRLInitCfg.fltIDcLd = 0.0F;
          g_sEstimRLInitCfg.fltIDcLq = 0.0F;
          g_sEstimRLInitCfg.fltIDcNegMax = 0.0F;
          g_sEstimRLInitCfg.u16LdqNumMeas = 1U;
          
          g_sEstimRLCtrlRun.fltIDcDReq = 0.0F;
          g_sEstimRLCtrlRun.fltIDcQReq = 0.0F;
          g_sEstimRLCtrlRun.fltIAcReq = 0.0F;
          g_sEstimRLCtrlRun.u16FAc = 1U;                /* Set frequency greater than zero to avoid returning error from MCAA_EstimRL_FLT. */
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;
          break;
      default:
          /* Mode 0 */
          g_sEstimRLInitCfg.fltIDcMax = g_sEstimRLInitFMSTR.fltIDcNom;
          g_sEstimRLInitCfg.fltIDcLd = 0.0F;
          g_sEstimRLInitCfg.fltIDcLq = 0.0F;
          g_sEstimRLInitCfg.fltIDcNegMax = 0.0F;
          g_sEstimRLInitCfg.u16LdqNumMeas = 0U;
          
          g_sEstimRLCtrlRun.fltIDcDReq = 0.0F;
          g_sEstimRLCtrlRun.fltIDcQReq = 0.0F;
          g_sEstimRLCtrlRun.fltIAcReq = 0.0F;
          g_sEstimRLCtrlRun.u16FAc = 0U;               
          g_sEstimRLCtrlRun.u8LdqSwitch = 0U;                
          break;
    }
        
    eEstimRetValInit = MCAA_EstimRLInit_FLT(F_SAMPLING, &g_sEstimRLInitCfg ,&g_sEstimRLStruct, &g_sEstimRLAdvTune);
    
    switch(eEstimRetValInit)
    {
      case ESTIMRL_RET_INIT_OK:
          /* Next is RL state */
          g_sMID.sMIDMeasStatus.ui32AllFinishedMeas &= ~MID_RL_FINISH;
          M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
          g_sMID.sMIDMeasStatus.eMIDState = kMID_RL;
          break;
      default:
          g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_RL_INIT_FAIL; 
          MID_TransAll2Fault();            
          break;
    }
}

/*!
 * @brief MID START to BJ transition
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransStart2BJ(void)
{
    /* Type the code to do when going from the Start to the BJ state */

    /* Check range of the pole-pair parameter */
    if((g_sMID.sMotorParams.ui32Pp > 20UL) ||
       (g_sMID.sMotorParams.ui32Pp < 1UL))
    {
        /* Report pole-pair parameter out of range. */
        g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_BJ_PP_PARAM_OUT_OF_RANGE;
    }
    
    /* Check if necessary electrical parameters were available */    
    if(!((g_sMID.sMotorParams.fltLd ) ||
         (g_sMID.sMotorParams.fltLq ) ||
         (g_sMID.sMotorParams.fltRs ) ||
         (g_sMID.sMotorParams.fltUdt) ))
    {
        /* Report missing electrical input parameters */
        g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_BJ_MISSING_RL_PARAMS;
    }

    if(g_sMID.sMIDMeasStatus.ui32FaultMID != MID_START_SUCCESSFUL)
    {
        /* Next is FAULT state - invalid input parameters */
        MID_TransAll2Fault(); 
    }
    else
    {
        /* Disable FOC current loop */
        g_sMidDrive.sFocPMSM.bCurrentLoopOn = FALSE;    
        
        /* Parameters with invalid values are reset to default values */
        if(g_sEstimBJInitFMSTR.fltIN <= 0)
           g_sEstimBJInitFMSTR.fltIN = I_NOMINAL;

        if(g_sEstimBJInitFMSTR.fltNN <= 0)
           g_sEstimBJInitFMSTR.fltNN = N_NOMINAL;
        
        /* Pass parmeters to initialization structure */        
        g_sEstimBJInitCfg.u32SamplingFreq  = F_SAMPLING;                           /* Sampling frequency [1/s]. */
        g_sEstimBJInitCfg.fltIN            = g_sEstimBJInitFMSTR.fltIN;            /* Nominal current [A]. */
        g_sEstimBJInitCfg.fltNN            = g_sEstimBJInitFMSTR.fltNN;            /* Nominal speed [rpm]. */
        g_sEstimBJInitCfg.ui32Pp           = g_sMID.sMotorParams.ui32Pp;           /* Number of pole pairs [-]. */
        g_sEstimBJInitCfg.fltRs            = g_sMID.sMotorParams.fltRs;            /* Stator resistance [Ohm]. */
        g_sEstimBJInitCfg.fltLd            = g_sMID.sMotorParams.fltLd;            /* D-axis inductance [H]. */
        g_sEstimBJInitCfg.fltLq            = g_sMID.sMotorParams.fltLq;            /* Q-axis inductance [H]. */
        g_sEstimBJInitCfg.fltUdt           = g_sMID.sMotorParams.fltUdt;           /* Dead time voltage drop of the power stage [V]. */
        /* Advanced parameters */
        g_sEstimBJInitCfg.bEstimFriction   = g_sEstimBJInitFMSTR.bEstimFriction;   /* Enable "Advanced" mode (friction estimation) [-]. */
        g_sEstimBJInitCfg.fltAlignTime     = g_sEstimBJInitFMSTR.fltAlignTime;     /* Time needed for rotor alignment [s]. */
        g_sEstimBJInitCfg.fltIReqOl        = g_sEstimBJInitFMSTR.fltIReqOl;        /* Required d-axis current for open loop startup [A]. */
        g_sEstimBJInitCfg.fltNStepDc       = g_sEstimBJInitFMSTR.fltNStepDc;       /* Required DC speed step for ramp [delta rpm/s]. */
        g_sEstimBJInitCfg.fltNStepAc       = g_sEstimBJInitFMSTR.fltNStepAc;       /* Required AC speed step for ramp [delta rpm/s]. */
        g_sEstimBJInitCfg.fltNDcReq1       = g_sEstimBJInitFMSTR.fltNDcReq1;       /* Required speed for measurement point 1 [rpm]. */
        g_sEstimBJInitCfg.fltNDcReq2       = g_sEstimBJInitFMSTR.fltNDcReq2;       /* Required speed for measurement point 2 ("Advanced mode") [rpm]. */
        g_sEstimBJInitCfg.fltNAcReq        = g_sEstimBJInitFMSTR.fltNAcReq;        /* Required amplitude of the injected AC speed [rpm]. */
        g_sEstimBJInitCfg.fltFInjMax       = g_sEstimBJInitFMSTR.fltFInjMax;       /* Maximum possible injection frequency [Hz]. */    
        g_sEstimBJInitCfg.fltFInjMin       = g_sEstimBJInitFMSTR.fltFInjMin;       /* Minimum possible injection frequency [Hz]. */
        g_sEstimBJInitCfg.fltFInjStep      = g_sEstimBJInitFMSTR.fltFInjStep;      /* Injection frequency calculation step [Hz]. */
        g_sEstimBJInitCfg.fltSSTimeoutTime = g_sEstimBJInitFMSTR.fltSSTimeoutTime; /* Time to reach steady state before timeout [s]. */
        g_sEstimBJInitCfg.fltSSTimeMin     = g_sEstimBJInitFMSTR.fltSSTimeMin;     /* Minimum time needed for stable behavior [s]. */
        g_sEstimBJInitCfg.fltSSTimeTrans   = g_sEstimBJInitFMSTR.fltSSTimeTrans;   /* Transition time during selection of injected frequency [s]. */
        g_sEstimBJInitCfg.fltDcPiPropGain  = g_sEstimBJInitFMSTR.fltDcPiPropGain;  /* Proportional gain of the speed loops DC controller [-]. */
        g_sEstimBJInitCfg.fltDcPiIntegGain = g_sEstimBJInitFMSTR.fltDcPiIntegGain; /* Integral gain of the speed loops DC controller [-]. */
        g_sEstimBJInitCfg.fltAcPiPropGain  = g_sEstimBJInitFMSTR.fltAcPiPropGain;  /* Proportional gain of the speed loops AC controller [-]. */
        g_sEstimBJInitCfg.fltAcPiIntegGain = g_sEstimBJInitFMSTR.fltAcPiIntegGain; /* Integral gain of the speed loops AC controller [-]. */

        /* Initialize the state variables and set algorithm parameters */
        eEstimBJRetValInit = MCAA_EstimBJInit_FLT(&g_sEstimBJInitCfg, &g_sEstimBJStruct);
        switch(eEstimBJRetValInit)
        {
          case ESTIMBJ_RET_INIT_OK:
              /* Next is BJ state */
              g_sMID.sMIDMeasStatus.ui32AllFinishedMeas &= ~MID_BJ_FINISH;
              M1_MCDRV_PWM3PH_EN(&g_sM1Pwm3ph);
              g_sMID.sMIDMeasStatus.eMIDState = kMID_BJ;
              break;
          
          default:
              /* Next is FAULT state - initialization failed */
              g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_BJ_INIT_FAIL;
              MID_TransAll2Fault(); 
              break;
        }  
    }
}

/*!
 * @brief (general) Any state to MID STOP transition
 *
 * @param None
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransAll2Stop(void)
{
    /* Type the code to do when going to the STOP state */  
                
    /* Disable FOC current loop */
    g_sMidDrive.sFocPMSM.bCurrentLoopOn = FALSE;
    
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    
    /* Reset active flags */
    g_sMID.sMIDAlignment.bActive  = FALSE;
    g_sMID.sMIDPp.bActive         = FALSE;

    /* Clear the measurement trigger if measurement has been finished. */
    if((g_sMID.eMeasurementType == kMID_ElectricalParams) ||
       (g_sMID.eMeasurementType == kMID_PolePairs)        ||
       (g_sMID.eMeasurementType == kMID_MechanicalParams))
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
 * @param None
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransAll2Fault(void)
{ 
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
    g_sMID.bMIDStart = FALSE;
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Fault;
}

/*!
 * @brief (general) MID STOP to CALIB transition
 *
 * @param None
 *
 * @return None
 */
RAM_FUNC_LIB
static void MID_TransStop2Calib(void)
{
    /* Type the code to do when going from the STOP to the RUN state */
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
    
    /* Next is START state */
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
    
    /* Enable FOC current loop */
    g_sMidDrive.sFocPMSM.bCurrentLoopOn = TRUE;

    /* Use OL position for Park transformation. */
    g_sMidDrive.sFocPMSM.bOpenLoop = TRUE;
    g_sMidDrive.sFocPMSM.bPosExtOn = TRUE;

    /* Check range of the measurement configuration. */
    if((sUserMIDMeasConfig.fltAlignId > I_NOMINAL) ||
       (sUserMIDMeasConfig.fltAlignId < 0.0F))
    {
        /* Report Alignment configuration out of range. */
        g_sMID.sMIDMeasStatus.ui32FaultMID |= MID_ALIGN_I_OUT_OF_RANGE;
    }
    else
    {
        /* Set the Align measurement configuration. */
        g_sMID.sMIDAlignment.fltCurrentAlign = sUserMIDMeasConfig.fltAlignId;
        
        /* Set time of alignment */
        g_sMID.sMIDAlignment.ui16AlignDuration = 10000U;
    }
    
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
RAM_FUNC_LIB
void MID_Init_AR(void)
{
    /* Clean the internal parameters. */
    g_sMID.sMotorParams.ui32Pp = 1UL;
    g_sMID.sMotorParams.fltRs  = 0.0F;
    g_sMID.sMotorParams.fltLd  = 0.0F;
    g_sMID.sMotorParams.fltLq  = 0.0F;
    g_sMID.sMotorParams.fltUdt = 0.0F;
    g_sMID.sMotorParams.fltKe  = 0.0F;
    g_sMID.sMotorParams.fltKt  = 0.0F;
    g_sMID.sMotorParams.fltJ   = 0.0F;
    g_sMID.sMotorParams.fltA   = 0.0F;
    g_sMID.sMotorParams.fltB   = 0.0F;

    /* Clear status variables. */
    g_sMID.bMIDStart                          = FALSE;
    g_sMID.sMIDMeasStatus.ui32FaultMID        = 0UL;
    g_sMID.sMIDMeasStatus.ui32AllFinishedMeas = 0UL;
    g_sMID.sMIDMeasStatus.ui32ActFinishedMeas = 0UL;

    /* Set the initial MID state. */
    g_sMID.sMIDMeasStatus.eMIDState = kMID_Stop;
      
    /**** Init FOC structure and pointer to drivers *****/
    /* Type the code to do when in the INIT state */
    g_sMidDrive.sFocPMSM.sIdPiParams.fltInErrK_1 = 0.0F;
    g_sMidDrive.sFocPMSM.sIdPiParams.bLimFlag    = FALSE;

    g_sMidDrive.sFocPMSM.sIqPiParams.fltInErrK_1 = 0.0F;
    g_sMidDrive.sFocPMSM.sIqPiParams.bLimFlag    = FALSE;

    /* PMSM FOC params */
    g_sMidDrive.sFocPMSM.sIdPiParams.fltPGain    = M1_D_KP_GAIN;
    g_sMidDrive.sFocPMSM.sIdPiParams.fltIGain    = M1_D_KI_GAIN;
    g_sMidDrive.sFocPMSM.sIdPiParams.fltUpperLim = M1_U_MAX;
    g_sMidDrive.sFocPMSM.sIdPiParams.fltLowerLim = -M1_U_MAX;

    g_sMidDrive.sFocPMSM.sIqPiParams.fltPGain    = M1_Q_KP_GAIN;
    g_sMidDrive.sFocPMSM.sIqPiParams.fltIGain    = M1_Q_KI_GAIN;
    g_sMidDrive.sFocPMSM.sIqPiParams.fltUpperLim = M1_U_MAX;
    g_sMidDrive.sFocPMSM.sIqPiParams.fltLowerLim = -M1_U_MAX;

    g_sMidDrive.sFocPMSM.ui16SectorSVM     = M1_SVM_SECTOR_DEFAULT;
    g_sMidDrive.sFocPMSM.fltDutyCycleLimit = M1_CLOOP_LIMIT;


    g_sMidDrive.sFocPMSM.fltUDcBus                     = 0.0F;
    g_sMidDrive.sFocPMSM.fltUDcBusFilt                 = 0.0F;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB0 = M1_UDCB_IIR_B0;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB1 = M1_UDCB_IIR_B1;
    g_sMidDrive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltA1 = M1_UDCB_IIR_A1;
    /* Filter init not to enter to fault */
    g_sMidDrive.sFocPMSM.sUDcBusFilter.fltFltBfrX[0] = (M1_U_DCB_UNDERVOLTAGE / 2.0F) + (M1_U_DCB_OVERVOLTAGE / 2.0F);
    g_sMidDrive.sFocPMSM.sUDcBusFilter.fltFltBfrY[0] = (M1_U_DCB_UNDERVOLTAGE / 2.0F) + (M1_U_DCB_OVERVOLTAGE / 2.0F);
    
    /* Timing control and general variables */
    g_sMidDrive.ui16CounterState           = 0U;
    g_sMidDrive.ui16TimeCalibration        = M1_CALIB_DURATION;             /* Multiply due to absence slow loop in MID */
    
    /* fault set to init states */
    MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdCaptured);
    MID_FAULT_CLEAR_ALL(g_sMidDrive.sFaultIdPending);

    /* fault thresholds */
    g_sMidDrive.sFaultThresholds.fltUDcBusOver     = M1_U_DCB_OVERVOLTAGE;
    g_sMidDrive.sFaultThresholds.fltUDcBusUnder    = M1_U_DCB_UNDERVOLTAGE;
    g_sMidDrive.sFaultThresholds.fltUDcBusTrip     = M1_U_DCB_TRIP;
    g_sMidDrive.sFaultThresholds.fltSpeedOver      = M1_N_OVERSPEED;
    g_sMidDrive.sFaultThresholds.fltSpeedMin       = M1_N_MIN;
    g_sMidDrive.sFaultThresholds.fltSpeedNom       = M1_N_NOM;
    g_sMidDrive.sFaultThresholds.fltUqBemf         = M1_E_BLOCK_TRH;
    g_sMidDrive.sFaultThresholds.ui16BlockedPerNum = M1_E_BLOCK_PER;

    /* Defined scaling for FreeMASTER */
    g_fltMIDvoltageScale         = M1_U_MAX;
    g_fltMIDcurrentScale         = M1_I_MAX;
    g_fltMIDDCBvoltageScale      = M1_U_DCB_MAX;
    g_fltMIDspeedScale           = M1_N_MAX;
    g_fltMIDspeedAngularScale    = (60.0F / (M1_MOTOR_PP * 2.0F * FLOAT_PI));
    g_fltMIDspeedMechanicalScale = (60.0F / (2.0F * FLOAT_PI));

    /* Clear rest of variables  */
    MID_ClearFOCVariables();

    /* Init sensors/actuators pointers */
    /* For PWM driver */
    g_sM1Pwm3ph.psUABC = &(g_sMidDrive.sFocPMSM.sDutyABC);
    
    /* For ADC driver */
    g_sM1Curr3phDcBus.pf16UDcBus     = &(g_sMidDrive.sFocPMSM.f16UDcBus);
    g_sM1Curr3phDcBus.psIABC         = &(g_sMidDrive.sFocPMSM.sIABCFrac);
    g_sM1Curr3phDcBus.pui16SVMSector = &(g_sMidDrive.sFocPMSM.ui16SectorSVM);
    g_sM1Curr3phDcBus.pui16AuxChan   = &(g_sMidDrive.f16AdcAuxSample);
    
    /* Disable PWM output */
    M1_MCDRV_PWM3PH_DIS(&g_sM1Pwm3ph);
}

/*!
 * MID fast-loop process function.
 */
RAM_FUNC_LIB
void MID_ProcessFast_FL(void)
{
    /* Read measurements from FOC motor-control module. */
    MID_ReadSignals();
    
    /* Detects faults */
    MID_FaultDetection();
    
    /* If fault flag */
    if (g_sMidDrive.sFaultIdPending  > 0U)
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
    /* Clear the start trigger to avoid MID restart. */
    g_sMID.bMIDStart = FALSE;

    /* Check whether the pole-pair measurement is ongoing. */
    if(TRUE == g_sMID.sMIDPp.bActive)
    {
        /* Stop the Pp assistant */
        g_sMID.sMIDPp.ui16PpDetermined = TRUE;
        
        /* Indicate finished measurement. */
        g_sMID.sMIDMeasStatus.ui32ActFinishedMeas |= MID_PP_FINISH;
    }

    /* Go to STOP state immediately */
    MID_TransAll2Stop();
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
    if(psMotorParams->fltRs != 0.0F)
    {
        g_sMID.sMotorParams.fltRs = psMotorParams->fltRs;
    }
    /* If Ld non-zero. */
    if(psMotorParams->fltLd != 0.0F)
    {
        g_sMID.sMotorParams.fltLd = psMotorParams->fltLd;
    }
    /* If Lq non-zero. */
    if(psMotorParams->fltLq != 0.0F)
    {
        g_sMID.sMotorParams.fltLq = psMotorParams->fltLq;
    }
    /* If Udt non-zero. */
    if(psMotorParams->fltUdt != 0.0F)
    {
        g_sMID.sMotorParams.fltUdt = psMotorParams->fltUdt;
    }
    /* If Ke non-zero. */
    if(psMotorParams->fltKe != 0.0F)
    {
        g_sMID.sMotorParams.fltKe = psMotorParams->fltKe;
    }
    /* If Kt non-zero. */
    if(psMotorParams->fltKt != 0.0F)
    {
        g_sMID.sMotorParams.fltKt = psMotorParams->fltKt;
    }
    /* If J non-zero. */
    if(psMotorParams->fltJ != 0.0F)
    {
        g_sMID.sMotorParams.fltJ  = psMotorParams->fltJ;
    }
    /* If A non-zero. */
    if(psMotorParams->fltA != 0.0F)
    {
        g_sMID.sMotorParams.fltA  = psMotorParams->fltA;
    }
    /* If B non-zero. */
    if(psMotorParams->fltB != 0.0F)
    {
        g_sMID.sMotorParams.fltB  = psMotorParams->fltB;
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
    psMotorParams->fltRs  = g_sMID.sMotorParams.fltRs;
    psMotorParams->fltLd  = g_sMID.sMotorParams.fltLd;
    psMotorParams->fltLq  = g_sMID.sMotorParams.fltLq;
    psMotorParams->fltUdt = g_sMID.sMotorParams.fltUdt;
    psMotorParams->fltKe  = g_sMID.sMotorParams.fltKe;
    psMotorParams->fltKt  = g_sMID.sMotorParams.fltKt;
    psMotorParams->fltJ   = g_sMID.sMotorParams.fltJ;
    psMotorParams->fltA   = g_sMID.sMotorParams.fltA;
    psMotorParams->fltB   = g_sMID.sMotorParams.fltB;
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
RAM_FUNC_LIB
uint16_t MID_GetActualState(void)
{
  return ((uint16_t)g_sMID.sMIDMeasStatus.eMIDState);
}

/*                             STATIC FUNCTIONS                               */
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
    g_sMidDrive.sFocPMSM.sIABC.fltA             = 0.0F;
    g_sMidDrive.sFocPMSM.sIABC.fltB             = 0.0F;
    g_sMidDrive.sFocPMSM.sIABC.fltC             = 0.0F;
    g_sMidDrive.sFocPMSM.sIAlBe.fltAlpha        = 0.0F;
    g_sMidDrive.sFocPMSM.sIAlBe.fltBeta         = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQ.fltD              = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQ.fltQ              = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQReq.fltD           = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQReq.fltQ           = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQError.fltD         = 0.0F;
    g_sMidDrive.sFocPMSM.sIDQError.fltQ         = 0.0F;
    g_sMidDrive.sFocPMSM.sDutyABC.f16A          = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16B          = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sDutyABC.f16C          = FRAC16(0.5);
    g_sMidDrive.sFocPMSM.sUAlBeReq.fltAlpha     = 0.0F;
    g_sMidDrive.sFocPMSM.sUAlBeReq.fltBeta      = 0.0F;
    g_sMidDrive.sFocPMSM.sUDQReq.fltD           = 0.0F;
    g_sMidDrive.sFocPMSM.sUDQReq.fltQ           = 0.0F;
    g_sMidDrive.sFocPMSM.sAnglePosEl.fltSin     = 0.0F;
    g_sMidDrive.sFocPMSM.sAnglePosEl.fltCos     = 0.0F;
    g_sMidDrive.sFocPMSM.sIdPiParams.bLimFlag   = FALSE;
    g_sMidDrive.sFocPMSM.sIqPiParams.bLimFlag   = FALSE;
    g_sMidDrive.sFocPMSM.sIdPiParams.fltIAccK_1 = 0.0F;
    g_sMidDrive.sFocPMSM.sIqPiParams.fltIAccK_1 = 0.0F;
    g_sMidDrive.sFocPMSM.bIdPiStopInteg         = FALSE;
    g_sMidDrive.sFocPMSM.bIqPiStopInteg         = FALSE;
}

/*!
 * @brief Fault detention routine - check various faults
 *
 * @param void  No input parameter
 *
 * @return None
 */
RAM_FUNC_LIB
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
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_I_DCBUS_OVER);

    /* Fault:   DC-bus over-voltage */
    if (g_sMidDrive.sFocPMSM.fltUDcBusFilt > g_sMidDrive.sFaultThresholds.fltUDcBusOver)
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_U_DCBUS_OVER);

    /* Fault:   DC-bus under-voltage */
    if (g_sMidDrive.sFocPMSM.fltUDcBusFilt < g_sMidDrive.sFaultThresholds.fltUDcBusUnder)
        MID_FAULT_SET(g_sMidDrive.sFaultIdPending, MID_FAULT_U_DCBUS_UNDER);

    /* Pass fault to Fault ID Captured */
    g_sMidDrive.sFaultIdCaptured |= g_sMidDrive.sFaultIdPending;
}

/*!
 * Function reads voltages, currents, etc. for MID. Called before MID state
 * machine. User defined.
 */
RAM_FUNC_LIB
static void MID_ReadSignals(void)
{
    frac16_t f16PosElPark;

    /* get all adc samples - DC-bus voltage, current, bemf and aux sample */
    M1_MCDRV_CURR_3PH_VOLT_DCB_GET(&g_sM1Curr3phDcBus); 
    
    /* Convert phase currents from fractional measured values to float */
    g_sMidDrive.sFocPMSM.sIABC.fltA = MLIB_ConvSc_FLTsf(g_sMidDrive.sFocPMSM.sIABCFrac.f16A, g_fltMIDcurrentScale);
    g_sMidDrive.sFocPMSM.sIABC.fltB = MLIB_ConvSc_FLTsf(g_sMidDrive.sFocPMSM.sIABCFrac.f16B, g_fltMIDcurrentScale);
    g_sMidDrive.sFocPMSM.sIABC.fltC = MLIB_ConvSc_FLTsf(g_sMidDrive.sFocPMSM.sIABCFrac.f16C, g_fltMIDcurrentScale);
    
    /* 3-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_Clark_FLT(&g_sMidDrive.sFocPMSM.sIABC, &g_sMidDrive.sFocPMSM.sIAlBe);
    
    /* Convert voltages from fractional measured values to float */
    g_sMidDrive.sFocPMSM.fltUDcBus = MLIB_ConvSc_FLTsf(g_sMidDrive.sFocPMSM.f16UDcBus, g_fltMIDDCBvoltageScale);                    
    /* Sampled DC-Bus voltage filter */
    g_sMidDrive.sFocPMSM.fltUDcBusFilt = GDFLIB_FilterIIR1_FLT(g_sMidDrive.sFocPMSM.fltUDcBus, &g_sMidDrive.sFocPMSM.sUDcBusFilter);
    
    /* Decide which position will be used for Park transformation. */
    if(g_sMidDrive.sFocPMSM.bPosExtOn)  
    {
        f16PosElPark = g_sMidDrive.sFocPMSM.f16PosElExt; 
    }
    else
    {
        f16PosElPark = g_sMidDrive.sFocPMSM.f16PosElEst;     
    }
     
    /* Position angle of the last PWM update */
    g_sMidDrive.sFocPMSM.sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)f16PosElPark);      
    g_sMidDrive.sFocPMSM.sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)f16PosElPark);
    
    /* 2-phase to 2-phase transformation to rotary ref. frame */
    GMCLIB_Park_FLT(&g_sMidDrive.sFocPMSM.sIAlBe,    &g_sMidDrive.sFocPMSM.sAnglePosEl, &g_sMidDrive.sFocPMSM.sIDQ);
    GMCLIB_Park_FLT(&g_sMidDrive.sFocPMSM.sUAlBeReq, &g_sMidDrive.sFocPMSM.sAnglePosEl, &g_sMidDrive.sFocPMSM.sUDQReq);

    /* For open loop control enabled parallel running of observer and FOC
     * Open loop electrical position passed to rest of FOC */
    if (g_sMidDrive.sFocPMSM.bOpenLoop || g_sMidDrive.sFocPMSM.bPosExtOn)
    {
        g_sMidDrive.sFocPMSM.sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)g_sMidDrive.sFocPMSM.f16PosElExt);
        g_sMidDrive.sFocPMSM.sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)g_sMidDrive.sFocPMSM.f16PosElExt);
        GMCLIB_Park_FLT(&g_sMidDrive.sFocPMSM.sIAlBe, &g_sMidDrive.sFocPMSM.sAnglePosEl, &g_sMidDrive.sFocPMSM.sIDQ);
    }
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
        /* Current loop if enabled */
        if (g_sMidDrive.sFocPMSM.bCurrentLoopOn)
        {       
            /* Open loop electrical position */
            g_sMidDrive.sFocPMSM.sAnglePosEl.fltSin = GFLIB_Sin_FLTa((acc32_t)g_sMidDrive.sFocPMSM.f16PosElExt);
            g_sMidDrive.sFocPMSM.sAnglePosEl.fltCos = GFLIB_Cos_FLTa((acc32_t)g_sMidDrive.sFocPMSM.f16PosElExt);
            GMCLIB_Park_FLT(&g_sMidDrive.sFocPMSM.sIAlBe, &g_sMidDrive.sFocPMSM.sAnglePosEl, &g_sMidDrive.sFocPMSM.sIDQ);
            
            /* D current error calculation */
            g_sMidDrive.sFocPMSM.sIDQError.fltD = MLIB_Sub_FLT(g_sMidDrive.sFocPMSM.sIDQReq.fltD, g_sMidDrive.sFocPMSM.sIDQ.fltD);

            /* Q current error calculation */
            g_sMidDrive.sFocPMSM.sIDQError.fltQ = MLIB_Sub_FLT(g_sMidDrive.sFocPMSM.sIDQReq.fltQ, g_sMidDrive.sFocPMSM.sIDQ.fltQ);

            /*** D - controller limitation calculation ***/
            g_sMidDrive.sFocPMSM.sIdPiParams.fltLowerLim = MLIB_MulNeg_FLT(g_sMidDrive.sFocPMSM.fltDutyCycleLimit, g_sMidDrive.sFocPMSM.fltUDcBusFilt);
            g_sMidDrive.sFocPMSM.sIdPiParams.fltUpperLim = MLIB_Mul_FLT(g_sMidDrive.sFocPMSM.fltDutyCycleLimit, g_sMidDrive.sFocPMSM.fltUDcBusFilt);

            /* D current PI controller */
            g_sMidDrive.sFocPMSM.sUDQReq.fltD =
                GFLIB_CtrlPIpAW_FLT(g_sMidDrive.sFocPMSM.sIDQError.fltD, &g_sMidDrive.sFocPMSM.bIdPiStopInteg, &g_sMidDrive.sFocPMSM.sIdPiParams);

            /*** Q - controller limitation calculation ***/
            g_sMidDrive.sFocPMSM.sIqPiParams.fltUpperLim =
                GFLIB_Sqrt_FLT(g_sMidDrive.sFocPMSM.sIdPiParams.fltUpperLim * g_sMidDrive.sFocPMSM.sIdPiParams.fltUpperLim -
                               g_sMidDrive.sFocPMSM.sUDQReq.fltD * g_sMidDrive.sFocPMSM.sUDQReq.fltD);
            g_sMidDrive.sFocPMSM.sIqPiParams.fltLowerLim = MLIB_Neg_FLT(g_sMidDrive.sFocPMSM.sIqPiParams.fltUpperLim);

            /* Q current PI controller */
            g_sMidDrive.sFocPMSM.sUDQReq.fltQ =
                GFLIB_CtrlPIpAW_FLT(g_sMidDrive.sFocPMSM.sIDQError.fltQ, &g_sMidDrive.sFocPMSM.bIqPiStopInteg, &g_sMidDrive.sFocPMSM.sIqPiParams);
        
            /* 2-phase to 2-phase transformation to stationary ref. frame */
            GMCLIB_ParkInv_FLT(&g_sMidDrive.sFocPMSM.sUDQReq, &g_sMidDrive.sFocPMSM.sAnglePosEl, &g_sMidDrive.sFocPMSM.sUAlBeReq);
        }        
        
        /* DCBus ripple elimination */
        GMCLIB_ElimDcBusRipFOC_F16ff(g_sMidDrive.sFocPMSM.fltUDcBusFilt, &g_sMidDrive.sFocPMSM.sUAlBeReq, &g_sMidDrive.sFocPMSM.sUAlBeCompFrac);

        /* space vector modulation */
        g_sMidDrive.sFocPMSM.ui16SectorSVM = GMCLIB_SvmStd_F16(&g_sMidDrive.sFocPMSM.sUAlBeCompFrac, &g_sMidDrive.sFocPMSM.sDutyABC);  
    }
    
    /* PWM peripheral update */
    M1_MCDRV_PWM3PH_SET(&g_sM1Pwm3ph);   
    
    /* Set current sensor for sampling */
    M1_MCDRV_CURR_3PH_CHAN_ASSIGN(&g_sM1Curr3phDcBus);     
}