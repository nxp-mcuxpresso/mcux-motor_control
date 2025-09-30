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

#include "mcdrv_endat3.h"

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
void MCDRV_Endat3Clear(mcdrv_endat3_t *base)
{
  base->fltSpdMeEst = 0.0F;                /* estimated speed calculated using encoder edges */
  base->f16PosMe = FRAC16(0.0);            /* mechanical position calculated using encoder edges */
  base->f16PosOffset = 0U;
}

/*!
 * @brief Function sets revolutions offset
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Endat3SetOffset(mcdrv_endat3_t *base)
{
  /* Set position to middle */
  base->i64EndatPosition = (int64_t)(base->ui64EndatPosition) - 16777216;
  /* Position offset */
  base->f16PosOffset = (frac16_t)((base->i64EndatPosition >> 9U)  * base->ui16Pp ); 
}

/*!
 * @brief Function reads raw data and converts to single turn and multi turn revolutions
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_Endat3DataRead(mcdrv_endat3_t *base)
{
  ENDAT3_getRsp(base->pui32EnDat3BaseAddress, 0, &base->rsp);      
}

/*!
 * @brief Function processes the data
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_EnDat3GetPositionFoc(mcdrv_endat3_t * base)
{
  uint64_t position;
  
  /* Read HPF data */
  position = ENDAT3_READ_HPF_DATA(base->rsp.hpf.hpf64);
  
  /* Shift HPF data to get 25bits value (encoder resolution). */
  base->ui64EndatPosition = (position & 0xFFFFFFFF) >> 7;
  
  /* Set position to middle */
  base->i64EndatPosition = (int64_t)(base->ui64EndatPosition) - 16777216; // ui64EndatPosition - (2^25)/2
  
  /* Mechanical position */
  base->f16PosMe = (frac16_t)(base->i64EndatPosition >> 9U);
  
  /* Electrical position in frac16 */
  *base->pf16PosElEst = (frac16_t)((base->i64EndatPosition >> 9U)  * base->ui16Pp ) - base->f16PosOffset; 
}

/*!
 * @brief Function processes the data (slow-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_EnDat3GetPositionFullAndSpeed(mcdrv_endat3_t * base)
{
    float_t fltSpdMech;
    
    /* Position difference (delta) */
    base->i64EndatDiff = base->i64EndatPosition - base->i64EndatPositionOld;
    
    /* Find out direction */
    if((base->i64EndatDiff) >= 0)
      base->bEndatDir = TRUE;
    else
      base->bEndatDir = FALSE;
    
    /* Find out revolution counter (multi-turn) */
    if(base->bEndatDir == FALSE)
    {      
      if(base->i64EndatDiff < -16777216 ) // Half of range  (2^(25))/2
      {
         base->i64RevCounter++;
         
         /* Diff calculation when counter overflow */
         base->i64EndatDiff = base->i64EndatPosition - base->i64EndatPositionOld + 33554432;
      }    
    }
    else /* bEndatDir == FALSE */
    {
      if(base->i64EndatDiff > 16777216 )
      {
         base->i64RevCounter--;
         
         /* Diff calculation when counter underflow */
         base->i64EndatDiff = base->i64EndatPosition - base->i64EndatPositionOld - 33554432;
      }
    }
    
    /* Multiturn position */
    
    /* Speed [Hz ~ rps (revolutions per second)] = PositionDelta / (SampleTime * MaxPositionNumber) = (PositionDelta * SampleFrequency) / MaxPositionNumber [Hz] */
    /* Speed [rpm] = 60 * Speed[Hz] */
    fltSpdMech = ((float_t)base->i64EndatDiff) * (4000.0F) * 60.0F / (33554432.0F);     /* Mechanical angular speed [rpm] */
    
    base->fltSpdMeEst = fltSpdMech * ((2.0F * FLOAT_PI)/60.0F);         /* Mechanical angular speed [rad/s]  */
         
    /* Store actual position */
    base->i64EndatPositionOld = base->i64EndatPosition;
    
    /* Store results to user-defined variables */
    /* Multiturn position in ACC32 */
    *base->pa32PosMeReal = (acc32_t)(( (((int32_t)base->i64RevCounter)) << 15 ) + (uint16_t)(base->ui64EndatPosition >> 10U) );
    
    /* Mechanical angular speed [rad/s]  */
    *base->pfltSpdMeEst = base->fltSpdMeEst;
}