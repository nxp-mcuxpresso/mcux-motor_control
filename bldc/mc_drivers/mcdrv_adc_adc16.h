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
#ifndef _MCDRV_ADC_ADC16_H_
#define _MCDRV_ADC_ADC16_H_

#include "gdflib.h"
#include "gmclib_types.h"
#include "fsl_device_registers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MCDRV_CHAN_OFF (0x1FU)
#define MCDRV_TEMP (0x1AU)

#define MCDRV_ADC0_BEMFA (0U)
#define MCDRV_ADC1_BEMFA (1U)
#define MCDRV_ADC0_BEMFB (2U)
#define MCDRV_ADC1_BEMFB (3U)
#define MCDRV_ADC0_BEMFC (4U)
#define MCDRV_ADC1_BEMFC (5U)
#define MCDRV_ADC0_UDCB (6U)
#define MCDRV_ADC1_UDCB (7U)
#define MCDRV_ADC0_IDCB (8U)
#define MCDRV_ADC1_IDCB (9U)
#define MCDRV_ADC0_AUX (10U)
#define MCDRV_ADC1_AUX (11U)

/* init sensors/actuators pointers */
#define M1_SET_PTR_U_DC_BUS(par1) (g_sM1AdcSensor.pf16UDcBus = &(par1))
#define M1_SET_PTR_I_DC_BUS(par1) (g_sM1AdcSensor.pf16IDcBus = &(par1))
#define M1_SET_PTR_BEMF_VOLT(par1) (g_sM1AdcSensor.pf16BemfVoltage = &(par1))
#define M1_SET_PTR_AUX_CHAN(par1) (g_sM1AdcSensor.pui16AuxChan = &(par1))

typedef struct _mcdrv_adc16
{
    GDFLIB_FILTER_MA_T_A32 ui16FiltDcCurr; /* Dc-bus current offset filter */
    ADC_Type *pui32UdcbAdcBase;            /* pointer to ADC where DC-bus voltage channel is assigned */
    ADC_Type *pui32IdcbAdcBase;            /* pointer to ADC where DC-bus current channel is assigned */
    ADC_Type *pui32BemfAAdcBase;           /* pointer to ADC where BEMF phase A channel is assigned */
    ADC_Type *pui32BemfBAdcBase;           /* pointer to ADC where BEMF phase B channel is assigned */
    ADC_Type *pui32BemfCAdcBase;           /* pointer to ADC where BEMF phase C channel is assigned */
    ADC_Type *pui32AuxAdcBase;             /* pointer to ADC where AUX channel is assigned  */
    uint16_t ui16IndexAux;                 /* auxiliary ADC channel index  */
    uint16_t ui16IndexUdcb;                /* DC-bus voltage ADC channel index  */
    uint16_t ui16IndexIdcb;                /* DC-bus current ADC channel index  */
    uint16_t ui16IndexBemf;                /* BEMF ADC channel index  */
    uint16_t ui16OffsetFiltWindow;         /* ADC offset filter window */
    uint16_t ui16OffsetDcCurr;             /* Dc-bus current offset */
    uint16_t ui16CalibDcCurr;              /* Dc-bus current offset calibration */
    frac16_t *pf16IDcBus;                  /* pointer to DC-bus  current variable */
    frac16_t *pf16UDcBus;                  /* pointer to DC-bus  voltage variable */
    frac16_t *pf16BemfVoltage;             /* pointer to actual BEMF voltage     */
    uint16_t *pui16AuxChan;                /* pointer to actual auxiliary variable   */
    uint16_t ui16Sector;                   /* commutation sector */
    uint16_t bldcAdc0SectorCfg[6];         /* array with BEMF channels assigned to ADC0 according commutation table */
    uint16_t bldcAdc1SectorCfg[6];         /* array with BEMF channels assigned to ADC1 according commutation table */
    uint16_t *pui16AdcArray;               /* pointer to ADC Array with channels numbers */
    ADC_Type *bldcAdcSelCfg[6]; /* array with ADC base addresses for BEMF sensing according commutation table*/
} mcdrv_adc16_t;

typedef struct _mcdrv_adc16_init
{
    uint16_t *ui16AdcArray;  /* pointer to ADC Array */
    ADC_Type *pui32Adc0Base; /* pointer to ADC0 base address */
    ADC_Type *pui32Adc1Base; /* pointer to ADC1 base address */
} mcdrv_adc16_init_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initializes ADC driver to measure DC-bus current, DC-bus voltage
 *        and BEMF voltage for BLDC sensorless algorithm
 *
 * @param this   Pointer to the current object
 * @param init   Pointer to initialization structure
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Adc16Init(mcdrv_adc16_t *this, mcdrv_adc16_init_t *init);

/*!
 * @brief Function set new channel assignment for next BEMF voltage sensing
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AssignBemfChannel(mcdrv_adc16_t *this);

/*!
 * @brief Function reads ADC result register containing actual BEMF voltage
 *
 * Result register value is shifted three times to the right and stored
 * to BEMF voltage pointer
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_BemfVoltageGet(mcdrv_adc16_t *this);

/*!
 * @brief Function reads ADC result register containing actual DC-bus voltage sample
 *
 * Result register value is shifted three times to the right and stored
 * to DC-bus voltage pointer
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_VoltDcBusGet(mcdrv_adc16_t *this);

/*!
 * @brief Function reads ADC result register containing actual DC-bus current sample
 *
 * Result register value is shifted three times to the right and stored
 * to DC-bus current pointer
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_CurrDcBusGet(mcdrv_adc16_t *this);

/*!
 * @brief Function reads ADC result register containing actual auxiliary sample
 *
 * Result register value is shifted 3 times right and stored to
 * auxiliary pointer
 *
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AuxValGet(mcdrv_adc16_t *this);

/*!
 * @brief Function initializes phase current channel offset measurement
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_CurrOffsetCalibInit(mcdrv_adc16_t *this);

/*!
 * @brief Function reads current samples and filter them
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_CurrOffsetCalib(mcdrv_adc16_t *this);

/*!
 * @brief Function passes measured offset values to main structure
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_CurrOffsetCalibSet(mcdrv_adc16_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADC_ADC16_H_ */
