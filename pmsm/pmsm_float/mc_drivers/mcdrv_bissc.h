/*
* Copyright 2025 NXP
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

#ifndef _MCDRV_BISSC_H_
#define _MCDRV_BISSC_H_

#include "fsl_common.h"
#include "mlib_types.h"
#include "mlib.h"
#include "amclib_FP.h"
#include "fsl_biss.h"


/*!
 * @addtogroup bissc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _bissc_type
{
    biss_master_t *pMaster;        /* BiSS master handle pointer */
    float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
    frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
    acc32_t *pa32PosMeReal;       /* pointer to real position */ 
  
    uint32_t mt;                /* Actual value of multi-turn */
    uint32_t st;                /* Actual value of single-turn */
    int32_t i32ST_k_1;             /* Previous value of single-turn */
    uint32_t mt_offset;
    uint32_t st_offset;
    frac16_t f16PosOffset;
    int32_t i32Diff;
     
    acc32_t a32PosMeReal;         /* real position (revolution counter + mechanical position) */
    float_t fltSpdMeEst;          /* estimated speed calculated using tracking observer */
    frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
    frac16_t f16PosMeEst;         /* estimated position calculated using tracking observer */
    
    uint16_t ui16Pp;              /* number of motor pole pairs */
    
    const uint8_t ui8DevDataLen;        /* BiSS device data length */
    const uint8_t ui8DevSTLen;          /* Single turn data length */
    const uint8_t ui8DevMTLen;          /* Multi turn data length */
} BISSC_Type;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @brief Function clears parameters of object
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCClear(BISSC_Type *base);

/*!
 * @brief Function sets revolutions offset
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCSetOffset(BISSC_Type *base);

/*!
 * @brief Function reads raw data and converts to single turn and multi turn revolutions
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCDataRead(BISSC_Type *base);

/*!
 * @brief Function processes the data (fast-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCGetPositionFoc(BISSC_Type *base);

/*!
 * @brief Function processes the data (slow-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCGetPositionFullAndSpeed(BISSC_Type *base);

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /* MCDRV_BISSC_H_*/
