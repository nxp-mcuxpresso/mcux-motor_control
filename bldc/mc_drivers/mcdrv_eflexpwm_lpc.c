/*
* Copyright 2016, Freescale Semiconductor, Inc.
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
 */

#include "mcdrv_eflexpwm_lpc.h"

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
 * @brief Function set duty cycle from input parameter
 *
 * @param this Pointer to the current object
 * @param f16InpDuty New input duty cycle to the pwma module
 *
 * @return none
 */
RAM_FUNC_LIB   
void MCDRV_eFlexSetDutyCycle(mcdrv_eflexpwm_t *this, frac16_t f16InpDuty)
{
    int16_t i16FirstEdge, i16SecondEdge, i16Duty;

    i16Duty = MLIB_Mul_F16((f16InpDuty), (this->ui16PwmModulo) / 4);

    i16FirstEdge = -(this->ui16PwmModulo) / 4 - i16Duty;
    if (i16FirstEdge < (-(this->ui16PwmModulo) / 2))
    {
        i16FirstEdge = -(this->ui16PwmModulo) / 2;
    }

    i16SecondEdge = (this->ui16PwmModulo) / 4 + i16Duty;
    if (i16SecondEdge > ((this->ui16PwmModulo) / 2))
    {
        i16SecondEdge = (this->ui16PwmModulo) / 2;
    }

    /* manual clear LDOK bits*/
    this->pui32PwmBaseAddress->MCTRL = (this->pui32PwmBaseAddress->MCTRL & ~PWM_MCTRL_CLDOK_MASK) | PWM_MCTRL_CLDOK(0x7);
    
    /* set updated values of PWM edges to value 2 & 3 registers of all channels*/
    this->pui32PwmBaseAddress->SM[0].VAL2 = (uint16_t)i16FirstEdge;
    this->pui32PwmBaseAddress->SM[0].VAL3 = (uint16_t)i16SecondEdge;

    this->pui32PwmBaseAddress->SM[1].VAL2 = (uint16_t)i16FirstEdge;
    this->pui32PwmBaseAddress->SM[1].VAL3 = (uint16_t)i16SecondEdge;

    this->pui32PwmBaseAddress->SM[2].VAL2 = (uint16_t)i16FirstEdge;
    this->pui32PwmBaseAddress->SM[2].VAL3 = (uint16_t)i16SecondEdge;

    /* set LDOK bits */
    this->pui32PwmBaseAddress->MCTRL |= (this->pui32PwmBaseAddress->MCTRL & ~PWM_MCTRL_LDOK_MASK) | PWM_MCTRL_LDOK(0x7);
    
}

/*!
 * @brief Function set pwm sector from input
 *
 * @param this Pointer to the current object
 * @param i16Sector Actual commutation sector
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_eFlexSetPwmOutput(mcdrv_eflexpwm_t *this, int16_t i16Sector)
{
    /* Mask PWM output - Turn off one phase for BEMF measurement*/
    this->pui32PwmBaseAddress->MASK     = *((this->pcBldcTable) + (2 * i16Sector));
    /* Set on one phase and invert signal generation of the other phase*/
    this->pui32PwmBaseAddress->DTSRCSEL = *((this->pcBldcTable) + (2 * i16Sector + 1));
    
    /* Update setup of PWM */
    this->pui32PwmBaseAddress->SM[0].CTRL2 |= PWM_CTRL2_FORCE(1);
    this->pui32PwmBaseAddress->SM[1].CTRL2 |= PWM_CTRL2_FORCE(1);
    this->pui32PwmBaseAddress->SM[2].CTRL2 |= PWM_CTRL2_FORCE(1);
}

/*!
 * @brief Function return actual value of over current flag
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
RAM_FUNC_LIB
bool_t MCDRV_eFlexPwm3PhFltGet(mcdrv_eflexpwm_t *this)
{
    /* read over-current flags */
    s_statusPass = (((this->pui32PwmBaseAddress->FSTS & PWM_FSTS_FFPIN_MASK) >> 8) &
                    (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));

    /* clear faults flag */
    this->pui32PwmBaseAddress->FSTS = ((this->pui32PwmBaseAddress->FSTS & ~(uint16_t)(PWM_FSTS_FFLAG_MASK)) |
                                       (1 << this->ui16FaultFixNum | 1 << this->ui16FaultAdjNum));

    return ((s_statusPass > 0));
}