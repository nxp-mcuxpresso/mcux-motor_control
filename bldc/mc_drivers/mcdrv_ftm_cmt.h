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
#ifndef _MCDRV_FTM_CMT_H_
#define _MCDRV_FTM_CMT_H_

#include "mlib.h"
#include "gflib.h"
#include "gdflib.h"
#include "gmclib.h"

#include "gmclib_types.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* init sensors/actuators pointers */
#define M1_SET_PTR_CNT_ACT(par1) (g_sM1CmtTmr.pui16FtmCntAct = &(par1))
#define M1_SET_PTR_VALUE_ACT(par1) (g_sM1CmtTmr.pui16FtmValueAct = &(par1))

typedef struct _mcdrv_ftm_cmt
{
    FTM_Type *pui32FtmBase;     /* pointer FlexTimer base address */
    uint16_t *pui16FtmCntAct;   /* pointer to actual value of FTM counter */
    uint16_t *pui16FtmValueAct; /* pointer to actual value of FTM value register */
    uint16_t ui16ChannelNum;    /* number of FTM channel used for compare event */
} mcdrv_ftm_cmt_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Function read actual values of FTM counter and value register
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmCmtGet(mcdrv_ftm_cmt_t *this);

/*!
 * @brief Function update FTM value register
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_FtmCmtSet(mcdrv_ftm_cmt_t *this, uint16_t ui16TimeNew);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_FTM_CMT_H_ */
