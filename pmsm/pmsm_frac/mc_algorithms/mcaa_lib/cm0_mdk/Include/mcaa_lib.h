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

/*!
*
* @brief Main MCAA_LIB header file for devices with fixed-point unit.
*
*******************************************************************************/
#ifndef _MCAA_LIB_H_
#define _MCAA_LIB_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
* Includes
*******************************************************************************/
#include "MCAA_EstimRL.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define MCAA_EstimRLInit_F16(u16SamplingFreq, psParam, psCtrl)                 \
        MCAA_EstimRLInit_F16_C(u16SamplingFreq, psParam, psCtrl)
#define MCAA_EstimRL_F16(f16UDcBus, psIAlBeFbck, psCtrl, psParam, psUAlBeReq)  \
        MCAA_EstimRL_F16_C(f16UDcBus, psIAlBeFbck, psCtrl, psParam, psUAlBeReq)

#if defined(__cplusplus)
}
#endif

#endif /* _MCAA_LIB_H_ */
