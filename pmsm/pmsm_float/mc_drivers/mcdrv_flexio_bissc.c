/*
* Copyright 2024 NXP
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

#include "mcdrv_flexio_bissc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Make sure the shifer index for rxd is biggest. Not to modify these definitions. */
#define TRG_SHIFTER    0
#define RXD_SHIFTER    1

#define DELAY_MAX                   10u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t FLEXIO_BISSC_GetInstance(FLEXIO_BISSC_Type *base);
static uint8_t FLEXIO_BISSC_CRC6(uint32_t *data, uint32_t len);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint8_t rx_shifter_count;

/*******************************************************************************
 * Codes
 ******************************************************************************/
RAM_FUNC_LIB
static uint32_t FLEXIO_BISSC_GetInstance(FLEXIO_BISSC_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

RAM_FUNC_LIB
void FLEXIO_BISSC_Init(FLEXIO_BISSC_Type *base, flexio_bissc_config_t *config, uint32_t srcClock_Hz)
{
    assert(base != NULL);
    assert(config != NULL);

    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;
    uint32_t ctrlReg  = 0u;
    uint16_t timerDiv = 0u;
    uint16_t timerCmp = 0u;

    uint32_t data_len = base->mt_len + base->st_len + 1u + 1u + FLEXIO_BISSC_CRC_LEN;     /* MT, ST, Error(1), Warn(1), CRC */
    if (data_len < 32u)
    {
        rx_shifter_count = 1u;
    }
    else
    {
        rx_shifter_count = 2u;
    }

    /* Clear the shifterConfig & timerConfig struct. */
    (void)memset(&shifterConfig, 0, sizeof(shifterConfig));
    (void)memset(&timerConfig, 0, sizeof(timerConfig));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_BISSC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Configure FLEXIO BISSC */
    ctrlReg = base->flexioBase->CTRL;
    ctrlReg &= ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    ctrlReg |= (FLEXIO_CTRL_DBGE(config->enableInDebug) | FLEXIO_CTRL_FASTACC(config->enableFastAccess) |
                FLEXIO_CTRL_FLEXEN(config->enable));
    if (!config->enableInDoze)
    {
        ctrlReg |= FLEXIO_CTRL_DOZEN_MASK;
    }

    base->flexioBase->CTRL = ctrlReg;

    /* Do hardware configuration. */
    /* 1. Configure the shifter for timer trigger. */
    shifterConfig.timerSelect   = base->timerIndex;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeTransmit;

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterStartIndex + TRG_SHIFTER, &shifterConfig);

    /* 2. Configure the shifter for rx. */
    shifterConfig.timerSelect   = base->timerIndex;
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect     = base->RxdPinIndex;
    shifterConfig.pinPolarity   = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromNextShifterOutput;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;

    for (uint32_t i = 0u; i < rx_shifter_count - 1u && i < 8u; i++)
    {
        FLEXIO_SetShifterConfig(base->flexioBase, base->shifterStartIndex + RXD_SHIFTER + i, &shifterConfig);
    }

    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromPin;
    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterStartIndex + RXD_SHIFTER + rx_shifter_count - 1, &shifterConfig);

    /*3. Configure the timer for Clock. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->shifterStartIndex + TRG_SHIFTER);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
    timerConfig.pinSelect       = base->ClockPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveLow;
    timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputZeroNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerHigh;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;

    /* Low 8-bits are used to configure baudrate. */
    timerDiv = (srcClock_Hz / config->baudRate_Bps) / 2u - 1u;
    timerCmp = ((1u * 2u - 1u) << 8u) | timerDiv;
    timerConfig.timerCompare    = timerCmp;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex, &timerConfig);

    /* Receive any bits to pull Clock high. */
    base->flexioBase->TIMCMP[base->timerIndex] = timerCmp;
    base->flexioBase->SHIFTBUF[base->shifterStartIndex + TRG_SHIFTER] = 0; /* Trigger the transfer. */
    while (!(base->flexioBase->SHIFTSTAT & (1 << base->shifterStartIndex + RXD_SHIFTER)))
    {}
    volatile uint32_t tmp = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER]; /* Flush the SHIFTBUF to clear the status flag. */

    SDK_DelayAtLeastUs(30u, SystemCoreClock);
}

RAM_FUNC_LIB
void FLEXIO_BISSC_EnableRxInterrupt(FLEXIO_BISSC_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->SHIFTSIEN |= 1 << base->shifterStartIndex + RXD_SHIFTER;
    }
    else
    {
        base->flexioBase->SHIFTSIEN &= ~(1 << base->shifterStartIndex + RXD_SHIFTER);
    }
}

RAM_FUNC_LIB
int FLEXIO_BISSC_Delay_Compensate(FLEXIO_BISSC_Type * base)
{
    uint32_t delay_clks = 0;
    uint32_t timerCmp;
    uint32_t i;
    register uint32_t data;
    int status;

    timerCmp = base->flexioBase->TIMCMP[base->timerIndex];
    timerCmp &= 0xFFFF00FF;
    timerCmp |= ((uint16_t)DELAY_MAX * 2u - 1u) << 8u;

    base->flexioBase->TIMCMP[base->timerIndex] = timerCmp;

    for (i = 0; i < 2; i ++)
    {
        do {
            status = FLEXIO_BISSC_ReadBlocking(base);
        } while(status);                                   /* Data In Pin is low. Encoder is busy. */
        SDK_DelayAtLeastUs(30u, SystemCoreClock);
    }

    data = base->data[0];
    data <<= 32u - DELAY_MAX + 2u;

    for (i = 0; i < DELAY_MAX; i++)
    {
        if (data & 0x80000000)
        {
            data <<= 1u;
            delay_clks++;
        }
        else
        {
            break;
        }
    }

    if (i >= DELAY_MAX)
    {
        return -1;
    }

    uint32_t clk_len = delay_clks + 4u + 1u + 1u + 1u + \
                       base->mt_len + base->st_len + \
                       1u + 1u + FLEXIO_BISSC_CRC_LEN;    /* Delay, Ack(1), Start(1), CDS(1); MT, ST, Error(1), Warn(1), CRC */

    timerCmp &= 0xFFFF00FF;
    timerCmp |= (clk_len * 2U - 1U) << 8u;

    base->flexioBase->TIMCMP[base->timerIndex] = timerCmp;
    SDK_DelayAtLeastUs(1u, SystemCoreClock);

    return 0;
}

RAM_FUNC_LIB
static uint8_t FLEXIO_BISSC_CRC6(uint32_t *data, uint32_t len)  /* P(x) = x6 + x1 + x0; MSB first in data. */
{
    register uint8_t crc = 0;
    register uint32_t tmp32;
    uint32_t len_remainder;
    uint8_t b0, b1;

    while (len)
    {
        len_remainder = len & 31u;
        if (len_remainder)
        {
            tmp32 = data[len >> 5u];
        }
        else
        {
            len_remainder = 32u;
            tmp32 = data[(len >> 5u) - 1u];
        }
        asm("rbit %0, %1" : "=r" (tmp32) : "r" (tmp32));        /* reverse bit order. MSb first to calculate CRC. */
        tmp32 >>= 32u - len_remainder;
        len -= len_remainder;

        while(len_remainder--)
        {
            b0 = (tmp32 ^ (crc >> 5u)) & 1u;
            b1 = (crc ^ b0) & 1u;
            crc <<= 1u;
            crc &= 0xFC;                                        /* Clear bit0 and bit1. */
            crc |= (b1 << 1u) | b0;
            tmp32 >>= 1u;
        }
    }

    return (~crc) & 0x3F;
}

RAM_FUNC_LIB
void FLEXIO_BISSC_Data_Proc(FLEXIO_BISSC_Type * base)
{
    uint64_t data64 = *(uint64_t *)base->data;
    uint8_t crc_recv, crc_cal;

    /* Parsing CRC from senzor */
    crc_recv = data64 & ((1u << FLEXIO_BISSC_CRC_LEN) - 1u);

    /* Shift about CRC length */
    data64 >>= FLEXIO_BISSC_CRC_LEN;
    
    /* CRC calculation */
    crc_cal = FLEXIO_BISSC_CRC6((uint32_t *)&data64, base->mt_len + base->st_len + 1u + 1u);
    
    /* CRC validation */
    base->crc_match = (crc_cal == crc_recv);

    /* Parsing warning bit */
    base->warn_bit = data64 & 1u;

    /* Shift about warning bit */
    data64 >>= 1;
    
    /* Parsing error bit */
    base->error_bit = data64 & 1u;

    /* Shift about error bit */
    data64 >>= 1;
      
    /* Parsing single turn data */
    /* for 16 bit is mask 0xffff = 0b1111 1111 1111 1111, base->st = data64 & (0xffff); */  
    base->st = data64 & ((1u << base->st_len) - 1U);
    
    /* Shift about single turn length */
    data64 >>= base->st_len;
    
    /* Parsing multi turn data */
    /* for 12 bit is mask 0xfff = 0b1111 1111 1111, base->mt = data64 & (0xfff); */
    base->mt = data64 & ((1u << base->mt_len) - 1U);

    /* mechanical position from single turn */
    base->f16PosMe = (frac16_t)(base->st - base->st_offset);
    
    ////////////////////////////////////////////////////////////////////////////
    /////////////// Tracking observer //////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
       
    /* tracking observer calculation */
    base->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(base->a32PosErr, &base->sTo);

    /* calculation of error function for tracking observer */
    base->a32PosErr = (acc32_t)MLIB_Sub_F16(base->f16PosMe, base->f16PosMeEst);

    /* speed estimation by the tracking observer */
    base->fltSpdMeEst = base->sTo.fltSpeed;
    
    /* position in accumulator type for motor control purposes */
    base->a32PosMeReal = (acc32_t)(( (((int32_t)base->mt) - 2048) << 15    ) + (((uint16_t)(base->st)) >> 1) ); 
    
    //base->a32PosMeReal = (acc32_t)(( (((int32_t)(base->mt - base->mt_offset)) - 2048) << 15    ) + (((uint16_t)(base->st - base->st_offset)) >> 1) );
    
    /* store results to user-defined variables */
    *base->pf16PosElEst = (frac16_t)(base->f16PosMeEst * base->ui16Pp);
    *base->pfltSpdMeEst = (base->fltSpdMeEst);
    

}

RAM_FUNC_LIB
int FLEXIO_BISSC_ReadBlocking(FLEXIO_BISSC_Type *base)
{
#if 0   /* Check if the encoder is ready. Optional. For safety purpose. */
    if (!(base->flexioBase->PIN & (1u << base->RxdPinIndex)))
    {
        return -1;
    }
#endif

#if 0   /* Flush the SHIFTBUF to clear the status flag. Optional. For safety purpose. */
    volatile uint32_t tmp = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER]; /* Flush the SHIFTBUF to clear the status flag. */
#endif
    base->flexioBase->SHIFTBUF[base->shifterStartIndex + TRG_SHIFTER] = 0;               /* Trigger the transfer. */

    /* Wait until data transfer complete. */
    while (!(base->flexioBase->SHIFTSTAT & (1 << base->shifterStartIndex + RXD_SHIFTER)))
    {}

    for (uint32_t i = 0; i< rx_shifter_count; i++)
    {
        base->data[i] = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER + rx_shifter_count - 1u - i];
    }

    return 0;
}

RAM_FUNC_LIB
int FLEXIO_BISSC_ReadNonBlocking(FLEXIO_BISSC_Type *base)
{
#if 0   /* Check if the encoder is ready. Optional. For safety purpose. */
    if (!(base->flexioBase->PIN & (1u << base->RxdPinIndex)))
    {
        return -1;
    }
#endif

#if 0   /* Flush the SHIFTBUF to clear the status flag. Optional. For safety purpose. */
    volatile uint32_t tmp = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER]; /* Flush the SHIFTBUF to clear the status flag. */
#endif
    base->flexioBase->SHIFTBUF[base->shifterStartIndex + TRG_SHIFTER] = 0;  /* Trigger the transfer. */

    return 0;
}

RAM_FUNC_LIB
static void FLEXIO_BISSC_ISR(void *bisscType, void *bisscDummyHandle)
{
    FLEXIO_BISSC_Type *base = (FLEXIO_BISSC_Type *)bisscType;

    for (uint32_t i = 0; i< rx_shifter_count; i++)
    {
        base->data[i] = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER + rx_shifter_count - 1u - i];
    }

    if (base->callback != NULL)
    {
        base->callback(base);
    }
}

RAM_FUNC_LIB
status_t FLEXIO_BISSC_RegisterIRQ(FLEXIO_BISSC_Type *base)
{
    static uint32_t bisscDummyHandle;

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Flush the SHIFTBUF to clear the status flag. */
    volatile uint32_t tmp = base->flexioBase->SHIFTBUFBIS[base->shifterStartIndex + RXD_SHIFTER];

    /* Clear pending NVIC IRQ before enable NVIC IRQ. */
    NVIC_ClearPendingIRQ(flexio_irqs[FLEXIO_BISSC_GetInstance(base)]);
    /* Enable interrupt in NVIC. */
    (void)EnableIRQ(flexio_irqs[FLEXIO_BISSC_GetInstance(base)]);

    /* Save the context in global variables to support the double weak mechanism. */
    return FLEXIO_RegisterHandleIRQ(base, &bisscDummyHandle, FLEXIO_BISSC_ISR);
}

/*!
 * @brief Function clears internal variables and decoder counter
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissClear(FLEXIO_BISSC_Type *base)
{

    base->a32PosMeReal = ACC32(0.0);         /* real position (revolution counter + mechanical position) */
    base->a32PosErr = ACC32(0.0);            /* position error to tracking observer  */
    base->fltSpdMeEst = 0.0F;          /* estimated speed calculated using tracking observer */
    base->f16PosMe = FRAC16(0.0);            /* mechanical position calculated using encoder edges */
    base->f16PosMeEst = FRAC16(0.0);         /* estimated position calculated using tracking observer */

    /* initilize tracking observer */
    base->sTo.f32Theta = FRAC32(0.0);
    base->sTo.fltSpeed = 0.0F;
    base->sTo.fltI_1   = 0.0F;
    
    base->mt_offset = 0U;
    base->st_offset = 0U;
   
}


/*!
 * @brief Function set positon offset after align
 *
 * @param this   Pointer to the current object
 *
 * @return none
 */
RAM_FUNC_LIB
void MCDRV_BissSetOffset(FLEXIO_BISSC_Type *base)
{
//    base->f16PosMeOffset =  (frac16_t)(base->f16PosMe);
  base->mt_offset = base->mt;
  base->st_offset = base->st;

}





////////////////////////////////////////////////////////////////////////////////
/////////// BISSC //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


RAM_FUNC_LIB
void MCDRV_BissCClear(BISSC_Type *base)
{

    base->a32PosMeReal = ACC32(0.0);         /* real position (revolution counter + mechanical position) */
    base->a32PosErr = ACC32(0.0);            /* position error to tracking observer  */
    base->fltSpdMeEst = 0.0F;          /* estimated speed calculated using tracking observer */
    base->f16PosMe = FRAC16(0.0);            /* mechanical position calculated using encoder edges */
    base->f16PosMeEst = FRAC16(0.0);         /* estimated position calculated using tracking observer */

    /* initilize tracking observer */
    base->sTo.f32Theta = FRAC32(0.0);
    base->sTo.fltSpeed = 0.0F;
    base->sTo.fltI_1   = 0.0F;
    
    base->mt_offset = 0U;
    base->st_offset = 0U;
   
}

RAM_FUNC_LIB
void MCDRV_BissCSetOffset(BISSC_Type *base)
{
//    base->f16PosMeOffset =  (frac16_t)(base->f16PosMe);
  base->mt_offset = base->mt;
  base->st_offset = base->st;

}

RAM_FUNC_LIB
void BISSC_Data_Proc(BISSC_Type * base)
{

    /* mechanical position from single turn */
    base->f16PosMe = (frac16_t)(base->st - base->st_offset);
    
    ////////////////////////////////////////////////////////////////////////////
    /////////////// Tracking observer //////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
       
    /* tracking observer calculation */
    base->f16PosMeEst = (frac16_t)AMCLIB_TrackObsrv_A32af(base->a32PosErr, &base->sTo);

    /* calculation of error function for tracking observer */
    base->a32PosErr = (acc32_t)MLIB_Sub_F16(base->f16PosMe, base->f16PosMeEst);

    /* speed estimation by the tracking observer */
    base->fltSpdMeEst = base->sTo.fltSpeed;
    
    /* position in accumulator type for motor control purposes */
    base->a32PosMeReal = (acc32_t)(( (((int32_t)base->mt) - 2048) << 15    ) + (((uint16_t)(base->st)) >> 1) ); 
    
    //base->a32PosMeReal = (acc32_t)(( (((int32_t)(base->mt - base->mt_offset)) - 2048) << 15    ) + (((uint16_t)(base->st - base->st_offset)) >> 1) );
    
    /* store results to user-defined variables */
    *base->pf16PosElEst = (frac16_t)(base->f16PosMeEst * base->ui16Pp);
    *base->pfltSpdMeEst = (base->fltSpdMeEst);
    

}
