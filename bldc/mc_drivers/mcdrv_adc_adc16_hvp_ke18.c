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
#include "mcdrv_adc_adc16.h"
#include "mcdrv_adc_adc16_hvp_ke18.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static bool_t s_statusPass;
static volatile bool_t s_bkey = TRUE;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function set new channel assignment for next BEMF voltage sensing.
 *        Board HVP-KE18F16 specific function.
 *
 * @param this   Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_AssignBemfChannel_hvp_ke18(mcdrv_adc16_t *this)
{
    s_statusPass = TRUE;

    switch (this->ui16Sector)
    {
            /* BEMF phase C sensing */
        case 0:
        case 3:
            if ((this->bldcAdcSelCfg[this->ui16Sector]) == ADC1)
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc0SectorCfg[this->ui16Sector]));
            else
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc1SectorCfg[this->ui16Sector]));
            break;

            /* BEMF phase B sensing */
        case 1:
        case 4:
            if ((this->bldcAdcSelCfg[this->ui16Sector]) == ADC1)
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc0SectorCfg[this->ui16Sector]));
            else
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc1SectorCfg[this->ui16Sector]));

            break;

            /* BEMF phase A sensing */
        case 2:
        case 5:
            if ((this->bldcAdcSelCfg[this->ui16Sector]) == ADC1)
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc0SectorCfg[this->ui16Sector]));
            else
                /* Set ADC_SC1_ADCH bitfield */
                this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] =
                    (this->bldcAdcSelCfg[this->ui16Sector]->SC1[this->ui16IndexBemf] &
                     ~(uint16_t)(ADC_SC1_ADCH(ADC_SC1_ADCH_MASK))) |
                    (ADC_SC1_ADCH(this->bldcAdc1SectorCfg[this->ui16Sector]));

            break;

        default:
            break;
    }

    return (s_statusPass);
}
