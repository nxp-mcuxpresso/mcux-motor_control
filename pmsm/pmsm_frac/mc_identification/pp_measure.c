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

#include "pp_measure.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Pp measurement routine
 *
 * @param *sPpMeasFcn   input structure of type #mid_get_pp_a1_t for passing
 *                      all necessary parameters.
 *
 * @return None
 */
void MID_getPp(mid_get_pp_a1_t *sPpMeasFcn)
{
    /* Initialisation */
    if (sPpMeasFcn->bActive == FALSE)
    {
        sPpMeasFcn->sSpeedElRampParam.f16RampUp   = MID_SPEED_RAMP_UP;
        sPpMeasFcn->sSpeedElRampParam.f16RampDown = MID_SPEED_RAMP_DOWN;
        sPpMeasFcn->sSpeedIntegrator.a32Gain      = M1_SCALAR_INTEG_GAIN;
        sPpMeasFcn->sSpeedIntegrator.f32IAccK_1   = FRAC32(0.0);
        GFLIB_IntegratorInit_F16(0, &sPpMeasFcn->sSpeedIntegrator);
        sPpMeasFcn->bActive       = TRUE;
        sPpMeasFcn->f16SpeedElRamp   = FRAC16(0.0);
        sPpMeasFcn->ui16PpDetermined = 0;
    }

    /* Set Id required */
    *(sPpMeasFcn->pf16IdReq) = sPpMeasFcn->f16IdReqOpenLoop;

    /* Else start incrementing position */
    if (sPpMeasFcn->ui16WaitingSteady == 0U)
    {
        /* Ramp electrical speed */
        sPpMeasFcn->f16SpeedElRamp = GFLIB_Ramp_F16(sPpMeasFcn->f16SpeedElReq, &sPpMeasFcn->sSpeedElRampParam);
        /* Integrate electrical speed to get electrical position */
        *sPpMeasFcn->pf16PosEl = GFLIB_Integrator_F16(sPpMeasFcn->f16SpeedElRamp, &sPpMeasFcn->sSpeedIntegrator);
    }

    /* If position overflows, wait 2400ms in zero position */
    if (((*sPpMeasFcn->pf16PosEl < FRAC16(0.0)) && (sPpMeasFcn->f16PosElLast > FRAC16(0.0))) ||
        (sPpMeasFcn->ui16WaitingSteady == 1U))
    {
        *sPpMeasFcn->pf16PosEl = FRAC16(-1.0);

        /* Initialise waiting */
        if (sPpMeasFcn->ui16WaitingSteady == 0U)
        {
            sPpMeasFcn->ui16LoopCounter   = 0;
            sPpMeasFcn->ui16WaitingSteady = 1;
        }

        sPpMeasFcn->ui16LoopCounter++;

        /* Escape waiting in steady position after 2400 ms */
        if (sPpMeasFcn->ui16LoopCounter > (uint16_t)MID_TIME_2400MS)
        {
            *sPpMeasFcn->pf16PosEl        = FRAC16(0.0);
            sPpMeasFcn->f16PosElLast      = FRAC16(0.0);
            sPpMeasFcn->ui16WaitingSteady = 0;
        }
    }

    /* Save last position */
    sPpMeasFcn->f16PosElLast    = sPpMeasFcn->f16PosElCurrent;
    sPpMeasFcn->f16PosElCurrent = *sPpMeasFcn->pf16PosEl;

    if (sPpMeasFcn->ui16PpDetermined > 0U)
    {
        /* When finished exit the function */
        sPpMeasFcn->bActive = FALSE;
    }
}
