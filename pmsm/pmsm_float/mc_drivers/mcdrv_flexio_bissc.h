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

#ifndef _MCDRV_FLEXIO_BISSC_H_
#define _MCDRV_FLEXIO_BISSC_H_

#include "fsl_common.h"
#include "fsl_flexio.h"
#include "mlib.h"
#include "mlib_types.h"
#include "amclib_FP.h"
//#include "amclib_types.h"

/*!
 * @addtogroup flexio_bissc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLEXIO_BISSC_CRC_LEN    6u

/*! @brief Define FlexIO BISSC access structure typedef. */
typedef struct _flexio_bissc_type
{
    FLEXIO_Type *flexioBase;    /*!< FlexIO base pointer. */
    
    float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
    frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
    
    uint8_t RxdPinIndex;        /*!< Pin select for data input. */
    uint8_t ClockPinIndex;      /*!< Pin select for clock. */
    uint8_t shifterStartIndex;  /*!< Shifter index used in FlexIO BISSC. */
    uint8_t timerIndex;         /*!< Timer index used in FlexIO BISSC. */

    void (* callback)(void *bisscType);

    uint16_t mt_len;
    uint16_t st_len;

    uint32_t data[2];
    uint32_t mt;
    uint32_t st;
    uint32_t mt_offset;
    uint32_t st_offset;
    
    uint8_t error_bit;
    uint8_t warn_bit;
    bool crc_match;
    
    AMCLIB_TRACK_OBSRV_T_FLT sTo; /* tracking observer structure */
    
    //acc32_t a32PositionActualBiss;
        
    acc32_t a32PosMeReal;         /* real position (revolution counter + mechanical position) */
    acc32_t a32PosErr;            /* position error to tracking observer  */
    //acc32_t a32PosMeGain;         /* encoder pulses to mechanical position scale gain */
    float_t fltSpdMeEst;          /* estimated speed calculated using tracking observer */
    frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
    frac16_t f16PosMeEst;         /* estimated position calculated using tracking observer */
//    frac16_t f16PosMeOffset;      /* offset of mechanical position calculated after align*/
    
    uint16_t ui16Pp;              /* number of motor pole pairs */
    
    
} FLEXIO_BISSC_Type;

/*! @brief Define FlexIO BISSC configuration structure. */
typedef struct _flexio_bissc_config
{
    bool enable;                              /*!< Enable/disable FlexIO BISSC after configuration. */
    bool enableInDoze;                        /*!< Enable/disable FlexIO operation in doze mode. */
    bool enableInDebug;                       /*!< Enable/disable FlexIO operation in debug mode. */
    bool enableFastAccess;                    /*!< Enable/disable fast access to FlexIO registers,
                                              fast access requires the FlexIO clock to be at least
                                              twice the frequency of the bus clock. */
    uint32_t baudRate_Bps;                    /*!< Baud rate in Bps. */
} flexio_bissc_config_t;


typedef struct _bissc_type
{
  
    float_t *pfltSpdMeEst;        /* pointer to measured mechanical speed  */
    frac16_t *pf16PosElEst;       /* pointer to measured electrical position */
  
    uint32_t mt;
    uint32_t st;
    uint32_t mt_offset;
    uint32_t st_offset;
    
    AMCLIB_TRACK_OBSRV_T_FLT sTo; /* tracking observer structure */
     
    acc32_t a32PosMeReal;         /* real position (revolution counter + mechanical position) */
    acc32_t a32PosErr;            /* position error to tracking observer  */
    float_t fltSpdMeEst;          /* estimated speed calculated using tracking observer */
    frac16_t f16PosMe;            /* mechanical position calculated using encoder edges */
    frac16_t f16PosMeEst;         /* estimated position calculated using tracking observer */
    
    uint16_t ui16Pp;              /* number of motor pole pairs */
    
} BISSC_Type;





/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

RAM_FUNC_LIB
void FLEXIO_BISSC_Init(FLEXIO_BISSC_Type *base, flexio_bissc_config_t *config, uint32_t srcClock_Hz);

RAM_FUNC_LIB
void FLEXIO_BISSC_EnableRxInterrupt(FLEXIO_BISSC_Type *base, bool enable);

RAM_FUNC_LIB
int FLEXIO_BISSC_Delay_Compensate(FLEXIO_BISSC_Type * base);

RAM_FUNC_LIB
void FLEXIO_BISSC_Data_Proc(FLEXIO_BISSC_Type * base);

RAM_FUNC_LIB
int FLEXIO_BISSC_ReadBlocking(FLEXIO_BISSC_Type *base);

RAM_FUNC_LIB
int FLEXIO_BISSC_ReadNonBlocking(FLEXIO_BISSC_Type *base);

RAM_FUNC_LIB
status_t FLEXIO_BISSC_RegisterIRQ(FLEXIO_BISSC_Type *base);

RAM_FUNC_LIB
void MCDRV_BissClear(FLEXIO_BISSC_Type *base);

RAM_FUNC_LIB
void MCDRV_BissSetOffset(FLEXIO_BISSC_Type *base);




RAM_FUNC_LIB
void MCDRV_BissCClear(BISSC_Type *base);

RAM_FUNC_LIB
void MCDRV_BissCSetOffset(BISSC_Type *base);

RAM_FUNC_LIB
void BISSC_Data_Proc(BISSC_Type * base);






#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /* MCDRV_FLEXIO_BISSC_H_*/
