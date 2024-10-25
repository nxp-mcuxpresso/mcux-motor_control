/*******************************************************************************
*
* Copyright 2013-2016, Freescale Semiconductor, Inc.
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

#include "mcdrv_sctimer_lpc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static bool_t s_statusPass;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function updates FTM value register
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_scTmrPwm3PhSet(mcdrv_sctimer_pwm_t *this)
{

    uint32_t periodMatchReg0 = 0U;
    uint32_t pulseMatchReg0 = 0U, pulseMatchReg1 = 0U, pulseMatchReg2 = 0U, pulseMatchReg3 = 0U, pulseMatchReg4 = 0U,
             pulseMatchReg5 = 0U;
    uint32_t pulsePeriod0 = 0U, pulsePeriod1 = 0U, pulsePeriod2 = 0U, pulsePeriod3 = 0U, pulsePeriod4 = 0U, pulsePeriod5 = 0U;
    uint32_t period;

    /* Retrieve the match register number for the PWM period */
    periodMatchReg0 = SCT0->EV[this->eventNumberOutput0].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    period          = SCT0->MATCH[periodMatchReg0];

    // A TOP   P18-5
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg0 = SCT0->EV[this->eventNumberOutput0 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod0 = ((period * this->psUABC->f16A) / 32768) + this->ui16DeadTimePWM;
    // A BOTTOM    P18-20
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg1 = SCT0->EV[this->eventNumberOutput1 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod1 = ((period * this->psUABC->f16A) / 32768) - this->ui16DeadTimePWM;
    // B TOP     P18-6
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg2 = SCT0->EV[this->eventNumberOutput2 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod2 = ((period * this->psUABC->f16B) / 32768) + this->ui16DeadTimePWM;
    // B BOTTOM      P18-3
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg3 = SCT0->EV[this->eventNumberOutput3 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod3 = ((period * this->psUABC->f16B) / 32768) - this->ui16DeadTimePWM;
    // C TOP      P18-19
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg4 = SCT0->EV[this->eventNumberOutput4 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod4 = ((period * this->psUABC->f16C) / 32768) + this->ui16DeadTimePWM;
    // C BOTTOM       P17-13
    /* Retrieve the match register number for the PWM pulse period */
    pulseMatchReg5 = SCT0->EV[this->eventNumberOutput5 + 1].CTRL & SCT_EV_CTRL_MATCHSEL_MASK;
    /* Calculate pulse width match value */
    pulsePeriod5 = ((period * this->psUABC->f16C) / 32768) - this->ui16DeadTimePWM;

    /* Stop the counter before updating match register */
    SCT0->CTRL_ACCESS16BIT.CTRLL |= (SCT_CTRLL_HALT_L_MASK);

    /* Update dutycycle */
    SCT0->MATCHREL[pulseMatchReg0] = SCT_MATCHREL_RELOADn_L(pulsePeriod0);
    SCT0->MATCHREL[pulseMatchReg1] = SCT_MATCHREL_RELOADn_L(pulsePeriod1);
    SCT0->MATCHREL[pulseMatchReg2] = SCT_MATCHREL_RELOADn_L(pulsePeriod2);
    SCT0->MATCHREL[pulseMatchReg3] = SCT_MATCHREL_RELOADn_L(pulsePeriod3);
    SCT0->MATCHREL[pulseMatchReg4] = SCT_MATCHREL_RELOADn_L(pulsePeriod4);
    SCT0->MATCHREL[pulseMatchReg5] = SCT_MATCHREL_RELOADn_L(pulsePeriod5);

    /* Restart the counter */
    SCT0->CTRL_ACCESS16BIT.CTRLL &= ~((uint16_t)SCT_CTRLL_HALT_L_MASK);

}

/*!
 * @brief Function enables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_scTmrPwm3PhOutEn(mcdrv_sctimer_pwm_t *this)
{

    /* Stop the counter before updating match register */
    SCT0->CTRL_ACCESS16BIT.CTRLL |= (SCT_CTRLL_HALT_L_MASK);

    SCT0->OUT[0].SET |= this->ui32OutSet0;
    SCT0->OUT[1].SET |= this->ui32OutSet1;
    SCT0->OUT[2].SET |= this->ui32OutSet2;
    SCT0->OUT[3].SET |= this->ui32OutSet3;
    SCT0->OUT[4].SET |= this->ui32OutSet4;
    SCT0->OUT[5].SET |= this->ui32OutSet5;

    SCT0->OUT[0].CLR |= this->ui32OutClr0;
    SCT0->OUT[1].CLR |= this->ui32OutClr1;
    SCT0->OUT[2].CLR |= this->ui32OutClr2;
    SCT0->OUT[3].CLR |= this->ui32OutClr3;
    SCT0->OUT[4].CLR |= this->ui32OutClr4;
    SCT0->OUT[5].CLR |= this->ui32OutClr5;

    /* Restart the counter */
    SCT0->CTRL_ACCESS16BIT.CTRLL &= ~((uint16_t)SCT_CTRLL_HALT_L_MASK);

}

/*!
 * @brief Function disables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_scTmrPwm3PhOutDis(mcdrv_sctimer_pwm_t *this)
{

    /* Stop the counter before updating match register */
    SCT0->CTRL_ACCESS16BIT.CTRLL |= (SCT_CTRLL_HALT_L_MASK);

    SCT0->OUT[0].SET = 0U;
    SCT0->OUT[1].SET = 0U;
    SCT0->OUT[2].SET = 0U;
    SCT0->OUT[3].SET = 0U;
    SCT0->OUT[4].SET = 0U;
    SCT0->OUT[5].SET = 0U;

    SCT0->OUT[0].CLR = 0U;
    SCT0->OUT[1].CLR = 0U;
    SCT0->OUT[2].CLR = 0U;
    SCT0->OUT[3].CLR = 0U;
    SCT0->OUT[4].CLR = 0U;
    SCT0->OUT[5].CLR = 0U;

    SCT0->OUTPUT = 0U; // Clear all SCTimer output

    /* Restart the counter */
    SCT0->CTRL_ACCESS16BIT.CTRLL &= ~((uint16_t)SCT_CTRLL_HALT_L_MASK);

}

/*!
 * @brief Function initialite PWM outputs structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_scTmrPwm3PhOutInit(mcdrv_sctimer_pwm_t *this)
{

    this->ui32OutSet0 = SCT0->OUT[0].SET;
    this->ui32OutSet1 = SCT0->OUT[1].SET;
    this->ui32OutSet2 = SCT0->OUT[2].SET;
    this->ui32OutSet3 = SCT0->OUT[3].SET;
    this->ui32OutSet4 = SCT0->OUT[4].SET;
    this->ui32OutSet5 = SCT0->OUT[5].SET;

    this->ui32OutClr0 = SCT0->OUT[0].CLR;
    this->ui32OutClr1 = SCT0->OUT[1].CLR;
    this->ui32OutClr2 = SCT0->OUT[2].CLR;
    this->ui32OutClr3 = SCT0->OUT[3].CLR;
    this->ui32OutClr4 = SCT0->OUT[4].CLR;
    this->ui32OutClr5 = SCT0->OUT[5].CLR;

}

/*!
 * @brief Function return actual value of over current flag
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_scTmrPwm3PhFltGet(mcdrv_sctimer_pwm_t *this)
{
    /* read over-current flags */
    s_statusPass = (((this->pui32PwmBaseAddress->INPUT & SCT_INPUT_AIN0_MASK) >> 0) &
                    (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));

    return ((s_statusPass > 0));
}
