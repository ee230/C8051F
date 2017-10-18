#ifndef  PP_PHY_CONST_H
#define  PP_PHY_CONST_H
//=============================================================================
// ppPhyConst.h
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   Version 3.6
//
// Tool Chains:
//    Keil        9
//    SDCC        3
//    Raisonance  7
//
// Project Name:
//    PreProcessorPHY
//
// Version 2.0 - See release notes in ppPhy.c
//    - 08NOV2011
//
// This software must be used in accordance with the End User License
// Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Qualified includes
//-----------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
#ifndef  PHY_DEFS_H
#include "ppPhy_defs.h"
#endif
//-----------------------------------------------------------------------------
// Divide with Rounding
//-----------------------------------------------------------------------------
#define DIVR(a,b)    (((a)+(b)/2)/((b)))
//-----------------------------------------------------------------------------
// Macros for calculating TRX_DEVIATION
//-----------------------------------------------------------------------------
#ifndef TRX_DEVIATION
#if (TRX_DATA_RATE>1250)
#define TRX_DEVIATION (TRX_DATA_RATE/2)
#else
#define TRX_DEVIATION (625L)
#endif
#endif
//-----------------------------------------------------------------------------
// Macros for calculating RX_BANDWIDTH based on XTAL_TOLERANCE
//-----------------------------------------------------------------------------
#ifndef RX_BANDWIDTH
#define IDEAL_BANDWIDTH (TRX_DATA_RATE + (2*TRX_DEVIATION))
#define FREQUENCY_ERROR (TRX_FREQUENCY/1000*XTAL_TOLERANCE*4/1000)
#if (IDEAL_BANDWIDTH < 620000L)
#if(FREQUENCY_ERROR>IDEAL_BANDWIDTH)
#define RX_BANDWIDTH FREQUENCY_ERROR
#else
#define RX_BANDWIDTH IDEAL_BANDWIDTH
#endif
#else
#define RX_BANDWIDTH 620000L
#endif
#endif
//-----------------------------------------------------------------------------
// Include Table after defining TRX_DEVIATION and RX_BANDWIDTH
//-----------------------------------------------------------------------------
#include "ppPhyIfFilterBandwidth.h"
//-----------------------------------------------------------------------------
// Frequency Macros
// Macros using parameters use CALC_ prefix
// Explicit casting to U32
//-----------------------------------------------------------------------------
#define CALC_HIGH_FREQ_BAND(f)               ((((U32)f)-480000000L)/20000000L)
#define CALC_LOW_FREQ_BAND(f)                ((((U32)f)-240000000L)/10000000L)
#define CALC_CARRIER_FREQUENCY_HIGH(f)       ((DIVR((((U32)f)-480000000L-20000000L*CALC_HIGH_FREQ_BAND(f)),625L))<<1)
#define CALC_CARRIER_FREQUENCY_LOW(f)        ((DIVR((((U32)f)-240000000L-10000000L*CALC_LOW_FREQ_BAND(f)),625L))<<2)
#define CALC_NOMINAL_CARRIER_FREQEUNCY(f)    ((((U32)f)>480000000)?CALC_CARRIER_FREQUENCY_HIGH(f):CALC_CARRIER_FREQUENCY_LOW(f))
//-----------------------------------------------------------------------------
// TRX_CHANNEL_SPACING Macros
//-----------------------------------------------------------------------------
#define CALC_FREQUENCY_HOPPING_STEP_SIZE(f)  (DIVR(((U32)f),10000L))
//-----------------------------------------------------------------------------
// TX_DEVIATION Macros
//-----------------------------------------------------------------------------
#define CALC_TX_DEVIATION(f)  (DIVR(f,625L))
//-----------------------------------------------------------------------------
// TX_DATA_RATE Macros
//-----------------------------------------------------------------------------
#define CALC_TX_DATA_RATE_LOW(r)    (DIVR((((U32)r)<<15),15625L))  // r*2^21/10^6 = r*2^15/15625
#define CALC_TX_DATA_RATE_HIGH(r)   (DIVR((((U32)r)<<10),15625L))  // r*2^16/10^6 = r*2^10/15625
#define CALC_TX_DATA_RATE(r)        ((((U32)r)<30000L)?CALC_TX_DATA_RATE_LOW(r):CALC_TX_DATA_RATE_HIGH(r))
//-----------------------------------------------------------------------------
// RX Modem Macros
//-----------------------------------------------------------------------------
#define  RXOSR          (DIVR(((4000000L*(RX_DWN3_BYP*2+1))>>RX_NDEC_EXP),(TRX_DATA_RATE)))
#define  NCOFF          (DIVR(((DIVR(((TRX_DATA_RATE)<<13),15625))<<(RX_NDEC_EXP+2)),(RX_DWN3_BYP*2+1)))
#define  CRGAIN         (DIVR(((DIVR(((TRX_DATA_RATE)<<13),RXOSR))<<3),TRX_DEVIATION)+1)
//-----------------------------------------------------------------------------
// Simple math macros using parameters
// Explicit casting to U32
//-----------------------------------------------------------------------------
#define BYTE0(a)     (((U32)a)&0x000000FFL)
#define BYTE1(a)     ((((U32)a)&0x0000FF00L)>>8)
#define BYTE2(a)     ((((U32)a)&0x00FF0000L)>>16)
#define BYTE3(a)     ((((U32)a)&0xFF000000L)>>24)
//-----------------------------------------------------------------------------
// Macros to assemble actual bytes to write to registers
//-----------------------------------------------------------------------------
// Modulation Mode Registers
//-----------------------------------------------------------------------------
#if     (TRX_DATA_RATE<30000L)
#define MODULATION_MODE_CONTROL_1_VALUE    0x2C
#else   // (TRX_DATA_RATE>=30000L)
#define MODULATION_MODE_CONTROL_1_VALUE    0x0C
#endif

#if     (CALC_TX_DEVIATION(TRX_DEVIATION)>255)
#define MODULATION_MODE_CONTROL_2_VALUE    (MODULATION_MODE|0x24)
#else
#define MODULATION_MODE_CONTROL_2_VALUE    (MODULATION_MODE|0x20)
#endif

//-----------------------------------------------------------------------------
//TRX Frequency Registers
//-----------------------------------------------------------------------------
#define TX_DEVIATION_0_VALUE               BYTE0(CALC_TX_DEVIATION(TRX_DEVIATION))

#if  (TRX_FREQUENCY>480000000L)
#define FREQUENCY_BAND_SELECT_VALUE        (CALC_HIGH_FREQ_BAND(TRX_FREQUENCY)|0x60)
#else
#define FREQUENCY_BAND_SELECT_VALUE        (CALC_LOW_FREQ_BAND(TRX_FREQUENCY)|0x40)
#endif

#define NOMINAL_CARRIER_FREQUENCY_0_VALUE  BYTE0(CALC_NOMINAL_CARRIER_FREQEUNCY(TRX_FREQUENCY))
#define NOMINAL_CARRIER_FREQUENCY_1_VALUE  BYTE1(CALC_NOMINAL_CARRIER_FREQEUNCY(TRX_FREQUENCY))

#ifdef   TRX_CHANNEL_SPACING
#define FREQUENCY_HOPPING_STEP_SIZE_VALUE  CALC_FREQUENCY_HOPPING_STEP_SIZE(TRX_CHANNEL_SPACING)
#else
#define FREQUENCY_HOPPING_STEP_SIZE_VALUE    0
#endif
//-----------------------------------------------------------------------------
//TX Modem Settings
//-----------------------------------------------------------------------------
#define  TX_POWER_VALUE                    0x1F  // +20 dB, LNA switch enabled
#define  TX_DATA_RATE_1_VALUE              BYTE1(CALC_TX_DATA_RATE(TRX_DATA_RATE))
#define  TX_DATA_RATE_0_VALUE              BYTE0(CALC_TX_DATA_RATE(TRX_DATA_RATE))
//-----------------------------------------------------------------------------
// RX Modem Settings
//-----------------------------------------------------------------------------
#define  CLOCK_RECOVERY_OVERSAMPLING_RATIO_0_VALUE BYTE0(RXOSR)
#define  CLOCK_RECOVERY_OFFSET_2_VALUE             ((BYTE1(RXOSR)<<5)|BYTE2(NCOFF))
#define  CLOCK_RECOVERY_OFFSET_1_VALUE             BYTE1(NCOFF)
#define  CLOCK_RECOVERY_OFFSET_0_VALUE             BYTE0(NCOFF)

#if(CRGAIN < 0x7FF)
#define  CLOCK_RECOVERY_TIMING_LOOP_GAIN_1_VALUE   (BYTE1(CRGAIN))
#define  CLOCK_RECOVERY_TIMING_LOOP_GAIN_0_VALUE   BYTE0(CRGAIN)
#else
#define  CLOCK_RECOVERY_TIMING_LOOP_GAIN_1_VALUE   0x07
#define  CLOCK_RECOVERY_TIMING_LOOP_GAIN_0_VALUE   0xFF
#endif

#define  AFC_LOOP_GEARSHIFT_OVERRIDE_VALUE         0x40  // AFC Enabled setting
#define  AGC_OVERRIDE_1_VALUE                      0x60
#define  CLOCK_RECOVERY_GEARSHIFT_OVERRIDE_VALUE   0x03



#if(TRX_DATA_RATE < 100000)
#define CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE_VALUE 0x80
#elif(TRX_DATA_RATE < 200000)
#define CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE_VALUE 0xC0
#else
#define CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE_VALUE 0xED
#endif

#if(TRX_DATA_RATE < 200000)
#define AFC_TIMING_CONTROL_VALUE                   0x0A
#else
#define AFC_TIMING_CONTROL_VALUE                   0x02
#endif

#define AFC_BANDWIDTH (RX_BANDWIDTH*9/10)

#if  (TRX_FREQUENCY>480000000L)
#define ACF_LIMIT  (AFC_BANDWIDTH/2500)
#else
#define ACF_LIMIT  (AFC_BANDWIDTH/1250)
#endif

#if  (ACF_LIMIT<80)
#define    ACF_LIMIT_VALUE    ACF_LIMIT
#else
#define    ACF_LIMIT_VALUE    80
#endif
//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------
#define NUMBER_OF_REGISTERS                       28
//-----------------------------------------------------------------------------
// Public code constants
//-----------------------------------------------------------------------------
extern SEGMENT_VARIABLE (ppPhyRegisters[NUMBER_OF_REGISTERS], U8, code);
extern SEGMENT_VARIABLE (ppPhySettings[NUMBER_OF_REGISTERS], U8, code);
//=============================================================================
//=============================================================================
#endif //PP_PHY_CONST_H