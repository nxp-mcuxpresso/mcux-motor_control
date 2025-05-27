/*
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2025 NXP
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

#include "mcdrv_emios_mcxe.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
   
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

frac16_t f16DutyCycleA, f16DutyCycleB, f16DutyCycleC, f16DutyCycleTemp, f16ModuloTemp;

void MCDRV_eMIOS_PhSet(mcdrv_pwm3ph_emios_t *this)
{
 
    GMCLIB_3COOR_T_F16 sUABCtemp;
    
    /* pointer to duty cycle structure */
    sUABCtemp = *this->psUABC;

    /* get modulo value divided by 2 */    
    f16ModuloTemp = (this->pui32PwmBaseAddress->UC[0U].A)/2;
    
    /* phase A */
//    f16DutyCycleA = MLIB_AbsSat_F16(MLIB_Mul_F16(f16ModuloTemp, sUABCtemp.f16A));
    f16DutyCycleA = MLIB_Mul_F16(f16ModuloTemp, sUABCtemp.f16A);
    this->pui32PwmBaseAddress->UC[1U].A = EMIOS_A_A((f16ModuloTemp - f16DutyCycleA));
    this->pui32PwmBaseAddress->UC[1U].B = EMIOS_B_B((f16ModuloTemp + f16DutyCycleA));
    
    /* phase B */
    f16DutyCycleB = MLIB_Mul_F16(f16ModuloTemp, sUABCtemp.f16B);
    this->pui32PwmBaseAddress->UC[2U].A = EMIOS_A_A(f16ModuloTemp - f16DutyCycleB);
    this->pui32PwmBaseAddress->UC[2U].B = EMIOS_B_B(f16ModuloTemp + f16DutyCycleB);
    
    /* phase C */
    f16DutyCycleC = MLIB_Mul_F16(f16ModuloTemp, sUABCtemp.f16C);
    this->pui32PwmBaseAddress->UC[3U].A = EMIOS_A_A(f16ModuloTemp - f16DutyCycleC);
    this->pui32PwmBaseAddress->UC[3U].B = EMIOS_B_B(f16ModuloTemp + f16DutyCycleC);
    
}


bool_t MCDRV_eMIOS_PhFltGet(mcdrv_pwm3ph_emios_t *this)
{

    return (false);
}
