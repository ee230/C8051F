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
// This file is strict ANSI-C. So compiler defs is not used.
//-----------------------------------------------------------------------------
#include  "lcdPutString.h"
//-----------------------------------------------------------------------------
// lcdPutString()
//
// This function will output a string to the lcd display. The LCD normally needs
// to be cleared before each string. So the AUTO_CLEAR_DISPLAY feature has been
// added to the put string function. This may be disabled by commenting out the
// build option in the header file.
//
// A generic character pointer is used. The string may be located in xdata or
// in code space.
//
// Note that the putString function depends on a terminating nul character.
// If the string does not have a terminating null character this function
// will continue to put characters until it hits a null byte in memory.
// This is normal behavdior for the putString function.
//
//-----------------------------------------------------------------------------
void lcdPutString (char * string)
{
#ifdef  AUTO_CLEAR_DISPLAY
   lcdPutChar('\r');                   // carraige return resets index
   lcdPutChar('\n');                   // new line clears display
#endif

   while(*string)
   {
      lcdPutChar(*string);
      string++;
   }
}
//=============================================================================
// end of file
//=============================================================================
