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

#include "mcdrv_sar_adc_mcxe.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void MCDRV_CurrAndVoltDcBusGet(mcdrv_adc_t *this)
{
  
    uint16_t ui16Dummy;
    GMCLIB_3COOR_T_F16 sIABCtemp;
    bctu_fifo_res_t result[6U];

    if (kBCTU_Fifo_1_Int == (BCTU_GetFifoStatusFlags(BCTU) & kBCTU_Fifo_1_Int))
    {
        for(uint8_t index = 0U; index < 6U; ++index)
        {
            BCTU_GetFifoResult(BCTU, kBCTU_Fifo_1, &(result[index]));
        }

        BCTU_ClearFifoStatusFlags(BCTU, kBCTU_Fifo_1_Int);

        this->ui16AdcCurrA = result[0].convRes;
        this->ui16AdcCurrC = result[1].convRes;
        this->ui16AdcCurrB = result[2].convRes;
        ui16Dummy =  result[3].convRes;
        this->ui16AdcDCBVolt = result[4].convRes;
        ui16Dummy =  result[5].convRes;
        ui16Dummy = ui16Dummy; 
        
        *this->pf16UDcBus    = (frac16_t)(this->ui16AdcDCBVolt);
                
    }
    
    switch (*this->pui16SVMSector)
    {
        case 2:
        case 3:
            /* direct sensing of phase A and C, calculation of B */
            /* Read ADC result FIFO0 */
            sIABCtemp.f16A = (((frac16_t)(this->ui16AdcCurrA)) - this->sCurrSec23.ui16OffsetPhaA) << 1U;
            sIABCtemp.f16C = (((frac16_t)(this->ui16AdcCurrC)) - this->sCurrSec23.ui16OffsetPhaC) << 1U;
            sIABCtemp.f16B = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16C));

            break;

        case 4:
        case 5:
            /* direct sensing of phase A and B, calculation of C */
            sIABCtemp.f16A = (((frac16_t)(this->ui16AdcCurrA)) - this->sCurrSec45.ui16OffsetPhaA) << 1U;
            sIABCtemp.f16B = (((frac16_t)(this->ui16AdcCurrB)) - this->sCurrSec45.ui16OffsetPhaB) << 1U;
            sIABCtemp.f16C = MLIB_Neg_F16(MLIB_AddSat_F16(sIABCtemp.f16A, sIABCtemp.f16B));

            break;

        case 1:
        case 6:
        default:
            /* direct sensing of phase B and C, calculation of A */
            sIABCtemp.f16B = (((frac16_t)(this->ui16AdcCurrB)) - this->sCurrSec16.ui16OffsetPhaB) << 1U;
            sIABCtemp.f16C = (((frac16_t)(this->ui16AdcCurrC)) - this->sCurrSec16.ui16OffsetPhaC) << 1U;
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