//-----------------------------------------------------------------------------
// F70x_LowPower_IdleMode.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This code puts c8051f700 in idle mode for a low power consuming state
//  to verify the current consumption numbers given for idle mode at
//  a internal oscillator speed of 1.25 MHz.
//
// How to test:
//
// 1) To get your board into its lowest power configuration, first read the
//       steps in the c8051f700 User's Guide under the Frequently Asked 
//       Questions section. 
//
// 2) Make sure no shorting block is connected to J17.
//
// 3) Connect with the IDE, compile code, download code, and disconnect the
//     USB Debug Adapter from the board.
//
// 4) Verify that the current consumption from the external power supply
//     is about 330 uA.
//
// Target:        C8051F12x
// Tool chain:    Keil C51 8/ Keil EVAL C51
// Command Line:  None
//
// Revision History:
//
// Release 1.0 / 10 JUL 2008 (PD)
//    -Initial Revision
//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051f700_defs.h>                // SFR declarations

//----------------------------------------------------------------------------
// main() Routine
//----------------------------------------------------------------------------

main(void)
{
   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

   SFRPAGE = CONFIG_PAGE;
   OSCICN |= 0x80;                     // Enable the precision internal osc.
      
   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // leave VDD Monitor enabled.

   CLKSEL = 0x70;                      // Select precision internal osc. 
                                       // divided by 128 as the system clock
      
   PCON |= 0x01;                       // Idle mode on
}