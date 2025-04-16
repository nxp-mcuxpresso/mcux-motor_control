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
  base->fltSpdMeEst = 0.0F;          /* estimated speed calculated using tracking observer */
  base->f16PosMe = FRAC16(0.0);            /* mechanical position calculated using encoder edges */
  base->f16PosMeEst = FRAC16(0.0);         /* estimated position calculated using tracking observer */
  
  base->mt_offset = 0U;
  base->st_offset = 0U;
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
void MCDRV_BissCSetOffset(BISSC_Type *base)
{
  int32_t i32ST;
  
  base->mt_offset = base->mt;
  base->st_offset = base->st;

  /* Set position to middle */
  i32ST = (int32_t)base->st - (int32_t)((1 << base->ui8DevSTLen) / 2 );
  
  /* Position offset */
  base->f16PosOffset = (frac16_t)(i32ST  * base->ui16Pp); 
  
}
  
/*!
 * @brief Function reads raw data and converts to single turn and multi turn revolutions
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCDataRead(BISSC_Type *base)
{
  uint64_t ui64PositionRaw;
  
  /* Read raw data from slave device ID 0 */
  ui64PositionRaw = BISS_SLVGetSCDRawData(base->pMaster, 0U);
  ui64PositionRaw = (ui64PositionRaw & (((uint64_t) 1 << (base->ui8DevDataLen)) - 1)) >> 2;
  
  /* Extract single turn and multiturn values */
  base->st = (uint32_t)((ui64PositionRaw) & (((uint64_t) 1 << base->ui8DevSTLen) - 1));
  base->mt = (uint32_t)((ui64PositionRaw >> base->ui8DevSTLen) & (((uint64_t) 1 << base->ui8DevMTLen) - 1));
}
  
/*!
 * @brief Function processes the data (fast-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCGetPositionFoc(BISSC_Type *base)
{  
  /* Mechanical position from single turn and set position to middle */
  base->f16PosMe = (frac16_t)((int32_t)base->st - (int32_t)((1 << base->ui8DevSTLen) / 2 ));
    
  /* Electrical position in frac16 */
  *base->pf16PosElEst = (frac16_t)(base->f16PosMe * base->ui16Pp ) - base->f16PosOffset; 
}

/*!
 * @brief Function processes the data (slow-loop)
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissCGetPositionFullAndSpeed(BISSC_Type *base)
{
  int32_t i32ST;
  float_t fltSpdMech;

  /* Set position to middle */
  i32ST = (int32_t)base->st - (int32_t)((1 << base->ui8DevSTLen) / 2 );
  
  /* Position difference (delta) */
  base->i32Diff = i32ST - base->i32ST_k_1;    /* TODO: check whether offset has impact */
         
  if(base->i32Diff< -((1 << base->ui8DevSTLen) / 2 ))
  {
//     base->i64RevCounter++;
    
     /* Diff calculation when counter overflow */
     base->i32Diff = i32ST - base->i32ST_k_1 + (1 << base->ui8DevSTLen);
  }    

  if(base->i32Diff > ((1 << base->ui8DevSTLen) / 2) )
  {
//     base->i64RevCounter--;
     
     /* Diff calculation when counter underflow */
     base->i32Diff = i32ST - base->i32ST_k_1 - (1 << base->ui8DevSTLen);
  }
  
  /* Speed [Hz ~ rps (revolutions per second)] = PositionDelta / (SampleTime * MaxPositionNumber) = (PositionDelta * SampleFrequency) / MaxPositionNumber [Hz] */
  /* Speed [rpm] = 60 * Speed[Hz] */
  fltSpdMech = ((float_t)base->i32Diff) * (4000.0F) * 60.0F / ((float_t)(1 << base->ui8DevSTLen));
  
  /* Mechanical angular speed [rad/s]  */
  base->fltSpdMeEst = fltSpdMech * ((2.0F * FLOAT_PI)/60.0F);
  
  /* Store latest value of single turn revolutions */
  base->i32ST_k_1 = i32ST;
   
   /* Mechanical angular speed [rad/s]  */
   *base->pfltSpdMeEst = base->fltSpdMeEst;
   
   /* Position in accumulator type for motor control purposes */
   base->a32PosMeReal = (acc32_t)(( (((int32_t)base->mt) - 2048) << 15    ) + (((uint16_t)(base->st)) >> 1) );
   *base->pa32PosMeReal = base->a32PosMeReal;
}
