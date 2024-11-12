/*
* Copyright 2013-2016 Freescale Semiconductor, Inc.
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

#ifndef _MCDRV_ADC_LPC_H_
#define _MCDRV_ADC_LPC_H_

#include "fsl_lpadc.h"

#include "gdflib.h"
#include "mlib_types.h"
#include "gmclib.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MCDRV_ADC (3)

/* init sensors/actuators pointers */
#define M1_SET_PTR_U_DC_BUS(par1) (g_sM1AdcSensor.pf16UDcBus = &(par1))
#define M1_SET_PTR_I_DC_BUS(par1) (g_sM1AdcSensor.pf16IDcBus = &(par1))
#define M1_SET_PTR_BEMF_VOLT(par1) (g_sM1AdcSensor.pf16BemfVoltage = &(par1))
#define M1_SET_PTR_AUX_CHAN(par1) (g_sM1AdcSensor.pui16AuxChan = &(par1))

typedef struct _mcdrv_adc
{
GDFLIB_FILTER_MA_T_A32 ui16FiltDcCurr; /* Dc-bus current offset filter */
    
    ADC_Type *pToAdcBase;
    
    uint16_t ui16OffsetFiltWindow;         /* ADC offset filter window */
    uint16_t ui16OffsetDcCurr;             /* Dc-bus current offset */
    uint16_t ui16CalibDcCurr;              /* Dc-bus current offset calibration */
    frac16_t *pf16BemfVoltage;             /* pointer to actual BEMF voltage     */
    frac16_t *pf16IDcBus;                  /* pointer to actual IDC bus current     */
    frac16_t *pf16UDcBus;                  /* pointer to actual UDC bus voltage     */
    uint16_t *pui16AuxChan;                /* pointer to actual auxiliary variable   */
    uint16_t ui16Sector;                   /* commutation sector */
    
    uint16_t ui16AdcBemfA;                 /* value of BemfA from ADC measurement */
    uint16_t ui16AdcBemfB;                 /* value of BemfB from ADC measurement */
    uint16_t ui16AdcBemfC;                 /* value of BemfC from ADC measurement */
    uint16_t ui16AdcDCBVolt;               /* value of UDC from ADC measurement */
    uint16_t ui16AdcDCBCurr;               /* value of IDC from ADC measurement */
   
    lpadc_conv_result_t s_ADC_ResultStructure;
} mcdrv_adc_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Reads and calculates 3 phase samples based on SVM sector
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrAndVoltDcBusGet(mcdrv_adc_t *this);

/*!
 * @brief Function initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalibInit(mcdrv_adc_t *this);

/*!
 * @brief Function reads current samples and filter them
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalib(mcdrv_adc_t *this);

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_CurrOffsetCalibSet(mcdrv_adc_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADC_LPC_H_ */
