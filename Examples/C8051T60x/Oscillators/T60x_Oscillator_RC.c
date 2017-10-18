//-----------------------------------------------------------------------------
// T60x_Oscillator_RC.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
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
// 1) Ensure that a capacitor (C20) and resistor (R18) are installed.
// 2) Specify the target frequency in the constant <RC_FREQUENCY>.
// 3) Download code to an 'T600 target board.
// 4) Measure the frequency output on P0.1.
//
// Choosing Component Values:
//
//       frequency (Hz) = 1 / (1.05 * R * C )
//
//       where R is in Ohms
//             C is in Farads
//
//       Note: Add 6pF stray capacitance to C.
//
// For a resistor value of 300K, and a capacitor value of 33pF, the
// expected RC_FREQUENCY is 81.4 kHz. The actual measured frequency
// on the target board was 80.7 kHz.
//
//
// Target:         C8051T60x
// Tool chain:     Keil C51 8.0 / Keil EVAL C51
// Command Line:   None
//
// Release 1.1 / 03 NOV 2008 (BD)
//    -Rerouted output to P0.1 so that 'T606 can use code too.
// Release 1.0 / 30 JUL 2008 (ADT)
//    -Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <c8051T600_defs.h>            // SFR declarations

//-----------------------------------------------------------------------------
// External Oscillator RC Frequency Selection
//-----------------------------------------------------------------------------
//
// Please initialize the following constant to the target frequency
//
#define RC_FREQUENCY   80702           //  Target RC Frequency in Hz

// XFCN Setting Macro
#if  (RC_FREQUENCY <= 25000)
   #define XFCN 0
#elif(RC_FREQUENCY <= 50000)
   #define XFCN 1
#elif(RC_FREQUENCY <= 100000)
   #define XFCN 2
#elif(RC_FREQUENCY <= 200000)
   #define XFCN 3
#elif(RC_FREQUENCY <= 400000)
   #define XFCN 4
#elif(RC_FREQUENCY <= 800000)
   #define XFCN 5
#elif(RC_FREQUENCY <= 1600000)
   #define XFCN 6
#elif(RC_FREQUENCY <= 3200000)
   #define XFCN 7
#else
   #error "RC Frequency must be less than or equal to 3.2 MHz"
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

   while (1);                          // loop forever

}                                      // end of main()

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
   int i;                              // delay counter

   OSCXCN = (0x40 | XFCN);             // start external oscillator with
                                       // the appropriate XFCN setting
                                       // based on RC frequency

   RSTSRC = 0x06;                      // Enable missing clock detector and
                                       // VDD Monitor reset



   OSCICN = 0x08;                      // Disable the internal oscillator.
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