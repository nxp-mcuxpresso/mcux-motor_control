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

#ifndef _MCDRV_FLASH_LPC_H_
#define _MCDRV_FLASH_LPC_H_

#include "sm_common.h"
#include "fsl_device_registers.h"
#include "fsl_flash.h"
#include "fsl_flash_ffr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Flash error values */
#define FLASH_ERR_NO                    (0U)
#define FLASH_ERR_INIT                  (1UL << 0U)     /* Error during initialization */
#define FLASH_ERR_READ                  (1UL << 1U)     /* Error during read configuration from the flash */
#define FLASH_ERR_WRITE                 (1UL << 2U)     /* Error during write configuration to the flash */
#define FLASH_ERR_CLEAR                 (1UL << 3U)     /* Error during clear configuration in the flash */
#define FLASH_ERR_CONF_EXCEEDED         (1UL << 4U)     /* Index of required configuration exceed NUMBER_USER_CONFIGURATIONS number */
#define FLASH_ERR_CONF_NA               (1UL << 5U)     /* Required configuration is not available in the flash */
#define FLASH_ERR_CONF_EXIST            (1UL << 6U)     /* Required configuration is already available in the flash */
#define FLASH_ERR_CONF_CHECKSUM         (1UL << 7U)     /* Checksum error */
   
/* N-pages from the end of flash memory reserved for our structures. 1 page reserved for 1 configuration */
#define NUMBER_USER_CONFIGURATIONS 5U
   
/* Load first available configuration from the flash after initialization */
#define ENABLE_CFG_UPDATE_INIT (true)
   
/* Length of the user description */
#define CFG_USER_DESCRIPTION_LENGTH 20U
   
/* Reserved memory space to fit structure into the one page */
#define FLASH_CONF_RESERVED_SPACE 114U

/* Update configurations in the flash enums */
typedef enum _drv_flash_cfg_t
{
  kFlashCfg_NoOperation = 0x0,  
  kFlashCfg_Read = 0x1,         /* Load data to the run-time configuration */
  kFlashCfg_Write = 0x2,        /* Store data from the run-time configuration */
  kFlashCfg_Clear = 0x3,        /* Clear configuration in the flash memory */
} drv_flash_cfg_t;

/* Update configurations status enums */
typedef enum _drv_flash_status_t
{
  kFlashStatus_Ready = 0x0,             /* Reade for next operation */
  kFlashStatus_Success = 0x1,           /* Operation successfull */
  kFlashStatus_Fail = 0x2,              /* Operation failed */
} drv_flash_status_t;

/* Configuration structure for online flash update. Be aware that sizeof(app_conf_flash_t) is equal to size of 1 flash page. */
typedef struct _app_conf_flash_t
{
  float_t fltM1currentScale;            /* g_fltM1currentScale */
  float_t fltM1DCBvoltageScale;         /* g_fltM1DCBvoltageScale */
  float_t fltM1speedScale;              /* g_fltM1speedScale */
  float_t fltM1voltageScale;            /* g_fltM1voltageScale */
  float_t fltM1speedAngularScale;       /* g_fltM1speedAngularScale */
  mcdef_fault_thresholds_t sFaultThresholds; /* g_sM1Drive.sFaultThresholds */
  uint16_t ui16TimeCalibration;         /* g_sM1Drive.ui16TimeCalibration */
  uint16_t ui16TimeFaultRelease;        /* g_sM1Drive.ui16TimeFaultRelease */
  uint16_t ui16TimeFullSpeedFreeWheel;  /* g_sM1Drive.ui16TimeFullSpeedFreeWheel */
  float_t fltUqMin;                     /* g_sM1Drive.sScalarCtrl.fltUqMin */
  float_t fltFreqMax;                   /* g_sM1Drive.sScalarCtrl.fltFreqMax */
  float_t fltVHzGain;                   /* g_sM1Drive.sScalarCtrl.fltVHzGain */
  acc32_t a32ScalarGain;                /* g_sM1Drive.sScalarCtrl.sFreqIntegrator.a32Gain */
  float_t fltScalarRampUp;              /* g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampUp */
  float_t fltRampDown;                  /* g_sM1Drive.sScalarCtrl.sFreqRampParams.fltRampDown */
  float_t fltUdReq;                     /* g_sM1Drive.sAlignment.fltUdReq */
  uint16_t ui16Time;                    /* g_sM1Drive.sAlignment.ui16Time */
  float_t fltStartupRampUp;             /* g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampUp */
  float_t fltStartupRampDown;           /* g_sM1Drive.sStartUp.sSpeedRampOpenLoopParams.fltRampDown */
  float_t fltCurrentStartup;            /* g_sM1Drive.sStartUp.fltCurrentStartup */
  float_t fltSpeedCatchUp;              /* g_sM1Drive.sStartUp.fltSpeedCatchUp */
  frac16_t f16CoeffMerging;             /* g_sM1Drive.sStartUp.f16CoeffMerging */
  acc32_t a32StartupGain;               /* g_sM1Drive.sStartUp.sSpeedIntegrator.a32Gain */
  float_t fltIdPiPGain;                 /* g_sM1Drive.sFocPMSM.sIdPiParams.fltPGain */
  float_t fltIdPiIGain;                 /* g_sM1Drive.sFocPMSM.sIdPiParams.fltIGain */
  float_t fltIqPiPGain;                 /* g_sM1Drive.sFocPMSM.sIqPiParams.fltPGain */
  float_t fltIqPiIGain;                 /* g_sM1Drive.sFocPMSM.sIqPiParams.fltIGain */
  float_t fltDutyCycleLimit;            /* g_sM1Drive.sFocPMSM.fltDutyCycleLimit */
  float_t fltSpeedRampUp;               /* g_sM1Drive.sSpeed.sSpeedRampParams.fltRampUp */
  float_t fltSpeedRampDown;             /* g_sM1Drive.sSpeed.sSpeedRampParams.fltRampDown */
  float_t fltUpperLim;                  /* g_sM1Drive.sSpeed.sSpeedPiParams.fltUpperLim */
  float_t fltLowerLim;                  /* g_sM1Drive.sSpeed.sSpeedPiParams.fltLowerLim */
  float_t fltSpeedPiPGain;              /* g_sM1Drive.sSpeed.sSpeedPiParams.fltPGain */
  float_t fltSpeedPiIGain;              /* g_sM1Drive.sSpeed.sSpeedPiParams.fltIGain */
  float_t fltSpeedFilterB0;             /* g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB0 */
  float_t fltSpeedFilterB1;             /* g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltB1 */
  float_t fltSpeedFilterA1;             /* g_sM1Drive.sSpeed.sSpeedFilter.sFltCoeff.fltA1 */
  float_t fltUDcBusFilterB0;            /* g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB0 */
  float_t fltUDcBusFilterB1;            /* g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltB1 */
  float_t fltUDcBusFilterA1;            /* g_sM1Drive.sFocPMSM.sUDcBusFilter.sFltCoeff.fltA1 */
  float_t fltBemfObsrvIGain;            /* g_sM1Drive.sFocPMSM.sBemfObsrv.fltIGain */
  float_t fltBemfObsrvUGain;            /* g_sM1Drive.sFocPMSM.sBemfObsrv.fltUGain */
  float_t fltBemfObsrvEGain;            /* g_sM1Drive.sFocPMSM.sBemfObsrv.fltEGain */
  float_t fltBemfObsrvWIGain;           /* g_sM1Drive.sFocPMSM.sBemfObsrv.fltWIGain */
  float_t fltBemfObstrCtrlPGain;        /* g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltPGain */
  float_t fltBemfObstrCtrlIGain;        /* g_sM1Drive.sFocPMSM.sBemfObsrv.sCtrl.fltIGain */
  float_t fltToPGain;                   /* g_sM1Drive.sFocPMSM.sTo.fltPGain */
  float_t fltToIGain;                   /* g_sM1Drive.sFocPMSM.sTo.fltIGain */
  float_t fltToThGain;                  /* g_sM1Drive.sFocPMSM.sTo.fltThGain */
  float_t fltSpeedElEstB0;              /* g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB0 */
  float_t fltSpeedElEstB1;              /* g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltB1 */
  float_t fltSpeedElEstA1;              /* g_sM1Drive.sFocPMSM.sSpeedElEstFilt.sFltCoeff.fltA1 */
  frac16_t f16PositionPGain;            /* g_sM1Drive.sPosition.f16PositionPGain */
  uint16_t ui16EncPp;                   /* g_sM1Enc.ui16Pp */
  uint16_t ui16EncPulseNumber;          /* g_sM1Enc.ui16PulseNumber */
  bool_t bEncDirection;                 /* g_sM1Enc.bDirection */
  float_t fltSpdEncMin;                 /* g_sM1Enc.fltSpdEncMin */
  acc32_t a32PosMeGain;                 /* g_sM1Enc.a32PosMeGain */
  float_t fltEncPGain;                  /* g_sM1Enc.sTo.fltPGain */
  float_t fltEncIGain;                  /* g_sM1Enc.sTo.fltIGain */
  float_t fltEncThGain;                 /* g_sM1Enc.sTo.fltThGain */
  char cDescription[CFG_USER_DESCRIPTION_LENGTH];       /* User's description */
  uint32_t ui32CheckSum;                                /* Checksum value */
  uint16_t ui16Reserved[FLASH_CONF_RESERVED_SPACE];     /* Reserved place to reach page-aligned structure */
} app_conf_flash_t;

/* Control flash update struct */
typedef struct _drv_flash_ctrl_t
{
  uint32_t ui32CfgIndex;
  bool_t bRetVal;
  bool_t bStart;
  drv_flash_cfg_t eOperation;
  drv_flash_status_t eStatus;
} drv_flash_ctrl_t;

/* Flash update driver struct */
typedef struct _drv_flash_t
{
  flash_config_t s_flashDriver;
  uint32_t ui32FlashErr;        /* Flash error status variable */
  status_t status;
  uint32_t failedAddress;
  uint32_t failedData;
  uint32_t systemFreqHz;
  uint32_t ui32ConfigAvailable; /* Available configurations in the flash */
  char cDescription[CFG_USER_DESCRIPTION_LENGTH];
  bool_t bCfgInFlash;
} drv_flash_t;

/******************************************************************************
 * Global variable definitions
 ******************************************************************************/
extern drv_flash_ctrl_t g_sM1FlashCtrl;
extern drv_flash_t g_sFlashDrv;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

bool_t Drv_Flash_Init(void);
bool_t Drv_Flash_Cfg_Read(uint32_t ui32Index);
bool_t Drv_Flash_Cfg_Clear(uint32_t ui32Index);
bool_t Drv_Flash_Cfg_Write(uint32_t ui32Index);
void Drv_ParamsSwap(drv_flash_cfg_t eOperation);
void Drv_Flash_Cfg_Background(void);

#ifdef __cplusplus
}
#endif

#endif /* _MCDRV_FLASH_LPC_H_ */
