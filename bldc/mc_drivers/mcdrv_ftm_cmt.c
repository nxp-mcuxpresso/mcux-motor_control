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

#include "mcdrv_ftm_cmt.h"

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
 * @brief Function read actual values of FTM counter and value register
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmCmtGet(mcdrv_ftm_cmt_t *this)
{
    s_statusPass = TRUE;

    /* read actual values of counter and defined value register */
    *this->pui16FtmCntAct   = (uint16_t)this->pui32FtmBase->CNT;
    *this->pui16FtmValueAct = (uint16_t)this->pui32FtmBase->CONTROLS[this->ui16ChannelNum].CnV;

    return (s_statusPass);
}

/*!
 * @brief Function update FTM value register
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmCmtSet(mcdrv_ftm_cmt_t *this, uint16_t ui16TimeNew)
{
    s_statusPass = TRUE;

    /* update defined value register */
    this->pui32FtmBase->CONTROLS[this->ui16ChannelNum].CnV = ui16TimeNew;

    return (s_statusPass);
}
