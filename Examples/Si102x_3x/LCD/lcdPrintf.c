//=============================================================================
// lcdPrintf.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
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
//-----------------------------------------------------------------------------
// Includes
//
// This file is written using 8051 compiler independent code.
//-----------------------------------------------------------------------------
#include <stdio.h>                     // This is needed for vsprintf()
#include <stdarg.h>                    // This is needed for the va_list
#include <compiler_defs.h>
#include "lcdPrintf.h"
#include "lcdPutString.h"
//-----------------------------------------------------------------------------
// Function Name
//          lcdPrintf (const char *fmt, ...)
// Parameters   :const char *fmt, variable argument list
// Return Value : none
//
// Description:
//
// This implementation of lcdPrintf may be used to format and send a character
// string to the LCD. This function uses the standard ANSI vsprintf function
// to print to the string. This vsprintf function is ideal for this function
// because it allows a variable argument list to be passed as a parameter.
// The sprintf() function would not work here with a variable argument list.
//
// The vsprintf() function is normally quite large. So inclusion of this
// function is optional.
//
//-----------------------------------------------------------------------------
void lcdPrintf (const char *fmt, ...)
{
   va_list ap;                         // ap is type va_list from stdarg.h

   static SEGMENT_VARIABLE (string[MAX_LCD_STRING], char, SEG_XDATA);

   va_start (ap, fmt);                 //start variable arg list

   vsprintf(string, fmt, ap);

   lcdPutString(string);

   va_end(ap);
}
//=============================================================================
// End of File
//=============================================================================
