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
#ifndef _MCDRV_ADC_HSADC_HVP_KV58_H_
#define _MCDRV_ADC_HSADC_HVP_KV58_H_

#include "gdflib.h"
#include "mlib_types.h"
#include "gmclib.h"
#include "fsl_device_registers.h"
#include "mcdrv_adc_hsadc.h"

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Initializes ADC driver to measure DC-bus current, DC-bus voltage
 *        and BEMF voltage for BLDC sensorless algorithm
 *        Board HVP-KV58F22 specific function
 *
 * @param this   Pointer to the current object
 * @param init   Pointer to initialization structure
 *
 * @return boot_t true on success
 */
bool_t MCDRV_HSAdcInit_hvp_kv58(mcdrv_hsadc_t *this, mcdrv_hsadc_init_t *init);

/*!
 * @brief Function set new channel assignment for next BEMF voltage sensing.
 *        Board HVP-KV58F22 specific function.
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AssignBemfChannel_hvp_kv58(mcdrv_hsadc_t *this);

/*!
 * @brief Function reads ADC result register containing actual DC-bus current sample
 *
 *        Board HVP-KV58F22 specific function
 *
 *        Result register value is shifted three times to the right and stored
 *        to DC-bus current pointer
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_BemfVoltageGet_hvp_kv58(mcdrv_hsadc_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADC_HSADC_HVP_KV58_H_ */
