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

#include "mcdrv_mosfet_predrv_qspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

static bool_t MCDRV_Driver3PhSendCmd(mcdrv_spi_drv3ph_t *this, uint8_t *pui8TxData, uint8_t *pui8RxData);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static bool_t s_statusPass;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Function send SPI command to MC33937
 *
 * @param this Pointer to the current object
 * @param pui8TxData Pointer to data which be send via SPI
 * @param pui8RxData Pointer to data which be read via SPI
 *
 * @return boot_t true on success
 */
static bool_t MCDRV_Driver3PhSendCmd(mcdrv_spi_drv3ph_t *this, uint8_t *pui8TxData, uint8_t *pui8RxData)
{
    uint16_t ui16SafetyCounter = 0; /* eliminate code freezing in while loop */

    s_statusPass = TRUE;

    /* Waits until empty buffer */
	while (((this->sSpiData.pSpiBase->SPSCR & (QSPI_SPSCR_SPTE_MASK)) == 0) && (ui16SafetyCounter < 1000))
	{
		ui16SafetyCounter++;
	};
	
	ui16SafetyCounter = 0;
    
    /* push data */
    QSPI_WriteData(this->sSpiData.pSpiBase, (uint16_t)(*pui8TxData));

    /* Waits until data transferred  */
    while (((this->sSpiData.pSpiBase->SPSCR & (QSPI_SPSCR_SPRF_MASK)) == 0) && (ui16SafetyCounter < 1000))
    {
        ui16SafetyCounter++;
    }

    /* load value to variable from register */
    *pui8RxData = (uint8_t)QSPI_ReadData(this->sSpiData.pSpiBase);

    return (s_statusPass);
}

/*!
 * @brief Function read MC33937 interrupt pin
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhReadInt(mcdrv_spi_drv3ph_t *this)
{
    s_statusPass = TRUE;

    /* read interrupt pin */
    s_statusPass = (bool_t)GPIO_PinRead(this->sSpiData.pGpioIntBase, (gpio_pin_t)this->sSpiData.ui16GpioIntPin);

    return (s_statusPass);
}

/*!
 * @brief Function clear MC33937 reset pin
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhClearRst(mcdrv_spi_drv3ph_t *this)
{
    s_statusPass = TRUE;

    /* clear pin output for 3PPA driver reset */
    GPIO_PinClear(this->sSpiData.pGpioResetBase, (gpio_pin_t)this->sSpiData.ui16GpioResetPin);

    return (s_statusPass);
}

/*!
 * @brief Function set MC33937 reset pin
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhSetRst(mcdrv_spi_drv3ph_t *this)
{
    s_statusPass = TRUE;

    /* set pin output for 3PPA driver reset */
    GPIO_PinSet(this->sSpiData.pGpioResetBase, (gpio_pin_t)this->sSpiData.ui16GpioResetPin);

    return (s_statusPass);
}

/*!
 * @brief Function clear MC33937 enable pin
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhClearEn(mcdrv_spi_drv3ph_t *this)
{
    s_statusPass = TRUE;

    GPIO_PinClear(this->sSpiData.pGpioEnBase, (gpio_pin_t)this->sSpiData.ui16GpioEnPin);

    return (s_statusPass);
}

/*!
 * @brief Function set MC33937 enable pin
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhSetEn(mcdrv_spi_drv3ph_t *this)
{
    s_statusPass = TRUE;

    GPIO_PinSet(this->sSpiData.pGpioEnBase, this->sSpiData.ui16GpioEnPin);

    return (s_statusPass);
}

/*!
 * @brief Function set MC33937 deadtime
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhSetDeadtime(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData, ui8RxData;

    s_statusPass = TRUE;

    /* zero deadtime calibration */
    /* send ZERODEADTIME command for dead time configuration with zero
       calibration */
    ui8TxData = 0x80;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    return (s_statusPass);
}

/*!
 * @brief Function clear MC33937 flag register
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhClearFlags(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData, ui8RxData;

    s_statusPass = TRUE;

    /* CLINT0_COMMAND = 0x6F */
    ui8TxData = 0x6F;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    /* CLINT1_COMMAND = 0x7F */
    ui8TxData = 0x7F;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    return (s_statusPass);
}

/*!
 * @brief Function read MC33937 status register 0
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhGetSr0(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData;

    s_statusPass = TRUE;

    /* status Register 0 reading = 0x00 */
    ui8TxData = 0x00;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr0.R);

    /* status Register 0 write to read SR0 result */
    ui8TxData = 0x00;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr0.R);

    return (s_statusPass);
}

/*!
 * @brief Function read MC33937 status register 1
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhGetSr1(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData;

    s_statusPass = TRUE;

    /* status Register 1 reading = 0x01 */
    ui8TxData = 0x01;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr0.R);

    /* status Register 0 write to read SR1 result */
    ui8TxData = 0x00;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr1.R);

    return (s_statusPass);
}

/*!
 * @brief Function read MC33937 status register 2
 *
 * @param this          Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhGetSr2(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData;

    s_statusPass = TRUE;

    /* status Register 2 reading = 0x02 */
    ui8TxData = 0x02;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr0.R);

    /* status Register 0 write to read SR2 result */
    ui8TxData = 0x00;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr2.R);

    return (s_statusPass);
}

/*!
 * @brief Function read MC33937 status register 3
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhGetSr3(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData;

    s_statusPass = TRUE;

    /* status Register 3 reading = 0x03 */
    ui8TxData = 0x03;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr0.R);

    /* status Register 0 write to read SR3 result */
    ui8TxData = 0x00;
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &this->sSr3);

    return (s_statusPass);
}

/*!
 * @brief Function configure MC33937 pre-driver
 *
 * @param this Pointer to the current object
 *
 * @return boot_t true on success
 */
bool_t MCDRV_Driver3PhConfig(mcdrv_spi_drv3ph_t *this)
{
    uint8_t ui8TxData, ui8RxData;
    register uint32_t ui32loop_cnt;

    s_statusPass = TRUE;

    /* EN1 and EN2 are still low */
    s_statusPass &= MCDRV_Driver3PhClearEn(this);

    if (this->sSpiData.bResetPinControl)
    {
        /* set RST high */
        s_statusPass &= MCDRV_Driver3PhSetRst(this);
    }
    /* required start-up delay between the RST going up and initialization
       routine at 75 MHz the delay will be 50 ms , at lower CPU clocks will be
       more than 2 ms */
    for (ui32loop_cnt = 0; ui32loop_cnt < 1500000; ui32loop_cnt++)
    {
    	asm(nop);
    }

    /* clear all faults */
    s_statusPass &= MCDRV_Driver3PhClearFlags(this);

    /* initialize MASK register 0 */
    ui8TxData = 0x20 | (uint8_t)(this->sInterruptEnable.R);
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    /* initialize MASK register 1 */
    ui8TxData = 0x30 | (uint8_t)(this->sInterruptEnable.R >> 4);
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    /* initialize MODE_COMMAND register  */
    ui8TxData = 0x40 | (uint8_t)(this->sMode.R);
    s_statusPass &= MCDRV_Driver3PhSendCmd(this, &ui8TxData, &ui8RxData);

    /* set dead time, ONLY ZERO DT AVAILABLE - TBD */
    s_statusPass &= MCDRV_Driver3PhSetDeadtime(this);

    /* clear all faults */
    s_statusPass &= MCDRV_Driver3PhClearFlags(this);

    return (s_statusPass);
}
