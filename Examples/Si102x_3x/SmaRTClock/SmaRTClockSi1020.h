#ifndef  SMARTCLOCKSI1020_H
#define  SMARTCLOCKSI1020_H
//================================================================================================
// SmaRTClockSi1020.h
//================================================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//    This file is SmaRTClock header file for the Si102x/3x.
//
// Target:
//    Si102x/3x
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    Si102x/3x Example code
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 28 SEPTEMBER 2011
//
// This software must be used in accordance with the End Users License Agreement.
//
//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//-----------------------------------------------------------------------------
// Indirect RTC Register Addresses (ifndef to not overload hader file)
//-----------------------------------------------------------------------------
#ifndef CAPTURE0
#define CAPTURE0  0x00                 // RTC address of CAPTURE0 register
#define CAPTURE1  0x01                 // RTC address of CAPTURE1 register
#define CAPTURE2  0x02                 // RTC address of CAPTURE2 register
#define CAPTURE3  0x03                 // RTC address of CAPTURE3 register
#define RTC0CN    0x04                 // RTC address of RTC0CN register
#define RTC0XCN   0x05                 // RTC address of RTC0XCN register
#define RTC0XCF   0x06                 // RTC address of RTC0XCF register
#define RTC0CF    0x07                 // RTC address of RTC0PIN register
#define ALARM0B0  0x08                 // RTC address of ALARM0 Byte 0
#define ALARM0B1  0x09                 // RTC address of ALARM0 Byte 1
#define ALARM0B2  0x0A                 // RTC address of ALARM0 Byte 2
#define ALARM0B3  0x0B                 // RTC address of ALARM0 Byte 3
#define ALARM1B0  0x0C                 // RTC address of ALARM1 Byte 0
#define ALARM1B1  0x0D                 // RTC address of ALARM1 Byte 1
#define ALARM1B2  0x0E                 // RTC address of ALARM1 Byte 2
#define ALARM1B3  0x0F                 // RTC address of ALARM1 Byte 3
#define ALARM2B0  0x10                 // RTC address of ALARM2 Byte 0
#define ALARM2B1  0x11                 // RTC address of ALARM2 Byte 1
#define ALARM2B2  0x12                 // RTC address of ALARM2 Byte 2
#define ALARM2B3  0x13                 // RTC address of ALARM2 Byte 3
#endif
//-----------------------------------------------------------------------------
// SmaRTClock Bit Definitions
//-----------------------------------------------------------------------------
#define RTC0CAP   0x01
#define RTC0SET   0x02
#define RTC0FAST  0x04
#define RTC0TR    0x10
#define OSCFAIL   0x20
#define MCLKEN    0x40
#define RTC0EN    0x80
//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define TWELVE_PF       0x0C
#define CAP_AUTO_STEP   0x80
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void  RTC_Init (void);
U8    RTC_Read (U8 reg);
void  RTC_Write (U8 reg, U8 value);
U8    RTC_SetTimer(U32 time);
U32   RTC_CaptureTimer (void);
void  RTC_WriteAlarm(U8 alarm, U32 time);
U32   RTC_ReadAlarm (U8 alarm);
//=============================================================================
#endif  // SMARTCLOCKSI1020_H
