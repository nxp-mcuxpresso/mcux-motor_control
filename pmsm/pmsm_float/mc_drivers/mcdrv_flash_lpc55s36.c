/*******************************************************************************
*
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
* 
*
****************************************************************************/

#include "mcdrv_flash_lpc55s36.h"
#include "mc_periph_init.h"
#include "m1_pmsm_appconfig.h"
#include "m1_sm_snsless_enc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CFG_DEFAULT {\
  M1_I_MAX,\
  M1_U_DCB_MAX,\
  M1_N_MAX,\
  M1_U_MAX,\
  M1_N_ANGULAR_MAX,\
  M1_U_DCB_OVERVOLTAGE, M1_U_DCB_UNDERVOLTAGE, M1_U_DCB_TRIP, M1_N_OVERSPEED, M1_N_MIN, M1_N_NOM, M1_E_BLOCK_TRH, M1_E_BLOCK_PER,\
  M1_CALIB_DURATION,\
  M1_FAULT_DURATION,\
  M1_FREEWHEEL_DURATION,\
  M1_SCALAR_UQ_MIN,\
  M1_FREQ_MAX,\
  M1_SCALAR_VHZ_FACTOR_GAIN,\
  M1_SCALAR_INTEG_GAIN,\
  M1_SCALAR_RAMP_UP,\
  M1_SCALAR_RAMP_DOWN,\
  M1_ALIGN_VOLTAGE,\
  M1_ALIGN_DURATION,\
  M1_OL_START_RAMP_INC,\
  M1_OL_START_RAMP_INC,\
  M1_OL_START_I,\
  M1_MERG_SPEED_TRH,\
  M1_MERG_COEFF,\
  M1_SCALAR_INTEG_GAIN,\
  M1_D_KP_GAIN,\
  M1_D_KI_GAIN,\
  M1_SPEED_PI_PROP_GAIN,\
  M1_SPEED_PI_INTEG_GAIN,\
  M1_CLOOP_LIMIT,\
  M1_SPEED_RAMP_UP,\
  M1_SPEED_RAMP_DOWN,\
  M1_SPEED_LOOP_HIGH_LIMIT,\
  M1_SPEED_LOOP_LOW_LIMIT,\
  M1_SPEED_PI_PROP_GAIN,\
  M1_SPEED_PI_INTEG_GAIN,\
  M1_SPEED_IIR_B0,\
  M1_SPEED_IIR_B1,\
  M1_SPEED_IIR_A1,\
  M1_UDCB_IIR_B0,\
  M1_UDCB_IIR_B1,\
  M1_UDCB_IIR_A1,\
  M1_I_SCALE,\
  M1_U_SCALE,\
  M1_E_SCALE,\
  M1_WI_SCALE,\
  M1_BEMF_DQ_KP_GAIN,\
  M1_BEMF_DQ_KI_GAIN,\
  M1_TO_KP_GAIN,\
  M1_TO_KI_GAIN,\
  M1_TO_THETA_GAIN,\
  M1_TO_SPEED_IIR_B0,\
  M1_TO_SPEED_IIR_B1,\
  M1_TO_SPEED_IIR_A1,\
  M1_POS_P_PROP_GAIN,\
  M1_MOTOR_PP,\
  M1_POSPE_ENC_PULSES,\
  M1_POSPE_ENC_DIRECTION,\
  M1_POSPE_ENC_N_MIN,\
  M1_POSPE_MECH_POS_GAIN,\
  M1_POSPE_TO_KP_GAIN,\
  M1_POSPE_TO_KI_GAIN,\
  M1_POSPE_TO_THETA_GAIN,\
  "Default parameters"}

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static uint32_t Drv_Flash_GetCheckSum(uint8_t *uiData, uint16_t ui16Size);

/*******************************************************************************
 * Variables
 ******************************************************************************/
app_conf_flash_t g_sAppConfRun = CFG_DEFAULT;

drv_flash_t g_sFlashDrv;
drv_flash_ctrl_t g_sM1FlashCtrl = {1U, TRUE, FALSE, kFlashCfg_NoOperation, kFlashStatus_Ready};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Initialize flash driver and check available parameter configurations in the flash memory
 *
 * @param void  No input parameter
 *
 * @return bool_t Return true if the initialization was successful. Else return false.
 */
bool_t Drv_Flash_Init(void)
{
  uint32_t i, destAdrss, firstConfig;

  g_sFlashDrv.ui32FlashErr = FLASH_ERR_NO;

  /* Clean up flash, Cache driver structure */
  memset(&(g_sFlashDrv.s_flashDriver), 0, sizeof(flash_config_t));

  /* Initialize flash driver */
  if (FLASH_Init(&(g_sFlashDrv.s_flashDriver)) != kStatus_Success)
  {
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_INIT;

    return FALSE;
  }

  /* Clear available configurations, data in the flash memory must be checked first */
  g_sFlashDrv.ui32ConfigAvailable = 0U;
  g_sFlashDrv.bCfgInFlash = FALSE;

  /* Adjust the clock cycle for accessing the flash memory according to the system clock. */
  g_sFlashDrv.systemFreqHz = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  CLOCK_SetFLASHAccessCyclesForFreq(g_sFlashDrv.systemFreqHz);

  /** Check if the flash page is blank before reading to avoid hardfault. See AN12949 chapter 3.4 for the reference.
    * If the flash is erased (clean), FLASH_VerifyErase return kStatus_Success -> flash must be programmed first!
    */
  for(i = 1U; i <= NUMBER_USER_CONFIGURATIONS; i++)
  {
    /* Create address for specific page. */
    destAdrss = g_sFlashDrv.s_flashDriver.PFlashBlockBase + (g_sFlashDrv.s_flashDriver.PFlashTotalSize - (i * g_sFlashDrv.s_flashDriver.PFlashPageSize));

    /* Check flash memory content. */
    g_sFlashDrv.status = FLASH_VerifyErase(&g_sFlashDrv.s_flashDriver, destAdrss, g_sFlashDrv.s_flashDriver.PFlashPageSize);
    if (g_sFlashDrv.status == kStatus_Success)
    {
      /* Configuration "i" is not available. */
      g_sFlashDrv.ui32ConfigAvailable &= ~(1 << (i-1));
    }
    else
    {
      /* Note: Data validation not available yet. */

      if(g_sFlashDrv.bCfgInFlash == FALSE)
      {
        /* Some data are stored in the flash, can be read. */
        g_sFlashDrv.bCfgInFlash = TRUE;

        /* Store index of the first available configuration, which will be used. */
        firstConfig = i;
      }

      /* Configuration "i" is available. */
      g_sFlashDrv.ui32ConfigAvailable |= (1 << (i - 1));
    }
  }

    /* Copy strig of user description */
    for(uint16_t i = 0U; i < CFG_USER_DESCRIPTION_LENGTH; i++)
    {
      g_sFlashDrv.cDescription[i] = g_sAppConfRun.cDescription[i];
    }

#if ENABLE_CFG_UPDATE_INIT
  /* Load the first available configuration */
  if(g_sFlashDrv.bCfgInFlash == TRUE)
  {
    if(Drv_Flash_Cfg_Read(firstConfig) != TRUE)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_INIT;
      return FALSE;
    }

    Drv_ParamsSwap(kFlashCfg_Read);
  }
#endif

  return TRUE;
}

/*!
 * @brief Read the configuration from the flash to the temporary structure
 *
 * @param uint32_t  Index of the desired configuration
 *
 * @return bool_t Return true if the read was successful. Else return false.
 */
bool_t Drv_Flash_Cfg_Read(uint32_t ui32Index)
{
  uint32_t destAdrss, srcAddr;

  g_sFlashDrv.ui32FlashErr = FLASH_ERR_NO;

  if(ui32Index > NUMBER_USER_CONFIGURATIONS)
  {
    /* Required index exceed the maximum number of possible configurations */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_EXCEEDED;

    return FALSE;
  }

  /* Check configuration availability with required index */
  if(g_sFlashDrv.ui32ConfigAvailable & (1 << (ui32Index - 1)))
  {
    uint32_t ui32Sum;

    /* Calculate checksum only for useful data, so omit the reserved space */
    uint16_t ui16Size = sizeof(g_sAppConfRun) - (2U*FLASH_CONF_RESERVED_SPACE);

    /* User configuration is available in flash, load this configuration to RAM. */
    srcAddr = g_sFlashDrv.s_flashDriver.PFlashBlockBase + (g_sFlashDrv.s_flashDriver.PFlashTotalSize - (ui32Index * g_sFlashDrv.s_flashDriver.PFlashPageSize));

    /* Set pointer to destination structure */
    destAdrss = (uint32_t)&g_sAppConfRun;

    /* Copy parameters from flash to run-time (RAM) structure. */
    g_sFlashDrv.status = FLASH_Read(&g_sFlashDrv.s_flashDriver, srcAddr, (uint8_t *)destAdrss, g_sFlashDrv.s_flashDriver.PFlashPageSize);

    if (g_sFlashDrv.status != kStatus_Success)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_READ;
      return FALSE;
    }

    /* Calculate checksum of read parameters */
    ui32Sum = Drv_Flash_GetCheckSum((uint8_t *)destAdrss, ui16Size);

    if(ui32Sum != g_sAppConfRun.ui32CheckSum)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_CHECKSUM;
      return FALSE;
    }
  }
  else
  {
    /* Configuration is not available, no data to erase */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_NA;

    return FALSE;
  }

  return TRUE;
}

/*!
 * @brief Clear selected configuration in the flash memory
 *
 * @param uint32_t  Index of the configuration to be deleted
 *
 * @return bool_t Return true if the clear was successful. Else return false.
 */
bool_t Drv_Flash_Cfg_Clear(uint32_t ui32Index)
{
  uint32_t destAdrss;

  g_sFlashDrv.ui32FlashErr = FLASH_ERR_NO;

  if(ui32Index > NUMBER_USER_CONFIGURATIONS)
  {
    /* Required index exceed the maximum number of possible configurations */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_EXCEEDED;

    return FALSE;
  }

  if(g_sFlashDrv.ui32ConfigAvailable & (1 << (ui32Index - 1)))
  {
    /* Configuration is available, will be erased in the flash */
    destAdrss = g_sFlashDrv.s_flashDriver.PFlashBlockBase + (g_sFlashDrv.s_flashDriver.PFlashTotalSize - (ui32Index * g_sFlashDrv.s_flashDriver.PFlashPageSize));

    /* Erase selected memory area */
    g_sFlashDrv.status = FLASH_Erase(&g_sFlashDrv.s_flashDriver, destAdrss, g_sFlashDrv.s_flashDriver.PFlashPageSize, kFLASH_ApiEraseKey);
    if (g_sFlashDrv.status != kStatus_Success)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CLEAR;
      return FALSE;
    }

    /* Verify if the given flash range is successfully erased. */
    g_sFlashDrv.status = FLASH_VerifyErase(&g_sFlashDrv.s_flashDriver, destAdrss, g_sFlashDrv.s_flashDriver.PFlashPageSize);
    if (g_sFlashDrv.status != kStatus_Success)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CLEAR;
      return FALSE;
    }

    /* Configuration is no longer available */
    g_sFlashDrv.ui32ConfigAvailable &= ~(1 << (ui32Index - 1));

    /* Check if any configuration is available */
    g_sFlashDrv.bCfgInFlash = (bool_t)(1U && g_sFlashDrv.ui32ConfigAvailable);
  }
  else
  {
    /* Configuration is not available, no data to erase */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_NA;
    return FALSE;
  }

  return TRUE;
}

/*!
 * @brief Write the configuration to the flash from the temporary structure
 *
 * @param uint32_t  Index of the desired configuration
 *
 * @return bool_t Return true if the read was successful. Else return false.
 */
bool_t Drv_Flash_Cfg_Write(uint32_t ui32Index)
{
  uint32_t srcAddr, destAdrss;

  g_sFlashDrv.ui32FlashErr = FLASH_ERR_NO;

  if(ui32Index > NUMBER_USER_CONFIGURATIONS)
  {
    /* Required index exceed the maximum number of possible configurations */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_EXCEEDED;

    return FALSE;
  }

  if(g_sFlashDrv.ui32ConfigAvailable & (1 << (ui32Index - 1)))
  {
    /* Configuration already available, must be firstly erased in the flash */
    g_sFlashDrv.ui32FlashErr |= FLASH_ERR_CONF_EXIST;

    return FALSE;
  }
  else
  {
    /* Calculate checksum only for useful data, so omit the reserved space */
    uint16_t ui16Size = sizeof(g_sAppConfRun) - (2U*FLASH_CONF_RESERVED_SPACE);

    srcAddr = (uint32_t)&g_sAppConfRun;

    /* Calculate checksum of parameters to be written to the flash, save checksum */
    g_sAppConfRun.ui32CheckSum = Drv_Flash_GetCheckSum((uint8_t *)srcAddr, ui16Size);

    /* Calculate flash destination address */
    destAdrss = g_sFlashDrv.s_flashDriver.PFlashBlockBase + (g_sFlashDrv.s_flashDriver.PFlashTotalSize - (ui32Index * g_sFlashDrv.s_flashDriver.PFlashPageSize));

    /* Start programming specified flash region */
    g_sFlashDrv.status = FLASH_Program(&g_sFlashDrv.s_flashDriver, destAdrss, (uint8_t *)srcAddr, sizeof(g_sAppConfRun));
    if (g_sFlashDrv.status != kStatus_Success)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_WRITE;
      return FALSE;
    }

    /* Verify if the given flash region is successfully programmed with given data */
    g_sFlashDrv.status = FLASH_VerifyProgram(&g_sFlashDrv.s_flashDriver, destAdrss, sizeof(g_sAppConfRun), (const uint8_t *)srcAddr, &g_sFlashDrv.failedAddress,
                                 &g_sFlashDrv.failedData);
    if (g_sFlashDrv.status != kStatus_Success)
    {
      g_sFlashDrv.ui32FlashErr |= FLASH_ERR_WRITE;
      return FALSE;
    }

    g_sFlashDrv.bCfgInFlash = TRUE;

    /* New configuration is available */
    g_sFlashDrv.ui32ConfigAvailable |= (1 << (ui32Index - 1));
  }

  return TRUE;
}

void Drv_ParamsSwap(drv_flash_cfg_t eOperation)
{
  if(eOperation == kFlashCfg_Read)
  {
    /* Load data: Flash memory -> g_sAppConfRun -> run-time configuration */
    g_fltM1currentScale = g_sAppConfRun.fltM1currentScale;
    g_fltM1DCBvoltageScale = g_sAppConfRun.fltM1DCBvoltageScale;
    g_fltM1speedScale = g_sAppConfRun.fltM1speedScale;
    g_fltM1voltageScale = g_sAppConfRun.fltM1voltageScale;
    g_fltM1speedAngularScale = g_sAppConfRun.fltM1speedAngularScale;
    g_sM1Drive.sFaultThresholds.fltUDcBusOver = g_sAppConfRun.sFaultThresholds.fltUDcBusOver;
    g_sM1Drive.sFaultThresholds.fltUDcBusUnder = g_sAppConfRun.sFaultThresholds.fltUDcBusUnder;
    g_sM1Drive.sFaultThresholds.fltUDcBusTrip = g_sAppConfRun.sFaultThresholds.fltUDcBusTrip;
    g_sM1Drive.sFaultThresholds.fltSpeedOver = g_sAppConfRun.sFaultThresholds.fltSpeedOver;
    g_sM1Drive.sFaultThresholds.fltSpeedMin = g_sAppConfRun.sFaultThresholds.fltSpeedMin;
    g_sM1Drive.sFaultThresholds.fltSpeedNom = g_sAppConfRun.sFaultThresholds.fltSpeedNom;
    g_sM1Drive.sFaultThresholds.fltUqBemf = g_sAppConfRun.sFaultThresholds.fltUqBemf;
    g_sM1Drive.sFaultThresholds.ui16BlockedPerNum = g_sAppConfRun.sFaultThresholds.ui16BlockedPerNum;
    g_sM1Drive.ui16TimeCalibration = g_sAppConfRun.ui16TimeCalibration;
    g_sM1Drive.ui16TimeFaultRelease = g_sAppConfRun.ui16TimeFaultRelease;
    g_sM1Drive.ui16TimeFullSpeedFreeWheel = g_sAppConfRun.ui16TimeFullSpeedFreeWheel;
    g_sM1Drive.sScalarCtrl.fltUqMin = g_sAppConfRun.fltUqMin;
    g_sM1Drive.sScalarCtrl.fltFreqMax = g_sAppConfRun.fltFreqMax;
    g_sM1Drive.sScalarCtrl.fltVHzGain = g_sAppConfRun.fltVHzGain;
    g_sM1Drive.sScalarCtrl.sFreqIntegrator.a32Gain = g_sAppConfRun.a32ScalarGain;
    g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampUp = g_sAppConfRun.fltScalarRampUp;
    g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampDown = g_sAppConfRun.fltRampDown;
    g_sM1Drive.sAlignment.fltUdReq = g_sAppConfRun.fltUdReq;
    g_sM1Drive.sAlignment.ui16Time = g_sAppConfRun.ui16Time;
    g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampUp = g_sAppConfRun.fltStartupRampUp;
    g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampDown = g_sAppConfRun.fltStartupRampDown;
    g_sM1Drive.sStartUp.fltCurrentStartup = g_sAppConfRun.fltCurrentStartup;
    g_sM1Drive.sStartUp.fltSpeedCatchUp = g_sAppConfRun.fltSpeedCatchUp;
    g_sM1Drive.sStartUp.f16CoeffMerging = g_sAppConfRun.f16CoeffMerging;
    g_sM1Drive.sStartUp.sSpeedIntegrator.a32Gain = g_sAppConfRun.a32StartupGain;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain = g_sAppConfRun.fltIdPiPGain;
    g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain = g_sAppConfRun.fltIdPiIGain;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain = g_sAppConfRun.fltIqPiPGain;
    g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain = g_sAppConfRun.fltIqPiIGain;
    g_sM1Drive.sFocPMSM.fltDutyCycleLimit = g_sAppConfRun.fltDutyCycleLimit;
    g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp = g_sAppConfRun.fltSpeedRampUp;
    g_sM1Drive.sSpeed.sSpeedRampParams.fltRampDown = g_sAppConfRun.fltSpeedRampDown;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltUpperLim = g_sAppConfRun.fltUpperLim;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltLowerLim = g_sAppConfRun.fltLowerLim;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain = g_sAppConfRun.fltSpeedPiPGain;
    g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain = g_sAppConfRun.fltSpeedPiIGain;
    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB0 = g_sAppConfRun.fltSpeedFilterB0;
    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB1 = g_sAppConfRun.fltSpeedFilterB1;
    g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltA1 = g_sAppConfRun.fltSpeedFilterA1;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB0 = g_sAppConfRun.fltUDcBusFilterB0;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB1= g_sAppConfRun.fltUDcBusFilterB1;
    g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltA1 = g_sAppConfRun.fltUDcBusFilterA1;
    g_sM1Drive.sFocPMSM.sBemfObsrv.fltIGain = g_sAppConfRun.fltBemfObsrvIGain;
    g_sM1Drive.sFocPMSM.sBemfObsrv.fltUGain = g_sAppConfRun.fltBemfObsrvUGain;
    g_sM1Drive.sFocPMSM.sBemfObsrv.fltEGain= g_sAppConfRun.fltBemfObsrvEGain;
    g_sM1Drive.sFocPMSM.sBemfObsrv.fltWIGain= g_sAppConfRun.fltBemfObsrvWIGain;
    g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltPGain = g_sAppConfRun.fltBemfObstrCtrlPGain;
    g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltIGain = g_sAppConfRun.fltBemfObstrCtrlIGain;
    g_sM1Drive.sFocPMSM.sTo.fltPGain = g_sAppConfRun.fltToPGain;
    g_sM1Drive.sFocPMSM.sTo.fltIGain = g_sAppConfRun.fltToIGain;
    g_sM1Drive.sFocPMSM.sTo.fltThGain = g_sAppConfRun.fltToThGain;
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB0 = g_sAppConfRun.fltSpeedElEstB0;
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB1 = g_sAppConfRun.fltSpeedElEstB1;
    g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltA1 = g_sAppConfRun.fltSpeedElEstA1;
    g_sM1Drive.sPosition.f16PositionPGain = g_sAppConfRun.f16PositionPGain;
    g_sM1Enc.ui16Pp = g_sAppConfRun.ui16EncPp;
    g_sM1Enc.ui16PulseNumber = g_sAppConfRun.ui16EncPulseNumber;
    g_sM1Enc.bDirection = g_sAppConfRun.bEncDirection;
    g_sM1Enc.fltSpdEncMin = g_sAppConfRun.fltSpdEncMin;
    g_sM1Enc.a32PosMeGain = g_sAppConfRun.a32PosMeGain;
    g_sM1Enc.sTo.fltPGain = g_sAppConfRun.fltEncPGain;
    g_sM1Enc.sTo.fltIGain = g_sAppConfRun.fltEncIGain;
    g_sM1Enc.sTo.fltThGain = g_sAppConfRun.fltEncThGain;

    /* Call update direction and pulses to correct propage changes */
    M1_MCDRV_QD_SET_DIRECTION(&g_sM1Enc);
    M1_MCDRV_QD_SET_PULSES(&g_sM1Enc);

    /* Copy strig of user description */
    for(uint16_t i = 0U; i < CFG_USER_DESCRIPTION_LENGTH; i++)
    {
      g_sFlashDrv.cDescription[i] = g_sAppConfRun.cDescription[i];
    }
  }

  if(eOperation == kFlashCfg_Write)
  {
    /* Store data: Run-time configuration -> g_sAppConfRun -> Flash memory */
    g_sAppConfRun.fltM1currentScale = g_fltM1currentScale;
    g_sAppConfRun.fltM1DCBvoltageScale = g_fltM1DCBvoltageScale;
    g_sAppConfRun.fltM1speedScale = g_fltM1speedScale;
    g_sAppConfRun.fltM1voltageScale = g_fltM1voltageScale;
    g_sAppConfRun.fltM1speedAngularScale = g_fltM1speedAngularScale;
    g_sAppConfRun.sFaultThresholds.fltUDcBusOver = g_sM1Drive.sFaultThresholds.fltUDcBusOver;
    g_sAppConfRun.sFaultThresholds.fltUDcBusUnder = g_sM1Drive.sFaultThresholds.fltUDcBusUnder;
    g_sAppConfRun.sFaultThresholds.fltUDcBusTrip = g_sM1Drive.sFaultThresholds.fltUDcBusTrip;
    g_sAppConfRun.sFaultThresholds.fltSpeedOver = g_sM1Drive.sFaultThresholds.fltSpeedOver;
    g_sAppConfRun.sFaultThresholds.fltSpeedMin = g_sM1Drive.sFaultThresholds.fltSpeedMin;
    g_sAppConfRun.sFaultThresholds.fltSpeedNom = g_sM1Drive.sFaultThresholds.fltSpeedNom;
    g_sAppConfRun.sFaultThresholds.fltUqBemf = g_sM1Drive.sFaultThresholds.fltUqBemf;
    g_sAppConfRun.sFaultThresholds.ui16BlockedPerNum = g_sM1Drive.sFaultThresholds.ui16BlockedPerNum;
    g_sAppConfRun.ui16TimeCalibration = g_sM1Drive.ui16TimeCalibration;
    g_sAppConfRun.ui16TimeFaultRelease = g_sM1Drive.ui16TimeFaultRelease;
    g_sAppConfRun.ui16TimeFullSpeedFreeWheel = g_sM1Drive.ui16TimeFullSpeedFreeWheel;
    g_sAppConfRun.fltUqMin = g_sM1Drive.sScalarCtrl.fltUqMin;
    g_sAppConfRun.fltFreqMax = g_sM1Drive.sScalarCtrl.fltFreqMax;
    g_sAppConfRun.fltVHzGain = g_sM1Drive.sScalarCtrl.fltVHzGain;
    g_sAppConfRun.a32ScalarGain = g_sM1Drive.sScalarCtrl.sFreqIntegrator.a32Gain;
    g_sAppConfRun.fltScalarRampUp = g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampUp;
    g_sAppConfRun.fltRampDown = g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampDown;
    g_sAppConfRun.fltUdReq = g_sM1Drive.sAlignment.fltUdReq;
    g_sAppConfRun.ui16Time = g_sM1Drive.sAlignment.ui16Time;
    g_sAppConfRun.fltStartupRampUp = g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampUp;
    g_sAppConfRun.fltStartupRampDown = g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampDown;
    g_sAppConfRun.fltCurrentStartup = g_sM1Drive.sStartUp.fltCurrentStartup;
    g_sAppConfRun.fltSpeedCatchUp = g_sM1Drive.sStartUp.fltSpeedCatchUp;
    g_sAppConfRun.f16CoeffMerging = g_sM1Drive.sStartUp.f16CoeffMerging;
    g_sAppConfRun.a32StartupGain = g_sM1Drive.sStartUp.sSpeedIntegrator.a32Gain;
    g_sAppConfRun.fltIdPiPGain = g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain;
    g_sAppConfRun.fltIdPiIGain = g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain;
    g_sAppConfRun.fltIqPiPGain = g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain;
    g_sAppConfRun.fltIqPiIGain = g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain;
    g_sAppConfRun.fltDutyCycleLimit = g_sM1Drive.sFocPMSM.fltDutyCycleLimit;
    g_sAppConfRun.fltSpeedRampUp = g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp;
    g_sAppConfRun.fltSpeedRampDown = g_sM1Drive.sSpeed.sSpeedRampParams.fltRampDown;
    g_sAppConfRun.fltUpperLim = g_sM1Drive.sSpeed.sSpeedPiParams.fltUpperLim;
    g_sAppConfRun.fltLowerLim = g_sM1Drive.sSpeed.sSpeedPiParams.fltLowerLim;
    g_sAppConfRun.fltSpeedPiPGain = g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain;
    g_sAppConfRun.fltSpeedPiIGain = g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain;
    g_sAppConfRun.fltSpeedFilterB0 = g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB0;
    g_sAppConfRun.fltSpeedFilterB1 = g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB1;
    g_sAppConfRun.fltSpeedFilterA1 = g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltA1;
    g_sAppConfRun.fltUDcBusFilterB0 = g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB0;
    g_sAppConfRun.fltUDcBusFilterB1 = g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB1;
    g_sAppConfRun.fltUDcBusFilterA1 = g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltA1;
    g_sAppConfRun.fltBemfObsrvIGain = g_sM1Drive.sFocPMSM.sBemfObsrv.fltIGain;
    g_sAppConfRun.fltBemfObsrvUGain = g_sM1Drive.sFocPMSM.sBemfObsrv.fltUGain;
    g_sAppConfRun.fltBemfObsrvEGain = g_sM1Drive.sFocPMSM.sBemfObsrv.fltEGain;
    g_sAppConfRun.fltBemfObsrvWIGain = g_sM1Drive.sFocPMSM.sBemfObsrv.fltWIGain;
    g_sAppConfRun.fltBemfObstrCtrlPGain = g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltPGain;
    g_sAppConfRun.fltBemfObstrCtrlIGain = g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltIGain;
    g_sAppConfRun.fltToPGain = g_sM1Drive.sFocPMSM.sTo.fltPGain;
    g_sAppConfRun.fltToIGain = g_sM1Drive.sFocPMSM.sTo.fltIGain;
    g_sAppConfRun.fltToThGain = g_sM1Drive.sFocPMSM.sTo.fltThGain;
    g_sAppConfRun.fltSpeedElEstB0 = g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB0;
    g_sAppConfRun.fltSpeedElEstB1 = g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB1;
    g_sAppConfRun.fltSpeedElEstA1 = g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltA1;
    g_sAppConfRun.f16PositionPGain = g_sM1Drive.sPosition.f16PositionPGain;
    g_sAppConfRun.ui16EncPp = g_sM1Enc.ui16Pp;
    g_sAppConfRun.ui16EncPulseNumber = g_sM1Enc.ui16PulseNumber;
    g_sAppConfRun.bEncDirection = g_sM1Enc.bDirection;
    g_sAppConfRun.fltSpdEncMin = g_sM1Enc.fltSpdEncMin;
    g_sAppConfRun.a32PosMeGain = g_sM1Enc.a32PosMeGain;
    g_sAppConfRun.fltEncPGain = g_sM1Enc.sTo.fltPGain;
    g_sAppConfRun.fltEncIGain = g_sM1Enc.sTo.fltIGain;
    g_sAppConfRun.fltEncThGain = g_sM1Enc.sTo.fltThGain;

    /* Copy strig of user description */
    for(uint16_t i = 0U; i < CFG_USER_DESCRIPTION_LENGTH; i++)
    {
      g_sAppConfRun.cDescription[i] = g_sFlashDrv.cDescription[i];
    }
  }
}

void Drv_Flash_Cfg_Background(void)
{
  static uint32_t ui32Cnt = 0U;         /* Counter for change status after while */

  if(g_sM1FlashCtrl.bStart == TRUE)
  {
    g_sFlashDrv.ui32FlashErr = FLASH_ERR_NO;

    ui32Cnt = 0U;       /* Reset counter */

    switch (g_sM1FlashCtrl.eOperation)
    {
      case kFlashCfg_Read:
        /* Request to load parameters from the flash memory to the runtime parameters (RAM) */
        g_sM1FlashCtrl.bRetVal = Drv_Flash_Cfg_Read(g_sM1FlashCtrl.ui32CfgIndex);
        if(g_sM1FlashCtrl.bRetVal == TRUE)
        {
          /* Configuration was successfully read, runtime parameters can be updated */
          Drv_ParamsSwap(kFlashCfg_Read);
          g_sM1FlashCtrl.eStatus = kFlashStatus_Success;
        }
        else
        {
          g_sM1FlashCtrl.eStatus = kFlashStatus_Fail;
        }

        /* Clear request */
        g_sM1FlashCtrl.eOperation &= ~(kFlashCfg_Read);
        break;

      case kFlashCfg_Write:
        /* Request to store runtime parameters from the RAM to the flash memory */
        Drv_ParamsSwap(kFlashCfg_Write);
        g_sM1FlashCtrl.bRetVal = Drv_Flash_Cfg_Write(g_sM1FlashCtrl.ui32CfgIndex);

        if(g_sM1FlashCtrl.bRetVal == TRUE)
        {
          g_sM1FlashCtrl.eStatus = kFlashStatus_Success;
        }
        else
        {
          g_sM1FlashCtrl.eStatus = kFlashStatus_Fail;
        }

        /* Clear request */
        g_sM1FlashCtrl.eOperation &= ~(kFlashCfg_Write);
        break;

      case kFlashCfg_Clear:
        /* Clear selected configuration in the flash memory */
        g_sM1FlashCtrl.bRetVal = Drv_Flash_Cfg_Clear(g_sM1FlashCtrl.ui32CfgIndex);

        if(g_sM1FlashCtrl.bRetVal == TRUE)
        {
          g_sM1FlashCtrl.eStatus = kFlashStatus_Success;
        }
        else
        {
          g_sM1FlashCtrl.eStatus = kFlashStatus_Fail;
        }

        /* Clear request */
        g_sM1FlashCtrl.eOperation &= ~(kFlashCfg_Clear);
        break;

      default:
        break;
    }

    g_sM1FlashCtrl.bStart = FALSE;
  }

  /* Change status after 5 seconds (if background function called every 1ms) */
  if(ui32Cnt == 5000U)
  {
    g_sM1FlashCtrl.eStatus = kFlashStatus_Ready;
  }

  ui32Cnt++;
}

static uint32_t Drv_Flash_GetCheckSum(uint8_t *uiData, uint16_t ui16Size)
{
    uint16_t ui16Id  = 0;           /* Byte array index. */
    uint32_t ui32Sum = 0U; /* Initiate sum with the seed. */

    /* Sum all bytes in array (except last four bytes reserved for chesksum). */
    for(ui16Id = 0; ui16Id < (ui16Size - 4U); ui16Id++)
    {
        ui32Sum += uiData[ui16Id];
    }

    return ui32Sum;
}
