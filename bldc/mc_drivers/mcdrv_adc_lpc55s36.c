/*
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2016-2024 NXP
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
lpadc_conv_command_config_t lpadcCommandConfig1;

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
  /* Read available converted values from the FIFO[0]. */
  while( GetAdcResFIFO(this->pToAdcBase, &this->s_ADC_ResultStructure, 0U) )
  {
      switch( this->s_ADC_ResultStructure.commandIdSource )
      {
      case 1U:
        /* Command 1 - IDC_BUS */
        this->ui16AdcDCBCurr = (this->s_ADC_ResultStructure.convValue << 1);
        *this->pf16IDcBus = (frac16_t)((frac16_t)((this->ui16AdcDCBCurr) -  this->ui16OffsetDcCurr)) ;
        break;
        
      case 2U:
        /* Command 2 - BEMF_A */
        this->ui16AdcBemfA = (this->s_ADC_ResultStructure.convValue);
        break;
        
      case 3U:
        /* Command 3 - BEMF_B */
        this->ui16AdcBemfB = (this->s_ADC_ResultStructure.convValue);
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
        /* Command 1 - UDC_BUS */
        this->ui16AdcDCBVolt = (this->s_ADC_ResultStructure.convValue) ;
        *this->pf16UDcBus = (frac16_t)(this->ui16AdcDCBVolt);
        break;
        
      case 2U:
        /* Command 2 - BEMF_C */
        this->ui16AdcBemfC = (this->s_ADC_ResultStructure.convValue);
        break;
        
      default:
        break;
      }
  }
  
  
  /* Selects the right value of BEMF voltage for working with */
  switch (this->ui16Sector)
  {
  case 0:
  case 3:
    
    *this->pf16BemfVoltage = (frac16_t)(this->ui16AdcBemfC);
      
    break;
    
  case 1:
  case 4:
    
    *this->pf16BemfVoltage = (frac16_t)(this->ui16AdcBemfB);
      
    break;
    
  case 2:
  case 5:
    
    *this->pf16BemfVoltage = (frac16_t)(this->ui16AdcBemfA);
      
    break;
    
  default:
    break;
  }
  
}

/*!
 * @brief Function initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalibInit(mcdrv_adc_t *this)
{ 
  
    /* clear offset values */
    this->ui16OffsetDcCurr = 0U;
    this->ui16CalibDcCurr  = 0U;

    /* initialize offset filters */
    this->ui16FiltDcCurr.u16Sh = this->ui16OffsetFiltWindow;

    GDFLIB_FilterMAInit_F16((frac16_t)0, &this->ui16FiltDcCurr);
    
}

/*!
 * @brief Function reads current samples and filter them
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalib(mcdrv_adc_t *this)
{
    this->ui16CalibDcCurr = GDFLIB_FilterMA_F16((frac16_t)(this->ui16AdcDCBCurr), &this->ui16FiltDcCurr);
}

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalibSet(mcdrv_adc_t *this)
{
    
    /* pass Calib data for DC Bus current offset */
    this->ui16OffsetDcCurr = (this->ui16CalibDcCurr);
    
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