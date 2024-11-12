/*******************************************************************************
*
* Copyright 2013-2016 Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
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
*
****************************************************************************/

#include "mcdrv_enc_qtmr.h"

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
void MCDRV_QtmrEncGet(mcdrv_qtmr_enc_t *this)
{	
	/* read number of pulses and get mechanical position */
	this->f16PosMe = (frac16_t)(MLIB_Mul_F16as(this->a32PosMeGain, (frac16_t)((this->pui32QdBase->CHANNEL[0].CNTR))));
	
	/* tracking observer calculation */
	this->f16PosMeEst = AMCLIB_TrackObsrv_F16(this->f16PosErr, &this->sTo);
	
	/* calculation of error function for tracking observer */
	this->f16PosErr = MLIB_Sub_F16(this->f16PosMe, this->f16PosMeEst);
	
	/* speed estimation by the tracking observer */
	this->f16SpdMeEst = MLIB_Conv_F16l(this->sTo.f32Speed);
			
	/* if TCF1 flag occurs - increment revolution */
	if((((this->pui32QdBase->CHANNEL[0].CSCTRL) & TMR_CSCTRL_TCF1_MASK) >> TMR_CSCTRL_TCF1_SHIFT) == 1U)
	{
		this->i32RevCounter++;	
		/* clear TCF1 flag */
		(this->pui32QdBase->CHANNEL[0].CSCTRL)  &= ~TMR_CSCTRL_TCF1(1);
	}
	/* if TCF2 flag occurs - decrement revolution */
	if((((this->pui32QdBase->CHANNEL[0].CSCTRL) & TMR_CSCTRL_TCF2_MASK) >> TMR_CSCTRL_TCF2_SHIFT) == 1U)
	{
		this->i32RevCounter--;	
		/* clear TCF2 flag */
		(this->pui32QdBase->CHANNEL[0].CSCTRL)  &= ~TMR_CSCTRL_TCF2(1);
	}
	
	/* LOAD register is equal to COMP1 (number of encoder pulses) if direction is 0 */
	if((((this->pui32QdBase->CHANNEL[0].CSCTRL) & TMR_CSCTRL_UP_MASK) >> TMR_CSCTRL_UP_SHIFT) == 0U)
		(this->pui32QdBase->CHANNEL[0].LOAD) = (this->pui32QdBase->CHANNEL[0].COMP1);
	else
		(this->pui32QdBase->CHANNEL[0].LOAD) = 0x0;	
			
	/* calculating position for position control */
	this->a32PosMeReal = (acc32_t)((((this->i32RevCounter) << 15) + (((uint16_t)(this->f16PosMe)) >> 1))); 
	
	/* store results to user-defined variables */
	*this->pf16PosElEst = (frac16_t)(this->f16PosMeEst * this->ui16Pp);
    *this->pf16SpdMeEst = (this->f16SpdMeEst);

}

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_QtmrEncClear(mcdrv_qtmr_enc_t *this)
{
	this->f16PosMe    = FRAC16(0.0);
	this->f16PosMeEst = FRAC16(0.0);
	this->f16SpdMeEst = FRAC16(0.0);
	
	/* initilize tracking observer */
	this->sTo.f32Theta = FRAC32(0.0);
	this->sTo.f32Speed = FRAC32(0.0);
	this->sTo.f32I_1   = FRAC32(0.0);
	
	/* clear QTMR counter */
	this->pui32QdBase->CHANNEL[0].CNTR = 0U;
	/* clear revolution counter */
	this->i32RevCounter = 0;

}

/*!
 * @brief Function set direction of quadrature encoder
 *
 * @param this       Pointer to the current object
 *        bDirection Encoder direction
 *
 * @return none
 */
void MCDRV_QtmrEncSetDirection(mcdrv_qtmr_enc_t *this)
{
	/* forward/reverse */
	if (this->bDirection)
		this->pui32QdBase->CHANNEL[0].CTRL |= TMR_CTRL_DIR_MASK;
	else
		this->pui32QdBase->CHANNEL[0].CTRL &= ~TMR_CTRL_DIR_MASK;

}

/*!
 * @brief Function set quadrature encoder pulses per one revolution
 *
 * @param this            Pointer to the current object
 *        ui16PulseNumber Encoder pulses per revolution
 *
 * @return none
 */
void MCDRV_QtmrEncSetPulses(mcdrv_qtmr_enc_t *this)
{
    /* Set modulo counter to encoder number of pulses * 4 - 1 */
	this->pui32QdBase->CHANNEL[0].COMP1 = TMR_COMP1_COMPARISON_1(((this->ui16PulseNumber * 4U) - 1U));
	this->pui32QdBase->CHANNEL[0].COMP2 = TMR_COMP2_COMPARISON_2(0U);

}
