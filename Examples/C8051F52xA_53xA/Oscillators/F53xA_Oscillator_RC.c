//-----------------------------------------------------------------------------
// F53xA_Oscillator_RC.c
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates how to configure the oscillator for use with an
// external RC Oscillator.
//
//
// How To Test:
//
// 1) Ensure that a capacitor (C29 or C34) and resistor (R16 or R21) are
//    installed.
// 2) Specify the target frequency in the constant <RC_FREQUENCY>.
// 3) Download code to an 'F53xA device (either device A or device B on the
//    C8051F530A target board).
// 4) Measure the frequency output on P0.0.
//
// Choosing Component Values:
//
//       frequency (Hz) = 1.23*10^3 / (R * C )
//
//       where R is in Ohms
//             C is in Farads
//
//       Note: Add 1pF stray capacitance to C.
//
// For a resistor value of 249K, and a capacitor value of 47pF, the
// expected RC_FREQUENCY is 102.9 kHz. The actual measured frequency
// on the target board was 104 kHz.
//
// Target:         C8051F52xA/F53xA (C8051F530A TB)
// Tool chain:     Raisonance / Keil
// Command Line:   None
//
// Release 1.2 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.1
//    -Updated for 'F530A TB (TP)
//    -30 JAN 2008
//
// Release 1.0
//    -Initial Revision (DS)
//    -18 June 2007
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
#define RC_FREQUENCY   100800          //  Target RC Frequency in Hz

// XFCN Setting Macro
#if  (RC_FREQUENCY <= 20000)
   #define XFCN 0
#elif(RC_FREQUENCY <= 58000)
   #define XFCN 1
#elif(RC_FREQUENCY <= 155000)
   #define XFCN 2
#elif(RC_FREQUENCY <= 415000)
   #define XFCN 3
#elif(RC_FREQUENCY <= 1100000)
   #define XFCN 4
#elif(RC_FREQUENCY <= 3100000)
   #define XFCN 5
#elif(RC_FREQUENCY <= 8200000)
   #define XFCN 6
#elif(RC_FREQUENCY <= 25000000)
   #define XFCN 7
#else
   #error "RC Frequency must be less than or equal to 25 MHz"
#endif

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
// in RC mode.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Delay counter

   OSCXCN = (0x40 | XFCN);             // Start external oscillator with
                                       // the appropriate XFCN setting
                                       // based on RC frequency

   for (i=0; i < 256; i++);            // Wait for RC osc. to start

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
// The oscillator pin should be configured as an analog input when using the
// external oscillator in RC mode.
//
// P0.0   digital    push-pull     /SYSCLK
//
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