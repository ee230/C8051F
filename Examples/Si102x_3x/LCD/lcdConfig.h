#ifndef  LCD_CONFIG_H
#define  LCD_CONFIG_H
//================================================================================================
// lcdConfig.h
//================================================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//    This file is LCD configuration header file for the Si102x/3x.
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
//    Si102x LCD example
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 29 SEPTEMBER 2011
//
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