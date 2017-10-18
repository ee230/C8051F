#ifndef  LCD_PUT_STRING_H
#define  LCD_PUT_STRING_H
//=============================================================================
// lcdPutString.h
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
#define  AUTO_CLEAR_DISPLAY
//-----------------------------------------------------------------------------
// Public function prototypes (API)
//-----------------------------------------------------------------------------
void lcdPutString (char * string);
//-----------------------------------------------------------------------------
// lcdPutChar() prototype
//
// The lcdPutChar() function is specific to the particular LCD display and
// the pin connections. A prototype is included here to keep this file
// hardware generic. The function lcdPutChar() must be defined elsewhere
// or a linker error will occur.
//-----------------------------------------------------------------------------
void lcdPutChar (char c);
//=============================================================================
// end LCD_PUTS_H
//=============================================================================
#endif //LCD_PUT_STRING_H