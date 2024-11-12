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

#ifndef _MCDRV_ADC_ADC16_TWR_KV11_H_
#define _MCDRV_ADC_ADC16_TWR_KV11_H_

#include "gdflib.h"
#include "gmclib_types.h"
#include "fsl_device_registers.h"
#include "mcdrv_adc_adc16.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

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
 *  Board specific file for the tower kv11
 *
 * @param this   Pointer to the current object
 * @param init   Pointer to initialization structure
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Adc16Init_twr_kv11(mcdrv_adc16_t *this, mcdrv_adc16_init_t *init);

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
bool_t MCDRV_VoltDcBusGet_twr_kv11(mcdrv_adc16_t *this);

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
bool_t MCDRV_CurrDcBusGet_twr_kv11(mcdrv_adc16_t *this);

/*!
 * @brief Function reads current samples and filter them
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_CurrOffsetCalib_twr_kv11(mcdrv_adc16_t *this);

/*!
 * @brief Function set new channel assignment for next BEMF voltage sensing
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AssignDCBusChannel(mcdrv_adc16_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADC_ADC16_TWR_KV11_H_ */
