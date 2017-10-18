#ifndef  LCD_PRINTF_H
#define  LCD_PRINTF_H
//=============================================================================
// lcdPrintf.h
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
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
//    Si102x/3x LCD example
//
// Release 1.0
//    - Initial Revision (MRF)
//    - 29 SEPTEMBER 2011
//
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
// #ifndef COMPILER_DEFS_H
// #include <compiler_defs.h>
// #endif
//-----------------------------------------------------------------------------
// This defines the size of the string container used for the lcd display.
//
// The string must be large enough to include all special characters and the
// terminating nul character.
//
// Worst case for VIM878 is 25 characters.
//
// "'1.'2.'3.'4.'5.'6.'7.'8."
//
// The max is set to 32 to give some margin.
// 
//-----------------------------------------------------------------------------
#define  MAX_LCD_STRING    (32)
//-----------------------------------------------------------------------------
// Public function prototypes (API)
//-----------------------------------------------------------------------------
void lcdPrintf (const char *fmt, ...);
//=============================================================================
// end LCD_PRINTF_H
//=============================================================================
#endif //LCD_PRINTF_H