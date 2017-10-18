#ifndef  LCD_CONFIG_H
#define  LCD_CONFIG_H
//================================================================================================
// lcdConfig.h
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
// LCD Contrast modes
//-----------------------------------------------------------------------------
#define LCD_BYPASS_MODE             1
#define LCD_MIN_CONTRAST_MODE       2
#define LCD_CONSTANT_CONTRAST_MODE  3
#define LCD_AUTO_BYPASS_MODE        4
//-----------------------------------------------------------------------------
// RTC frequency
//-----------------------------------------------------------------------------
#define RTC_XTAL_FREQ               32786
#define RTC_LFO_FREQ                16000
//-----------------------------------------------------------------------------
// Define RTC mode here
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define RTC_LFO_MODE
//#define RTC_XTAL_MODE
//-----------------------------------------------------------------------------
// Public function prototypes (API)
//-----------------------------------------------------------------------------
void LCD0_Config(U8 muxMode, U8 segments, U8 contrastMode, U16 contrast, U8 refresh, U8 toggle);
void LCD0_ConfigClear(void);
void LCD0_ConfigMuxMode(U8 muxMode);
void LCD0_ConfigSegments(U8 segments);
void LCD0_ConfigContrastMode(U8 contrastMode);
void LCD0_SetContrast(U16 contrast);
void LCD0_ConfigRefresh(U8 refresh, U8 muxMode);
void LCD0_ConfigToggle(U8 toggle, U8 refresh,  U8 muxMode);
void LCD0_ConfigEnable(void);
//=============================================================================
// end LCD_CONFIG_H
//=============================================================================
#endif //LCD_CONFIG_H