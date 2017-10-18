//-----------------------------------------------------------------------------
// T60x_Oscillator_Capacitor.c
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
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
// 1) Ensure that a capacitor (C20) is installed.
// 2) Specify the target frequency in the constant <C_FREQUENCY>.
// 3) Download code to an T600 target board.
// 4) Measure the frequency output on P0.1.
//
// Choosing Component Values:
//
//       frequency (Hz) is proportional to [ KF(XFCN) / (C * VDD) ]
//
//       where KF(XFCN) is a factor based on XFCN.
//
// The actual frequency is best determined by measurement. In this example,
// a 33pF capacitor was used with VDD = 3.3V. Component values are best
// chosen by trial and error.
//
// Target:         C8051T60x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1 / 03 NOV 2008 (BD)
//    -Rerouted output to P0.1 so that 'T606 can use code too.
// Release 1.0 / 29 JUL 2008 (ADT)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <c8051T600_defs.h>           // SFR declarations

//-----------------------------------------------------------------------------
// External Oscillator C Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the target frequency
//
#define C_FREQUENCY   12000000         // Target C Frequency in Hz

// XFCN Setting Macro
#define XFCN             5             // XFCN/KF setting.

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

   REG0CN |= 0x01;                     // OTP Power Controller Enable

   PORT_Init();                        // Initialize Port I/O
   OSCILLATOR_Init ();                 // Initialize Oscillator

   while (1);                          // loop forever

}  // end of main()

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

   int i;                              // delay counter

   OSCXCN = (0x50 | XFCN);             // start external oscillator with
                                       // the appropriate XFCN setting
                                       // in C Mode

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // VDD Monitor reset


   OSCICN = 0x08;                      // Switch to external clock

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
// P0.1   digital    push-pull     /SYSCLK
// P0.3   analog     don't care     EXTCLK
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
    // Oscillator Pins
   P0MDIN &= ~0x08;                    // P0.3 is analog
   XBR0 = 0x09;                        // P0.3, P0.0 skipped in the Crossbar

   // Buffered System Clock Output
   P0MDOUT |= 0x02;                    // P0.1 is push-pull

   // Crossbar Initialization
   XBR1    = 0x08;                     // Route /SYSCLK to first available pin
   XBR2    = 0x40;                     // Enable Crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------