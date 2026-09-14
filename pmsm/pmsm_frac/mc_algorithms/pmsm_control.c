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

#include "pmsm_control.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief PMSM Openloop control.
 *
 * This function is used for Openloop control mode.
 *
 * @param psOpenloop  The pointer of the Openloop structure
 *
 * @return None
 */
void MCS_PMSMOpenloop(mcs_openloop_t *psOpenloop){
	psOpenloop->f16PosElExt = MLIB_Add_F16(
		psOpenloop->f16Theta,
		GFLIB_Integrator_F16(
            psOpenloop->f16FreqReq,
			&psOpenloop->sFreqIntegrator
		)
	);

}

/*!
 * @brief PMSM field oriented current control.
 *
 * This function is used to compute PMSM field oriented current control.
 *
 * @param psFocPMSM     The pointer of the PMSM FOC structure
 *
 * @return None
 */
void MCS_PMSMFocCtrl(mcs_pmsm_foc_t *psFocPMSM)
{
    /* pass electrical position from outside function if enabled else estimated
     * electrical position is selected */
    if (psFocPMSM->bPosExtOn)
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElExt;
    }
    else
    {
        psFocPMSM->f16PosEl = psFocPMSM->f16PosElEst;
    }

    /* Position angle of the last PWM update */
    GFLIB_SinCos_F16(psFocPMSM->f16PosElEst, &psFocPMSM->sAnglePosEl);

    /* 3-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_Clark_F16(&psFocPMSM->sIABC, &psFocPMSM->sIAlBe);

    /* 2-phase to 2-phase transformation to rotary ref. frame */
    GMCLIB_Park_F16(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
    GMCLIB_Park_F16(&psFocPMSM->sUAlBeReq, &psFocPMSM->sAnglePosEl, &psFocPMSM->sUDQEst);

    /* BEMF observer in DQ system */
    psFocPMSM->sBemfObsrv.f16Error = AMCLIB_PMSMBemfObsrvDQ_F16(&psFocPMSM->sIDQ, &psFocPMSM->sUDQEst, psFocPMSM->f16SpeedElEst, &psFocPMSM->sBemfObsrv);

    /* Tracking observer calculation */
    psFocPMSM->f16PosElEst = AMCLIB_TrackObsrv_F16(psFocPMSM->sBemfObsrv.f16Error, &psFocPMSM->sTo);

    /* Speed estimation filter  */
    psFocPMSM->f16SpeedElEst =
        GDFLIB_FilterIIR1_F16(MLIB_Conv_F16l(psFocPMSM->sTo.f32Speed), &psFocPMSM->sSpeedElEstFilt);

    /* for open loop control enabled parallel running of observer and FOC
     * open loop electrical position passed to rest of FOC */
    if (psFocPMSM->bOpenLoop || psFocPMSM->bPosExtOn)
    {
		GFLIB_SinCos_F16(psFocPMSM->f16PosEl, &psFocPMSM->sAnglePosEl);
        GMCLIB_Park_F16(&psFocPMSM->sIAlBe, &psFocPMSM->sAnglePosEl, &psFocPMSM->sIDQ);
    }

    /* perform current control loop if enabled */
    if (psFocPMSM->bCurrentLoopOn)
    {
        /* D current error calculation */
        psFocPMSM->sIDQError.f16D = MLIB_SubSat_F16(psFocPMSM->sIDQReq.f16D, psFocPMSM->sIDQ.f16D);

#if defined(Q_CURRENT_ZC_FILTER)     
        /* Zero cancellation filter */
        psFocPMSM->sIDQReqFilt.f16Q = GDFLIB_FilterIIR1_F16(psFocPMSM->sIDQReq.f16Q, &psFocPMSM->sIqReqZCFilter);
        
        /* Q current error calculation */
        psFocPMSM->sIDQError.f16Q = MLIB_SubSat_F16(psFocPMSM->sIDQReqFilt.f16Q, psFocPMSM->sIDQ.f16Q);
#else
        psFocPMSM->sIDQError.f16Q = MLIB_SubSat_F16(psFocPMSM->sIDQReq.f16Q, psFocPMSM->sIDQ.f16Q);
#endif

        /*** D - controller limitation calculation ***/
        psFocPMSM->sIdPiParams.f16UpperLim = MLIB_Mul_F16(psFocPMSM->f16DutyCycleLimit, psFocPMSM->f16UDcBusFilt);
        psFocPMSM->sIdPiParams.f16LowerLim = MLIB_Neg_F16(psFocPMSM->sIdPiParams.f16UpperLim);

        /* D current PI controller */
        psFocPMSM->sUDQReq.f16D =
            GFLIB_CtrlPIpAW_F16(psFocPMSM->sIDQError.f16D, &psFocPMSM->bIdPiStopInteg, &psFocPMSM->sIdPiParams);

        /*** Q - controller limitation calculation ***/
        psFocPMSM->sIqPiParams.f16UpperLim = GFLIB_Sqrt_F16(
            MLIB_Sub_F16(MLIB_Mul_F16(psFocPMSM->sIdPiParams.f16UpperLim, psFocPMSM->sIdPiParams.f16UpperLim),
                         MLIB_Mul_F16(psFocPMSM->sUDQReq.f16D, psFocPMSM->sUDQReq.f16D)));
        psFocPMSM->sIqPiParams.f16LowerLim = MLIB_Neg_F16(psFocPMSM->sIqPiParams.f16UpperLim);

        /* Q current PI controller */
        psFocPMSM->sUDQReq.f16Q =
            GFLIB_CtrlPIpAW_F16(psFocPMSM->sIDQError.f16Q, &psFocPMSM->bIqPiStopInteg, &psFocPMSM->sIqPiParams);
    }

    /* 2-phase to 2-phase transformation to stationary ref. frame */
    GMCLIB_ParkInv_F16(&psFocPMSM->sUDQReq, &psFocPMSM->sAnglePosEl, &psFocPMSM->sUAlBeReq);

    /* DCBus ripple elimination */
    GMCLIB_ElimDcBusRipFOC_F16(psFocPMSM->f16UDcBusFilt, &psFocPMSM->sUAlBeReq, &psFocPMSM->sUAlBeComp);

    /* space vector modulation */
    psFocPMSM->ui16SectorSVM = GMCLIB_SvmStd_F16(&psFocPMSM->sUAlBeComp, &psFocPMSM->sDutyABC);
}

/*!
 * @brief PMSM field oriented speed control.
 *
 * This function is used to compute PMSM field oriented speed control.
 *
 * @param psFocPMSM     The pointer of the PMSM FOC structure
 * @param psSpeed       The pointer of the PMSM speed structure
 *
 * @return None
 */
void MCS_PMSMFocCtrlSpeed(mcs_pmsm_foc_t *psFocPMSM, mcs_speed_t *psSpeed)
{
    /* Speed saturation flag given by the Q current controller saturation flag and speed controller saturation flag */
    psSpeed->bSpeedPiStopInteg = (psSpeed->sSpeedPiParams.bLimFlag || psFocPMSM->sIqPiParams.bLimFlag) &&
                                 (MLIB_AbsSat_F16(psSpeed->f16SpeedCmd) >= MLIB_AbsSat_F16(psSpeed->f16SpeedFilt));

    /* Speed ramp generation */
    psSpeed->f16SpeedRamp = MLIB_Conv_F16l(GFLIB_Ramp_F32(MLIB_Conv_F32s(psSpeed->f16SpeedCmd), &psSpeed->sSpeedRampParams));

    if(psSpeed->bSpeedZCOn)
    {
        /* Speed zero cancellation filter - required speed is fltSpeedRamp */
        psSpeed->f16SpeedCmdFilt  = GDFLIB_FilterIIR1_F16(psSpeed->f16SpeedRamp, &psSpeed->sSpeedCmdZCFilter); 
        /* Speed error calculation */
        psSpeed->f16SpeedError = MLIB_SubSat_F16(psSpeed->f16SpeedCmdFilt, psSpeed->f16SpeedFilt);
    }
    else
    {
        /* Speed error calculation */
        psSpeed->f16SpeedError = MLIB_SubSat_F16(psSpeed->f16SpeedRamp, psSpeed->f16SpeedFilt);
    }

    /* Desired current by the speed PI controller */
    psFocPMSM->sIDQReq.f16Q =
        GFLIB_CtrlPIpAW_F16(psSpeed->f16SpeedError, &psSpeed->bSpeedPiStopInteg, &psSpeed->sSpeedPiParams);
}

/*!
 * @brief PMSM field oriented position control.
 *
 * This function is used to compute PMSM field oriented position control.
 *
 * @param psSpeed       The pointer of the PMSM position structure
 *
 * @return None
 */
void MCS_PMSMFocCtrlPosition(mcs_position_t *psPosition)
{
    /* Position error calculation */
    psPosition->a32PositionError = MLIB_Sub_F32(psPosition->a32PositionCmd, psPosition->a32Position);
    
    /* If Feed-Forward is on */
    if(psPosition->bFeedFrwdOn)
    {  
        psPosition->a32PositionCmdDelta = MLIB_Sub_F32(psPosition->a32PositionCmd, psPosition->a32PositionCmd_stored);
      
//      /* Limit fltPositionCmdDelta due to very high value after derivation */
//      GFLIB_Limit_FLT(psPosition->fltPositionCmdDelta, -0.5F, 0.5F);
      
        psPosition->a32FirstDerivation = MLIB_DivSat_A32as(psPosition->a32PositionCmdDelta, psPosition->f16SpeedLoopTs);
        
        psPosition->a32FirstDerivationDelta = MLIB_Sub_F32((psPosition->a32FirstDerivation), (psPosition->a32FirstDerivation_stored));
  
//      /* Limit fltFirstDerivationDelta due to very high value after derivation */
//      GFLIB_Limit_FLT(psPosition->fltFirstDerivationDelta, -0.8F, 0.8F); 
        
        psPosition->a32SecondDerivation = MLIB_DivSat_A32as(psPosition->a32FirstDerivationDelta, (psPosition->f16SpeedLoopTs));

        psPosition->a32SpeedFeedFrwdK1 = MLIB_Mul_A32(psPosition->a32FirstDerivation, psPosition->a32FeedFrwdK1);
        psPosition->a32SpeedFeedFrwdK2 = MLIB_Mul_A32(psPosition->a32SecondDerivation, psPosition->a32FeedFrwdK2);
      
    }
    else
    {
      psPosition->a32SpeedFeedFrwdK1 = 0;
      psPosition->a32SpeedFeedFrwdK2 = 0;
    }
      
    psPosition->f32SpeedFeedFrwdK1 = (frac32_t)((psPosition->a32SpeedFeedFrwdK1) << 0);
    psPosition->f32SpeedFeedFrwdK2 = (frac32_t)((psPosition->a32SpeedFeedFrwdK2) << 0);  
    
    psPosition->f16SpeedFeedFrwdK1 = (frac16_t)(psPosition->f32SpeedFeedFrwdK1 >> 16U);
    psPosition->f16SpeedFeedFrwdK2 = (frac16_t)(psPosition->f32SpeedFeedFrwdK2 >> 16U);
    
    /* Position P controller output */
    psPosition->f32SpeedReq = MLIB_Add4_F32(GFLIB_CtrlPIpAW_F32_FC((frac32_t)psPosition->a32PositionError, &psPosition->bPositionPiStopInteg, &psPosition->sPositionPiParams), psPosition->f32SpeedFeedFrwdK1, psPosition->f32SpeedFeedFrwdK2, 0U);
    
    /* Convert f32 to f16 */
    psPosition->f16SpeedReq = (frac16_t)((psPosition->f32SpeedReq) >> 16U);

    /* Store actual fltPositionCmd needed for next derivation */
    psPosition->a32PositionCmd_stored = psPosition->a32PositionCmd;
    /* Store actual fltFirstDerivation needed for next derivation */
    psPosition->a32FirstDerivation_stored = psPosition->a32FirstDerivation;
    
    /* Speed saturation flag given by the Q current controller saturation flag and speed controller saturation flag */
    psPosition->bSpeedPiStopInteg = (bool_t)((psPosition->sSpeedPiParams.bLimFlag | psPosition->bIqPiLimFlag) &
    		(bool_t)(MLIB_AbsSat_F16(psPosition->f16SpeedReq) >= MLIB_AbsSat_F16(psPosition->f16SpeedFilt)));
    
    /* Speed zero cancellation filter */
    psPosition->f16SpeedReqFilt  = GDFLIB_FilterIIR1_F16(psPosition->f16SpeedReq, &psPosition->sSpeedReqZCFilter);
    
    /* Speed error calculation */
    psPosition->f16SpeedError = MLIB_SubSat_F16(psPosition->f16SpeedReqFilt, psPosition->f16SpeedFilt);
    
    /* Desired current by the speed PI controller */
    psPosition->f16IqReq =  GFLIB_CtrlPIpAW_F16(psPosition->f16SpeedError, &psPosition->bSpeedPiStopInteg, &psPosition->sSpeedPiParams);    
   
}


frac32_t GFLIB_CtrlPIpAW_F32_FC(frac32_t f32InErr,
                                const bool_t *pbStopIntegFlag, 
                                GFLIB_CTRL_PI_P_AW_T_F32 *psParam)
{
    register acc64_t a64Proportional;
    register acc64_t a64Integral;
    register acc64_t a64IntegralX2;
    register acc64_t a64Out;    
    register acc32_t a32UpperLimit;
    register acc32_t a32LowerLimit;
    
    /* Proportional part calculation */
    a64Proportional = (acc64_t)((int64_t) f32InErr * (int64_t) psParam->a32PGain)>>15;
     
    /* Stop integration if flag is set */
    if (*pbStopIntegFlag)                                                /* if stop flag is set */
    {                                                                    
        f32InErr = 0;                                                    /* integration will be stopped     */
    }  
    
    /* Integral part calculation */
    a64Out = (acc64_t)f32InErr + (acc64_t)psParam->f32InErrK_1;     /* current + previous input shifted right to avoid overflow */
    a64IntegralX2 = (acc64_t)(((int64_t) a64Out * (int64_t) psParam->a32IGain)>>17);
    a64Integral = a64IntegralX2 + ((acc64_t)psParam->f32IAccK_1 >> 1);   /* integral part    */

    a32UpperLimit = (frac32_t)psParam->f32UpperLim ;      /* Calculation of limits in 32-bit */
    a32LowerLimit = (frac32_t)psParam->f32LowerLim ;                 
    /* Intergral part limitation */
    if (a64Integral >= (acc64_t)a32UpperLimit)                 /* limitation of integral part      */
    {
        a64Integral = (acc64_t)a32UpperLimit;
    }
    else
    {
        if(a64Integral <= (acc64_t)a32LowerLimit)
        {
            a64Integral = (acc64_t)a32LowerLimit;
        }
    }  
    psParam->f32IAccK_1 = (frac32_t)a64Integral << 1; /* Storing state value of the integral */
    a64Integral <<=1;
    a64Out = a64Proportional + a64Integral ;          /* Controller output calculation by adding the P and I terms */
                                                                         
    /* Output limitation */
    psParam->bLimFlag = 0;                            /* Clear flag and set later in case of limitation */                      
    if (a64Out >= (acc64_t)a32UpperLimit)
    {                                                 /* Limitation PI output (after addition of P and I terms) */
        a64Out = (acc64_t)a32UpperLimit;              
        psParam->bLimFlag = 1;                        /* Storing of output limitation flag */
    }                                                 
    else                                              
    {                                                 
        if(a64Out <= (acc64_t)a32LowerLimit)          
        {                                             
            a64Out = (acc64_t)a32LowerLimit;          
            psParam->bLimFlag = 1;                    /* Storing of output limitation flag */
        }                                             
    }           
                       
    psParam->f32InErrK_1 = (frac32_t)f32InErr;        /* Updating controller input state variables */
	
    /* return controller output */
    return((frac32_t)(a64Out));
}



/*!
 * @brief PMSM 2-step rotor alignment - 120deg in first step and 0deg in second.
 *
 * This function is used for alignment rotor in two steps - 120deg in first step and 0deg in second
 *
 * @param psFocPMSM     The pointer of the PMSM FOC structure
 * @param psAlignment   The pointer of the motor control alignment structure
 *
 * @return None
 */
void MCS_PMSMAlignment(mcs_pmsm_foc_t *psFocPMSM, mcs_alignment_t *psAlignment)
{
    /* first half duration time is position set to 120 degree */
    if (psAlignment->ui16TimeHalf > 0U)
    {
        psFocPMSM->f16PosElExt = FRAC16(120.0 / 180.0);
        psAlignment->ui16TimeHalf--;
    }
    else
    {
        psFocPMSM->f16PosElExt = FRAC16(0.0);
    }
}

/*!
 * @brief PMSM Open Loop Start-up
 *
 * This function is used to PMSM Open Loop Start-up
 *
 * @param psStartUp     The pointer of the PMSM open loop start up parameters structure
 *
 * @return None
 */
void MCS_PMSMOpenLoopStartUp(mcs_pmsm_startup_t *psStartUp)
{
    /* Open loop startup speed ramp */
    psStartUp->f16SpeedRampOpenLoop =
        MLIB_Conv_F16l(GFLIB_Ramp_F32(MLIB_Conv_F32s(psStartUp->f16SpeedReq), &psStartUp->sSpeedRampOpenLoopParams));

    /* generation of open loop position from the required speed */
    psStartUp->f16PosGen = GFLIB_Integrator_F16(psStartUp->f16SpeedRampOpenLoop, &psStartUp->sSpeedIntegrator);

    /* position merging starts above merging speed threshold*/
    if (MLIB_AbsSat_F16(psStartUp->f16SpeedRampOpenLoop) >= psStartUp->f16SpeedCatchUp)
    {
        /* increment position merging coefficient */
        psStartUp->f16RatioMerging = MLIB_AddSat_F16(psStartUp->f16RatioMerging, psStartUp->f16CoeffMerging);

        /* merging equation */
        psStartUp->f16PosMerged = MLIB_Add_F16(
            psStartUp->f16PosGen,
            MLIB_Mul_F16(MLIB_Sub_F16(psStartUp->f16PosEst, psStartUp->f16PosGen), psStartUp->f16RatioMerging));
    }
    else
    {
        psStartUp->f16PosMerged = psStartUp->f16PosGen;
    }

    /* clear open loop flag */
    if (psStartUp->f16RatioMerging == FRAC16(1.0))
    {
        psStartUp->bOpenLoop = FALSE;
    }
}

/*!
 * @brief PMSM scalar control, voltage is set based on required speed
 *
 * This function is used for alignment rotor in two steps - 120deg in first step and 0deg in second
 *
 * @param psFocPMSM      The pointer of the PMSM FOC structure
 * @param psScalarPMSM   The pointer of the PMSM scalar control structure
 *
 * @return None
 */
void MCS_PMSMScalarCtrl(mcs_pmsm_foc_t *psFocPMSM, mcs_pmsm_scalar_ctrl_t *psScalarPMSM)
{
    /* this part of code is executed when scalar control is turned-on */
    /* frequency ramp */
    psScalarPMSM->f16FreqRamp =
        MLIB_Conv_F16l(GFLIB_Ramp_F32(MLIB_Conv_F32s(psScalarPMSM->f16FreqCmd), &psScalarPMSM->sFreqRampParams));

    /* voltage calculation */
    psScalarPMSM->sUDQReq.f16Q = (frac16_t)(MLIB_ShLSat_F16(
        MLIB_Mul_F16(psScalarPMSM->f16VHzGain, psScalarPMSM->f16FreqRamp), (uint16_t)(psScalarPMSM->i16VHzGainShift)));
    psScalarPMSM->sUDQReq.f16D = 0;
    
    /* voltage limitation to f16UqMin */
    if (psScalarPMSM->sUDQReq.f16Q >= FRAC16(0.0))
    {
    	psScalarPMSM->sUDQReq.f16Q = (frac16_t)(GFLIB_LowerLimit_F16(psScalarPMSM->sUDQReq.f16Q,
    	                                                                     psScalarPMSM->f16UqMin));
    }
    else
    {
    	psScalarPMSM->sUDQReq.f16Q = (frac16_t)(GFLIB_UpperLimit_F16(psScalarPMSM->sUDQReq.f16Q,
    	                                                                     MLIB_Neg_F16(psScalarPMSM->f16UqMin)));
    }
                                         
    /* stator voltage angle , used the same integrator as for the open-loop start up*/
    psScalarPMSM->f16PosElScalar = GFLIB_Integrator_F16(psScalarPMSM->f16FreqRamp, &psScalarPMSM->sFreqIntegrator);

    /* pass parameters to FOC structure */
    psFocPMSM->sUDQReq     = psScalarPMSM->sUDQReq;
    psFocPMSM->f16PosElExt = psScalarPMSM->f16PosElScalar;

}
