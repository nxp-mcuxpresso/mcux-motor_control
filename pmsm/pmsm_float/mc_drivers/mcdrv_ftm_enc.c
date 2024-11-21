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

#include "mcdrv_ftm_enc.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function returns actual position and speed
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_FtmEncGet(mcdrv_ftm_enc_t *this)
{

    /* read number of pulses and get mechanical position */
    this->f16PosMe = (frac16_t)(MLIB_Mul_F16as(
        this->a32PosMeGain, (frac16_t)((this->pui32FtmBase->CNT & FTM_CNT_COUNT_MASK) >> FTM_CNT_COUNT_SHIFT)));

    /* tracking observer calculation */
    this->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(this->a32PosErr, &this->sTo);

    /* calculation of error function for tracking observer */
    this->a32PosErr = (acc32_t)MLIB_Sub_F16(this->f16PosMe, this->f16PosMeEst);

    if (this->a32PosErr > ACC32(0.5))
        this->a32PosErr -= ACC32(1.0);
    else if (this->a32PosErr < ACC32(-0.5))
        this->a32PosErr += ACC32(1.0);

    /* speed estimation by the tracking observer */
    this->fltSpdMeEst = this->sTo.fltSpeed;

    /* store results to user-defined variables */
    *this->pf16PosElEst = (frac16_t)(this->f16PosMeEst * this->ui16Pp);
    *this->pfltSpdMeEst = (this->fltSpdMeEst);

}

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_FtmEncClear(mcdrv_ftm_enc_t *this)
{

    this->f16PosMe    = 0;
    this->f16PosMeEst = 0;
    this->fltSpdMeEst = 0;

    /* initilize tracking observer */
    this->sTo.f32Theta = 0;
    this->sTo.fltSpeed = 0;
    this->sTo.fltI_1   = 0;

    /* clear FTM counter */
    this->pui32FtmBase->CNT = 0x0000;

}

/*!
 * @brief Function set mechanical position of quadrature encoder
 *
 * @param this     Pointer to the current object
 *        f16PosMe Mechanical position
 *
 * @return none
 */
void MCDRV_FtmEncSetPosMe(mcdrv_ftm_enc_t *this, frac16_t f16PosMe)
{
    frac16_t f16CntMod;

    f16CntMod = (frac16_t)((((this->pui32FtmBase->MOD) & FTM_MOD_MOD_MASK) >> FTM_MOD_MOD_SHIFT) >> 1);

    /* set mechnical position */
    this->f16PosMe     = f16PosMe;
    this->sTo.f32Theta = MLIB_Conv_F32s(f16PosMe);

    this->pui32FtmBase->CNT = ((this->pui32FtmBase->CNT) & ~(FTM_CNT_COUNT_MASK)) |
                              (FTM_CNT_COUNT((uint16_t)MLIB_Mul_F16(f16PosMe, f16CntMod) + (uint16_t)f16CntMod));

}
