/*
 * Copyright 2013 - 2015, Freescale Semiconductor, Inc.
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

#include "mcdrv_adc_lpc55s36.h"
#include "fsl_gpio.h"

#include "mc_periph_init.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static bool GetAdcResFIFO(ADC_Type *pBase, lpadc_conv_result_t *pResult, uint8_t index);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Reads and calculates 3 phase samples based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_CurrAndVoltDcBusGet(mcdrv_adc_t *this)
{
  GMCLIB_3COOR_T_F16 sIABCtemp;
  
  /* Read available converted values from the FIFO[0]. */
  while( GetAdcResFIFO(this->pToAdcBase, &this->s_ADC_ResultStructure, 0U) )
  {
    switch( this->s_ADC_ResultStructure.commandIdSource )
    {
    case 1U:
      /* Command 1 - current A */
      this->ui16AdcCurrA = (this->s_ADC_ResultStructure.convValue << 1U);
      break;
      
    case 2U:
      /* Command 2 - current B */
      this->ui16AdcCurrB = (this->s_ADC_ResultStructure.convValue << 1U);
      break;
      
    case 3U:
      /* Command 3 - voltage DcBus */
      this->ui16AdcDCBVolt = (this->s_ADC_ResultStructure.convValue);
      *this->pf16UDcBus    = (frac16_t)(this->ui16AdcDCBVolt);
      break;
      
    default:
      break;
    }
  }
  
  /* Read available converted values from the FIFO[1]. */
  while( GetAdcResFIFO(this->pToAdcBase, &this->s_ADC_ResultStructure, 1U) )
  {
    switch( this->s_ADC_ResultStructure.commandIdSource )
    {
    case 1U:
      /* Command 1 - current C */
      this->ui16AdcCurrC = (this->s_ADC_ResultStructure.convValue << 1U);
      break;
      
    default:
      break;
    }
  }
  
  switch (*this->pui16SVMSector)
  {
  case 2:
  case 3:
    /* direct sensing of phase A and C, calculation of B */
    sIABCtemp.f16A = (((frac16_t)(this->ui16AdcCurrA)) - this->sCurrSec23.ui16OffsetPhaA);
    sIABCtemp.f16C = (((frac16_t)(this->ui16AdcCurrC)) - this->sCurrSec23.ui16OffsetPhaC);
    sIABCtemp.f16B = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16C));
    
    break;
    
  case 4:
  case 5:
    /* direct sensing of phase A and B, calculation of C */
    sIABCtemp.f16A = (((frac16_t)(this->ui16AdcCurrA)) - this->sCurrSec45.ui16OffsetPhaA);
    sIABCtemp.f16B = (((frac16_t)(this->ui16AdcCurrB)) - this->sCurrSec45.ui16OffsetPhaB);
    sIABCtemp.f16C = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16B));
    
    break;
    
  case 1:
  case 6:
  default:
    /* direct sensing of phase B and C, calculation of A */
    sIABCtemp.f16B = (((frac16_t)(this->ui16AdcCurrB)) - this->sCurrSec16.ui16OffsetPhaB);
    sIABCtemp.f16C = (((frac16_t)(this->ui16AdcCurrC)) - this->sCurrSec16.ui16OffsetPhaC);
    sIABCtemp.f16A = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16B, sIABCtemp.f16C));
    
    break;
  }
  
  /* pass measured phase currents to the main module structure */
  this->psIABC->f16A = sIABCtemp.f16A;
  this->psIABC->f16B = sIABCtemp.f16B;
  this->psIABC->f16C = sIABCtemp.f16C;
}

/*!
 * @brief Initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalibInit(mcdrv_adc_t *this)
{
  /* clear offset values */
  this->sCurrSec16.ui16OffsetPhaB = 0U;
  this->sCurrSec16.ui16OffsetPhaC = 0U;
  this->sCurrSec23.ui16OffsetPhaA = 0U;
  this->sCurrSec23.ui16OffsetPhaC = 0U;
  this->sCurrSec45.ui16OffsetPhaA = 0U;
  this->sCurrSec45.ui16OffsetPhaB = 0U;
  
  this->sCurrSec16.ui16CalibPhaB = 0U;
  this->sCurrSec16.ui16CalibPhaC = 0U;
  this->sCurrSec23.ui16CalibPhaA = 0U;
  this->sCurrSec23.ui16CalibPhaC = 0U;
  this->sCurrSec45.ui16CalibPhaA = 0U;
  this->sCurrSec45.ui16CalibPhaB = 0U;
  
  /* initialize offset filters */
  this->sCurrSec16.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;
  this->sCurrSec16.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
  this->sCurrSec23.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
  this->sCurrSec23.ui16FiltPhaC.u16Sh = this->ui16OffsetFiltWindow;
  this->sCurrSec45.ui16FiltPhaA.u16Sh = this->ui16OffsetFiltWindow;
  this->sCurrSec45.ui16FiltPhaB.u16Sh = this->ui16OffsetFiltWindow;
  
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaB);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec16.ui16FiltPhaC);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaA);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec23.ui16FiltPhaC);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaA);
  GDFLIB_FilterMAInit_F16((frac16_t)0, &this->sCurrSec45.ui16FiltPhaB);
}

void MCDRV_Curr3Ph2ShChanAssign(mcdrv_adc_t *this)
{
  
}


/*!
 * @brief Get three phase current from two shunts measurements
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShGet(mcdrv_adc_t *this)
{

}

/*!
 * @brief Function reads current samples and filter them based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalib(mcdrv_adc_t *this)
{
  switch (*this->pui16SVMSector)
  {
  case 2:
  case 3:
    
    /* sensing of offset IA, IC*/
    this->sCurrSec23.ui16CalibPhaA =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrA), &this->sCurrSec23.ui16FiltPhaA);
    this->sCurrSec23.ui16CalibPhaC =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrC), &this->sCurrSec23.ui16FiltPhaC);
    break;
    
  case 4:
  case 5:
    
    /* sensing of offset IA, IB*/
    this->sCurrSec45.ui16CalibPhaA =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrA), &this->sCurrSec45.ui16FiltPhaA);
    this->sCurrSec45.ui16CalibPhaB =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrB), &this->sCurrSec45.ui16FiltPhaB);
    
    break;
    
  case 1:
  case 6:
  default:
    
    /* sensing of offset IB, IC*/
    this->sCurrSec16.ui16CalibPhaB =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrB), &this->sCurrSec16.ui16FiltPhaB);
    this->sCurrSec16.ui16CalibPhaC =
      GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcCurrC), &this->sCurrSec16.ui16FiltPhaC);
    
    break;
  }
}

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_Curr3Ph2ShCalibSet(mcdrv_adc_t *this)
{
  
  /* pass calibration data for sector 1 and 6 */
  this->sCurrSec16.ui16OffsetPhaB = this->sCurrSec16.ui16CalibPhaB;
  this->sCurrSec16.ui16OffsetPhaC = this->sCurrSec16.ui16CalibPhaC;
  
  /* pass calibration data for sector 2 and 3 */
  this->sCurrSec23.ui16OffsetPhaA = this->sCurrSec23.ui16CalibPhaA;
  this->sCurrSec23.ui16OffsetPhaC = this->sCurrSec23.ui16CalibPhaC;
  
  /* pass calibration data for sector 4 and 5 */
  this->sCurrSec45.ui16OffsetPhaA = this->sCurrSec45.ui16CalibPhaA;
  this->sCurrSec45.ui16OffsetPhaB = this->sCurrSec45.ui16CalibPhaB;
  
}

/*!
 * @brief Function reads and passes DCB voltage sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_VoltDcBusGet(mcdrv_adc_t *this)
{

}

/*!
 * @brief Function reads and passes auxiliary sample
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
void MCDRV_AuxValGet(mcdrv_adc_t *this)
{

}

/*!
 * @brief Get measured ADC value stored in FIFO
 *
 * @param pBase LPADC peripheral base address
 *
 * @param pResult Pointer to structure variable that keeps the conversion result in conversion FIFO
 *
 * @return Status whether FIFO entry is valid
 */
static bool GetAdcResFIFO(ADC_Type *pBase, lpadc_conv_result_t *pResult, uint8_t index)
{
  assert(pResult != NULL); /* Check if the input pointer is available. */
  
  uint32_t tmp32;
  
  tmp32 = pBase->RESFIFO[index];
  
  if (0U == (ADC_RESFIFO_VALID_MASK & tmp32))
  {
    return false; /* FIFO is empty. Discard any read from RESFIFO. */
  }
  
  pResult->commandIdSource = (tmp32 & ADC_RESFIFO_CMDSRC_MASK) >> ADC_RESFIFO_CMDSRC_SHIFT;
  pResult->loopCountIndex = (tmp32 & ADC_RESFIFO_LOOPCNT_MASK) >> ADC_RESFIFO_LOOPCNT_SHIFT;
  pResult->triggerIdSource = (tmp32 & ADC_RESFIFO_TSRC_MASK) >> ADC_RESFIFO_TSRC_SHIFT;
  pResult->convValue = (uint16_t)(tmp32 & ADC_RESFIFO_D_MASK);
  
  return true;
}
