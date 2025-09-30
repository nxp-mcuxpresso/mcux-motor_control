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

#ifndef _MCDRV_ENDAT3_H_
#define _MCDRV_ENDAT3_H_

#include "fsl_common.h"
#include "mlib_types.h"
#include "mlib.h"
#include "amclib_FP.h"
#include "fsl_endat3.h"


/*!
 * @addtogroup bissc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _mcdrv_endat3
{
  ENDAT3_Type *pui32EnDat3BaseAddress;      /* Pointer to EnDat3 base */
  endat3_rsp_t rsp;
  
  float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
  frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
  acc32_t *pa32PosMeReal;       /* pointer to real position (revolution counter + mechanical position) */ 
  
  float_t fltSpdMeEst;          /* estimated speed calculated using encoder edges */
  frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
  frac16_t f16PosOffset;              
  uint16_t ui16Pp;              /* number of motor pole pairs */
  uint64_t ui64EndatPosition;   /* position at step k (unsigned) */
  int64_t i64EndatPosition;     /* position at step k (signed) */
  int64_t i64EndatPositionOld;  /* position at step k-1 (signed) */
  int64_t i64RevCounter;        /* multiturn revolution counter */
  int64_t i64EndatDiff;         /* position difference between steps k and k-1 */
  bool_t bEndatDir;             /* direction of the encoder rotation */

} mcdrv_endat3_t;

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
void MCDRV_Endat3Clear(mcdrv_endat3_t *base);

/*!
 * @brief Function sets revolutions offset
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Endat3SetOffset(mcdrv_endat3_t *base);
   
/*!
 * @brief Function reads raw data and converts to single turn and multi turn revolutions
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Endat3DataRead(mcdrv_endat3_t *base);

/*!
 * @brief Function processes the data (fast-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_EnDat3GetPositionFoc(mcdrv_endat3_t * base);

/*!
 * @brief Function processes the data (slow-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_EnDat3GetPositionFullAndSpeed(mcdrv_endat3_t * base);

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /* MCDRV_Endat3_H_*/
