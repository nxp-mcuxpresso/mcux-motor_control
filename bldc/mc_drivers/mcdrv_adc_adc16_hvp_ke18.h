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
#ifndef _MCDRV_ADC_ADC16_HVP_KV11_H_
#define _MCDRV_ADC_ADC16_HVP_KV11_H_

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
 * @brief Function set new channel assignment for next BEMF voltage sensing.
 *        Board HVP-KE18F16 specific function.
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AssignBemfChannel_hvp_ke18(mcdrv_adc16_t *this);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_ADC_ADC16_TWR_KV11_H_ */
