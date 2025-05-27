/*
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2025 NXP
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
#ifndef _MCDRV_EMIOS_MCXE_H_
#define _MCDRV_EMIOS_MCXE_H_

#include "fsl_common.h"
#include "mlib.h"
#include "mlib_types.h"
#include "fsl_device_registers.h"
#include "gmclib.h"
   

typedef struct _mcdrv_pwm3ph_emios_t
{
    GMCLIB_3COOR_T_F16 *psUABC;    /* pointer to the 3-phase pwm duty cycles */
    EMIOS_Type *pui32PwmBaseAddress; /* PWMA base address */
//    uint16_t ui16PhASubNum;        /* PWMA phase A sub-module number */
//    uint16_t ui16PhBSubNum;        /* PWMA phase B sub-module number */
//    uint16_t ui16PhCSubNum;        /* PWMA phase C sub-module number */
//    uint16_t ui16FaultFixNum;      /* PWMA fault number for fixed over-current fault detection */
//    uint16_t ui16FaultAdjNum;      /* PWMA fault number for adjustable over-current fault detection */
} mcdrv_pwm3ph_emios_t;

void MCDRV_eMIOS_PhSet(mcdrv_pwm3ph_emios_t *this);

bool_t MCDRV_eMIOS_PhFltGet(mcdrv_pwm3ph_emios_t *this);

   

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_EMIOS_MCXE_H_ */