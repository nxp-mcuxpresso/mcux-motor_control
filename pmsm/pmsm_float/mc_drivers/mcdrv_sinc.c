/*
* Copyright 2025-2026 NXP
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

#include "mcdrv_sinc.h"

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
 * @brief Function reads SINC data and converts to measured phase currentsand DC-bus voltage
 *
 * @param base   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_SincReadPhCurrDcBusVolt(mcdrv_sinc_t *this)
{
  float_t fltTmp;
  uint32_t ui32ChannelId;
  int32_t i32Sinc1ResultsArray;
  float_t fltSinc1Results[4];

  /* Read channels 0-3 */
  for( ui32ChannelId = 0U; ui32ChannelId  < 4; ++ui32ChannelId )
  {
      const uint32_t u32CurrentCocMask = (uint32_t)(1U << (ui32ChannelId + SINC_NIS_CHF0_SHIFT));

      this->pui32SincBaseAddress->NIS = u32CurrentCocMask;	// Clear the CHF flag

      /* Read FIFO to discard useless data in FIFO, do not use this sample */
      i32Sinc1ResultsArray = (int32_t)this->pui32SincBaseAddress->CHANNEL[ui32ChannelId].CRDATA;
      
      /* ENOB is 13 bits, so right bitshift of 19 */
      i32Sinc1ResultsArray = (int32_t)(this->pui32SincBaseAddress->CHANNEL[ui32ChannelId].CRDATA) >> 19U;
      
      /* Save measured values used for offset calibration */
      this->f16PhCurr[ui32ChannelId] = (frac16_t)i32Sinc1ResultsArray;
    
      fltTmp = ((float_t)i32Sinc1ResultsArray) / ((float_t)0x800);        // get result within range <-1;1>
      fltSinc1Results[ui32ChannelId] = fltTmp;
  }
  
  /* Use the newest SINC measured phase currents */
  this->psIABC->fltA = (-1.0F) * fltSinc1Results[0] * this->fltCurrentScale;
  this->psIABC->fltB = (-1.0F) * fltSinc1Results[1] * this->fltCurrentScale;
  this->psIABC->fltC = (-1.0F) * fltSinc1Results[2] * this->fltCurrentScale;
  *this->pfltUDcBus = fltSinc1Results[3] * this->fltDCBvoltageScale;
}


/*!
 * @brief Initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_SincCurr3PhCalibInit(mcdrv_sinc_t *this)
{
  /* clear offset values */
  this->f16CalibPhaA = 0;
  this->f16CalibPhaB = 0;
  this->f16CalibPhaC = 0;
  
  /* initialize offset filters */    
  this->sFiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
  this->sFiltPhaB.u16Sh = this->ui16OffsetFiltWindow;
  this->sFiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
  
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sFiltPhaA);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sFiltPhaB);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sFiltPhaC);
    
  /* Master disable before clear BIAS values */
  this->pui32SincBaseAddress->MCR &= ~SINC_MCR_MEN_MASK;
  
  /* Clear offset of each channel used for phase current */
  this->pui32SincBaseAddress->CHANNEL[0].CBIAS = (uint32_t)(0U);
  this->pui32SincBaseAddress->CHANNEL[1].CBIAS = (uint32_t)(0U);
  this->pui32SincBaseAddress->CHANNEL[2].CBIAS = (uint32_t)(0U);
  
  /* Master enable when BIAS is set */
  this->pui32SincBaseAddress->MCR |= SINC_MCR_MEN(1U);
}


/*!
 * @brief Function reads current samples and filter them
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_SincCurr3PhCalib(mcdrv_sinc_t *this)
{
  /* sensing offset IA */
  this->f16CalibPhaA = GDFLIB_FilterMA_F16((frac16_t)(this->f16PhCurr[0]), &this->sFiltPhaA);
  
  /* sensing offset IB */
  this->f16CalibPhaB = GDFLIB_FilterMA_F16((frac16_t)(this->f16PhCurr[1]), &this->sFiltPhaB);
  
  /* sensing offset IC */
  this->f16CalibPhaC = GDFLIB_FilterMA_F16((frac16_t)(this->f16PhCurr[2]), &this->sFiltPhaC);
}

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_SincCurr3PhCalibSet(mcdrv_sinc_t *this)
{ 
  /* Master disable before BIAS set */
  this->pui32SincBaseAddress->MCR &= ~SINC_MCR_MEN_MASK;
  
  /* pass calibration data - left bitshift because of the right bitshift of 19 bits in the read function */
  this->pui32SincBaseAddress->CHANNEL[0].CBIAS = (uint32_t)(this->f16CalibPhaA << 19U);
  this->pui32SincBaseAddress->CHANNEL[1].CBIAS = (uint32_t)(this->f16CalibPhaB << 19U);
  this->pui32SincBaseAddress->CHANNEL[2].CBIAS = (uint32_t)(this->f16CalibPhaC << 19U);
  
  /* Master enable when BIAS is set */
  this->pui32SincBaseAddress->MCR |= SINC_MCR_MEN(1U);
}