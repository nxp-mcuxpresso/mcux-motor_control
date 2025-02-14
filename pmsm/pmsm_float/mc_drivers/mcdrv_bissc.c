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

#include "mcdrv_bissc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Codes
 ******************************************************************************/
/*!
 * @brief Function clears parameters of object
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCClear(BISSC_Type *base)
{
  base->a32PosMeReal = ACC32(0.0);         /* real position (revolution counter + mechanical position) */
  base->a32PosErr = ACC32(0.0);            /* position error to tracking observer  */
  base->fltSpdMeEst = 0.0F;          /* estimated speed calculated using tracking observer */
  base->f16PosMe = FRAC16(0.0);            /* mechanical position calculated using encoder edges */
  base->f16PosMeEst = FRAC16(0.0);         /* estimated position calculated using tracking observer */

  /* initilize tracking observer */
  base->sTo.f32Theta = FRAC32(0.0);
  base->sTo.fltSpeed = 0.0F;
  base->sTo.fltI_1   = 0.0F;
  
  base->mt_offset = 0U;
  base->st_offset = 0U;
}

/*!
 * @brief Function sets revolutions offset
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCSetOffset(BISSC_Type *base)
{
//    base->f16PosMeOffset =  (frac16_t)(base->f16PosMe);
  base->mt_offset = base->mt;
  base->st_offset = base->st;

}

/*!
 * @brief Function processes the data
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCDataProc(BISSC_Type * base)
{
  uint64_t ui64PositionRaw;
  
  /* Read raw data from slave device ID 0 */
  ui64PositionRaw = BISS_SLVGetSCDRawData(base->pMaster, 0U);
  ui64PositionRaw = (ui64PositionRaw & (((uint64_t) 1 << (base->ui8DevDataLen)) - 1)) >> 2;
  
  /* Extract single turn and multiturn values */
  base->st = (uint32_t)((ui64PositionRaw) & (((uint64_t) 1 << base->ui8DevSTLen) - 1));
  base->mt = (uint32_t)((ui64PositionRaw >> base->ui8DevSTLen) & (((uint64_t) 1 << base->ui8DevMTLen) - 1));
  
  /* mechanical position from single turn */
  base->f16PosMe = (frac16_t)(base->st - base->st_offset);
  
  ////////////////////////////////////////////////////////////////////////////
  /////////////// Tracking observer //////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
     
  /* tracking observer calculation */
  base->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(base->a32PosErr, &base->sTo);

  /* calculation of error function for tracking observer */
  base->a32PosErr = (acc32_t)MLIB_Sub_F16(base->f16PosMe, base->f16PosMeEst);

  /* Store speed estimation by the tracking observer */
  *base->pfltSpdMeEst = base->sTo.fltSpeed;
  
  /* position in accumulator type for motor control purposes */
  //base->a32PosMeReal = (acc32_t)(( (((int32_t)base->mt) - 2048) << 15    ) + (((uint16_t)(base->st)) >> 1) ); 
  *base->pa32PosMeReal = (acc32_t)(( (((int32_t)base->mt) - 2048) << 15    ) + (((uint16_t)(base->st)) >> 1) );
  
  //base->a32PosMeReal = (acc32_t)(( (((int32_t)(base->mt - base->mt_offset)) - 2048) << 15    ) + (((uint16_t)(base->st - base->st_offset)) >> 1) );
  
  /* store results to user-defined variables */
  *base->pf16PosElEst = (frac16_t)(base->f16PosMeEst * base->ui16Pp);
}