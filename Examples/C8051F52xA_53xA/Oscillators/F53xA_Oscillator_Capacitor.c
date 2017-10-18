//-----------------------------------------------------------------------------
// F53xA_Oscillator_Capacitor.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the oscillator for use with an
// external capacitor.
//
//
// How To Test:
//
// 1) Ensure that a capacitor (C29 or C34) is installed.
// 2) Specify the target frequency in the constant <C_FREQUENCY>.
// 3) Download code to an 'F53xA target board (either device A or device B).
// 4) Measure the frequency output on P0.0.
//
// Choosing Component Values:
//
//       frequency (Hz) is proportional to [ KF(XFCN) / (C * VDD) ]
//
//       where KF(XFCN) is a factor based on XFCN.
//
// The actual frequency is best determined by measurement. In this example,
// a 47pF capacitor was used with VDD = 2.1V. Component values are best
// chosen by trial and error.  It is a good idea to include at least 1pF
// of stray capacitances in frequency calculations.
//
// The expected frequencies using a 47pF capacitor plus 1pF of stray
// capacitances and 2.1V VDD are:
//
//       frequency    |   XFCN    |   KF
//      ------------------------------------
//         8.63  kHz  |     0     |   0.87
//         25.8  kHz  |     1     |   2.6
//         76.4  kHz  |     2     |   7.7
//          218  kHz  |     3     |   22
//          645  kHz  |     4     |   65
//         1.79  MHz  |     5     |   180
//         6.59  MHz  |     6     |   664
//         15.8  MHz  |     7     |   1590
//
// In this example, the desired frequency is 218kHz, and with a 48pF
// capacitance, XFCN is chosen to be 3, giving a KF of 22.
//
// Target:         C8051F52xA/F53xA (C8051F530A TB)
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -Updated for 'F53xA TB (TP)
//    -30 JAN 2008
//
// Release 1.0
//    -Initial Revision (DS)
//    -18 JUNE 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F520A_defs.h"

//-----------------------------------------------------------------------------
// External Oscillator RC Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the target frequency
//
#define C_FREQUENCY   218000           // Target RC Frequency in Hz

// XFCN Setting Macro
#define XFCN             3             // XFCN/KF setting.

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   while (1);                          // Loop forever

}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the external oscillator
// in C mode.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Delay counter

   OSCXCN = (0x50 | XFCN);             // Start external oscillator with
                                       // the appropriate XFCN setting
                                       // in C Mode

   for (i=0; i < 256; i++);            // Wait for C osc. to start

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // VDD Monitor reset

   CLKSEL = 0x01;                      // Select external oscillator as system
                                       // clock source

   OSCICN = 0x00;                      // Disable the internal oscillator.

}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// The oscillator pins should be configured as analog inputs when using the
// external oscillator in crystal mode.
//
// P0.0   digital    push-pull     /SYSCLK
// P1.0   analog     don't care     XTAL2
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   // Oscillator Pins
   P1MDIN &= ~0x01;                    // P1.0 is analog
   P1SKIP |= 0x01;                     // P1.0 skipped in the Crossbar

   // Buffered System Clock Output
   P0MDOUT |= 0x01;                    // P0.0 is push-pull

   // Crossbar Initialization
   XBR0    = 0x08;                     // Route /SYSCLK to first available pin
   XBR1    = 0x40;                     // Enable Crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------