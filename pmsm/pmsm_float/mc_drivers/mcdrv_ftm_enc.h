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

#ifndef _MCDRV_FTM_ENC_H_
#define _MCDRV_FTM_ENC_H_

#include "mlib.h"
#include "mlib_types.h"
#include "fsl_device_registers.h"
#include "amclib.h"
#include "pmsm_control.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _mcdrv_ftm_enc
{
    AMCLIB_TRACK_OBSRV_T_FLT sTo; /* tracking observer structure */
    FTM_Type *pui32FtmBase;       /* pointer to QD module base address*/
    float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
    frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
    acc32_t a32PosErr;            /* position error to tracking observer  */
    acc32_t a32PosMeGain;         /* encoder pulses to mechanical position scale gain */
    float_t fltSpdMeEst;          /* estimated speed calculated using tracking observer */
    frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
    frac16_t f16PosMeEst;         /* estimated position calculated using tracking observer */
    uint16_t ui16Pp;              /* number of motor pole pairs */
    float_t fltSpdEncMin;         /* encoder minimal speed resolution */
    frac16_t f16PosErr;           /* poisition error to tracking observer  */
    frac16_t f16PosMeGain;        /* encoder pulses to mechanical position scale gain */
    int16_t i16PosMeGainSh;       /* encoder pulses to mechanical position scale shift */
    acc32_t a32PosMeReal;         /* real position (revolution counter + mechanical position) */
    uint32_t ui32RevCounter;      /* revolution counter measured by periphery */
} mcdrv_ftm_enc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Function returns actual position and speed
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_FtmEncGet(mcdrv_ftm_enc_t *this);

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_FtmEncClear(mcdrv_ftm_enc_t *this);

/*!
 * @brief Function se mechanical position of quadrature encoder
 *
 * @param this     Pointer to the current object
 *        f16PosMe Mechanical position
 *
 * @return none
 */
void MCDRV_FtmEncSetPosMe(mcdrv_ftm_enc_t *this, frac16_t f16PosMe);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_FTM_ENC_H_ */
