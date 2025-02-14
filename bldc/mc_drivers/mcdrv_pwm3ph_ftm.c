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
#include "mcdrv_pwm3ph_ftm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
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
 * @param i16InpDuty New input duty cycle to the pwma module
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmSetDutyCycle(mcdrv_pwm3ph_ftm_t *this, int16_t i16InpDuty)
{
    int16_t i16FirstEdge, i16SecondEdge, i16Duty;

    s_statusPass = TRUE;

    i16Duty = MLIB_Mul_F16((i16InpDuty), (this->ui16PwmModulo) / 4);

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

    this->pui32PwmBase->CONTROLS[this->ui16ChanPhA].CnV     = (uint32_t)i16FirstEdge;
    this->pui32PwmBase->CONTROLS[this->ui16ChanPhA + 1U].CnV = (uint32_t)i16SecondEdge;
    this->pui32PwmBase->CONTROLS[this->ui16ChanPhB].CnV     = (uint32_t)i16FirstEdge;
    this->pui32PwmBase->CONTROLS[this->ui16ChanPhB + 1U].CnV = (uint32_t)i16SecondEdge;
    this->pui32PwmBase->CONTROLS[this->ui16ChanPhC].CnV     = (uint32_t)i16FirstEdge;
    this->pui32PwmBase->CONTROLS[this->ui16ChanPhC + 1U].CnV = (uint32_t)i16SecondEdge;

    this->pui32PwmBase->PWMLOAD |= (FTM_PWMLOAD_LDOK_MASK);

    return (s_statusPass);
}

/*!
 * @brief Function set pwm sector from input
 *
 * @param this Pointer to the current object
 * @param sector Actual commutation sector
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmSetPwmOutput(mcdrv_pwm3ph_ftm_t *this, int16_t i16Sector)
{
    s_statusPass = TRUE;

    this->pui32PwmBase->INVCTRL = *((this->pcBldcTable) + (2 * i16Sector + 1));
    this->pui32PwmBase->OUTMASK = *((this->pcBldcTable) + (2 * i16Sector));

    this->pui32PwmBase->SYNC |= (FTM_SYNC_SWSYNC_MASK);
    return (s_statusPass);
}

/*!
 * @brief Function return actual value of over current flag
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmPwm3PhFltGet(mcdrv_pwm3ph_ftm_t *this)
{
    /* Read fixed-value over-current flag */
    s_statusPass = this->pui32PwmBase->FMS & (1UL << this->ui16FaultFixNum);

    /* Clear fault flags */
    this->pui32PwmBase->FMS &= ~(1UL << FTM_FMS_FAULTF0_SHIFT);

    return (s_statusPass);
}
