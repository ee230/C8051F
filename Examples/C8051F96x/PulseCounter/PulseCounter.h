#ifndef  PULSE_COUNTER_H
#define  PULSE_COUNTER_H
//================================================================================================
// PulseCounter.h
//================================================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
// This file is SmaRTClock header file for the C8051F96x.
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   version 4.18
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
//
// Project Name:
//    F96x  Example code
//
//
// This software must be used in accordance with the End Users License Agreement.
//
//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H
#include "compiler_defs.h"
#endif
//-----------------------------------------------------------------------------
// Defines for PC0_Init parameters
//-----------------------------------------------------------------------------
#define PC0_SINGLE_MODE                0x40
#define PC0_DUAL_MODE                  0x80
#define PC0_QUADRATURE_MODE            0xC0

#define PC0_SAMPLING_RATE_250US        0x00
#define PC0_SAMPLING_RATE_500US        0x10
#define PC0_SAMPLING_RATE_1MS          0x20
#define PC0_SAMPLING_RATE_2MS          0x30
//-----------------------------------------------------------------------------
// Optional feature bits
//-----------------------------------------------------------------------------
#define PC0_STOP_CNT_FLUTTER           0x04
#define PC0_DUALSTCH                   0x02
//-----------------------------------------------------------------------------
// Define PC0 options Here !
//-----------------------------------------------------------------------------
#define PC0_OPTIONS                    0
//-----------------------------------------------------------------------------
// Defines for PC0PCF sfr
//-----------------------------------------------------------------------------
#define PC0_CAL_START                  0x84
#define PC0_CAL_BUSY                   0x80
#define PC0_CAL_PASS                   0x40
#define PC0_CAL_PC1                    0x20
#define PC0_CAL_PC0                    0x00
//-----------------------------------------------------------------------------
// Defines for PC0TH sfr
//-----------------------------------------------------------------------------
#define PC0_THRESHI_56                 0x00
#define PC0_THRESHI_60                 0x40
#define PC0_THRESHI_52                 0x80
#define PC0_THRESHI_48                 0xC0

#define PC0_THRESLO_40                 0x00
#define PC0_THRESLO_44                 0x10
#define PC0_THRESLO_36                 0x20
#define PC0_THRESLO_32                 0x30

#define PC0TH_READ_VALID_MASK          0x01
//-----------------------------------------------------------------------------
// Macros for calculating debounce time.
// n is time in us, m is sample rate 0,1,2,3
//-----------------------------------------------------------------------------
#define PC0DCH_VALUE(n,m)  ((((U32)(n))/250)>>m)
#define PC0DCL_VALUE(n,m)  (((((U32)(n))/250)>>m)-1)
//-----------------------------------------------------------------------------
// Defines for PC0INT0
//-----------------------------------------------------------------------------
#define  PC0_CMP1F      0x80
#define  PC0_CMP1EN     0x40
#define  PC0_CMP0F      0x20
#define  PC0_CMP0EN     0x10
#define  PC0_OVRF       0x08
#define  PC0_OVREN      0x04
#define  PC0_DIRCHGF    0x02
#define  PC0_DIRCHGEN   0x01
//-----------------------------------------------------------------------------
// Defines for PC0INT1
//-----------------------------------------------------------------------------
#define  PC0_FLTRSTF    0x80
#define  PC0_FLTSTREN   0x40
#define  PC0_FLTRSTPF   0x20
#define  PC0_FLTRSTEN   0x10
#define  PC0_ERRORF     0x08
#define  PC0_ERROREN    0x04
#define  PC0_TRANSF     0x02
#define  PC0_TRANSEN    0x01
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void  PC0_Init (U8 mode, U8 sampleRate, U8 flutterOptions);
U8    PC0_ReadPC0STAT(void);
U8    PC0_ReadPC0HIST(void);
U8    PC0_ReadPC0INT0(void);
U8    PC0_ReadPC0INT1(void);
U32   PC0_ReadCounter0 (void);
U32   PC0_ReadCounter1 (void);
void  PC0_WriteComparator0 (U32 count);
void  PC0_WriteComparator1 (U32 count);
U32   PC0_ReadComparator0 (void);
U32   PC0_ReadComparator1 (void);
U8    PC0_SetDebounceTimeHigh (U32 time);
U8    PC0_SetDebounceTimeLow (U32 time);
U32   PC0_SetPullUpCurrent (U32 current);
U32   PC0_ReadPullUpCurrent (void);
U8    PC0_AutoCalibrate (U8 channel);
//=============================================================================
#endif  // PULSE_COUNTER_H
