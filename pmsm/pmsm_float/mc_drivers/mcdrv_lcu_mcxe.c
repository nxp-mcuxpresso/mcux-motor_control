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

#include "mcdrv_lcu_mcxe.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function enables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_LCU_PhOutEn(void)
{
  
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output0), true);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output1), true);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output2), true);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output3), true);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc2Output2), true);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc2Output3), true);
  
}

/*!
 * @brief Function disables PWM outputs
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_LCU_PhOutDis(void)
{
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output0), false);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output1), false);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output2), false);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc1Output3), false);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc2Output2), false);
    LCU_EnableOutput(LCU_1, (1 << kLCU_Lc2Output3), false);

}